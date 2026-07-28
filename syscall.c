    
#include "syscall.h"
#include <stdint.h>
#include <stdio.h>

#include <stm32f1xx_hal.h>

#define TOTAL_BIT_LENGTH 32

// Extract Action Type (Bits 31-28, 4 bits)
#define GET_ACTION_TYPE(x) (((x) >> 28) & 0x0F)

// Extract Peripheral ID (Bits 27-24, 4 bits)
#define GET_PERIPHERAL(x) (((x) >> 24) & 0x0F)

// Extract Port ID (Bits 23-20, 4 bits)
#define GET_PORT_INDEX(x) (((x) >> 20) & 0x0F)

// Extract Pin Number (Bits 19-16, 4 bits)
#define GET_PIN_INDEX(x) (((x) >> 16) & 0x0F)

// Extract Direction (Bits 15-12, 4 bits)
#define GET_DIRECTION(x) (((x) >> 12) & 0x0F)

// Extract Argument 1 (Bits 11-8, 4 bits)
#define GET_ARG_ONE(x) (((x) >> 8) & 0x0F)

// Extract Argument 2 (Bits 7-4, 4 bits)
#define GET_ARG_TWO(x) (((x) >> 4) & 0x0F)

// Extract Argument 3 (Bits 3-0, 4 bits)
#define GET_ARG_THREE(x) ((x) & 0x0F)

#define UPDATE_ARG_ONE(cmd, value)                                             \
    do                                                                         \
    {                                                                          \
        (cmd) = ((cmd) & ~(0x0Fu << 8)) | ((((uint32_t)(value)) & 0x0F) << 8); \
    } while (0)

#define UPDATE_ARG_TWO(cmd, value)                                             \
    do                                                                         \
    {                                                                          \
        (cmd) = ((cmd) & ~(0x0Fu << 4)) | ((((uint32_t)(value)) & 0x0F) << 4); \
    } while (0)

#define UPDATE_ARG_THREE(cmd, value)                             \
    do                                                           \
    {                                                            \
        (cmd) = ((cmd) & ~0x0Fu) | (((uint32_t)(value)) & 0x0F); \
    } while (0)

    /*
    31      28 27    24 23    20 19    16 15    12 11     8 7      4 3      0
    +----------+--------+--------+--------+--------+--------+--------+--------+
    | Action   | Periph | Port   | Pin    | Arg1   | Arg2   | Arg3   | Arg4   |
    | 4 bits   | 4 bits | 4 bits | 4 bits | 4 bits | 4 bits | 4 bits | 4 bits |
    +----------+--------+--------+--------+--------+--------+--------+--------+
    */

#define PIN_MAP(x) (0x00000001u << x)

uint32_t result = 0x00u, action = 0x0u;

typedef void (*FunctionGeneric)(uint32_t);

FunctionGeneric initTable[] = {
    gpio_init
};

FunctionGeneric writeActionTable[] = {
    gpio_write,
};

FunctionGeneric readActionTable[] = {
    gpio_read
};

I2C_HandleTypeDef hi2c1;

enum DIRECTION
{
    ACTION_INIT = 0,
    ACTION_WRITE,
    ACTION_READ,
    ACTION_STREAM_WRITE,
    ACTION_STREAM_READ,
    ACTION_DMA,
    ACTION_CALLBACK
};

enum PortType
{
    PORT_A = 0,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E
};

static const uint32_t GPIO_PULL_MAP[] = {
    GPIO_NOPULL,
    GPIO_PULLUP,
    GPIO_PULLDOWN,
};

static const uint32_t GPIO_MODE_MAP[] = {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT_PP,
    GPIO_MODE_OUTPUT_OD,
    GPIO_MODE_AF_PP,
    GPIO_MODE_AF_OD,
    GPIO_MODE_AF_INPUT,
    GPIO_MODE_ANALOG,
    GPIO_MODE_IT_RISING,
    GPIO_MODE_IT_FALLING,
    GPIO_MODE_IT_RISING_FALLING,
    GPIO_MODE_EVT_RISING,
    GPIO_MODE_EVT_FALLING,
    GPIO_MODE_EVT_RISING_FALLING
};

/*
31-28  Peripheral
27-24  Instance
23-20  Port
19-16  Pin
15-12  Mode
11-8   Option1
7-4    Option2
3-0    Option3
*/


// init gpio porta pin3 ouput pull up
// write gpio portb pin4 high


GPIO_TypeDef *getGPIOPort(uint8_t port)
{
    switch (port)
    {
    case PORT_A:
        return GPIOA;
    case PORT_B:
        return GPIOB;
    case PORT_C:
        return GPIOC;
    case PORT_D:
        return GPIOD;
#ifdef GPIOE
    case PORT_E:
        return GPIOE;
#endif
    }

    return GPIOA;
}

void enableGPIOClock(uint8_t port)
{
    switch (port)
    {
    case PORT_A:
        __HAL_RCC_GPIOA_CLK_ENABLE();
        break;

    case PORT_B:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;

    case PORT_C:
        __HAL_RCC_GPIOC_CLK_ENABLE();
        break;

    case PORT_D:
        __HAL_RCC_GPIOD_CLK_ENABLE();
        break;

#ifdef GPIOE
    case PORT_E:
        __HAL_RCC_GPIOE_CLK_ENABLE();
        break;
#endif
    }
}

// void i2c_init(uint32_t action)
// {
//     uint8_t bus = GET_ARG_ONE(action);

//     HAL_I2C_Init(i2cTable[bus]);
// }

// void i2c_write(uint32_t action)
// {
//     uint8_t bus = GET_ARG_ONE(action);

//     uint8_t address = GET_ARG_TWO(action);

//     uint8_t length = GET_ARG_THREE(action);
//     uint8_t txData = GET_ARG_THREE(action);

//     HAL_I2C_Master_Transmit(
//         i2cTable[bus],
//         address << 1,
//         txBuffer,
//         length,
//         HAL_MAX_DELAY);
// }

void gpio_init(action){
    uint8_t portIndex = GET_PORT_INDEX(action);
    uint8_t pinIndex = GET_PIN_INDEX(action);
    uint8_t modeIndex = GET_ARG_ONE(action);
    uint8_t pullIndex = GET_ARG_TWO(action);

    enableGPIOClock(portIndex);

    GPIO_TypeDef *currentPort = getGPIOPort(portIndex);

    HAL_GPIO_WritePin(currentPort, PIN_MAP(pinIndex), GPIO_PIN_RESET);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PIN_MAP(pinIndex);
    GPIO_InitStruct.Mode = GPIO_MODE_MAP[modeIndex];
    GPIO_InitStruct.Pull = GPIO_PULL_MAP[pullIndex];
    HAL_GPIO_Init(currentPort, &GPIO_InitStruct);
}

void init_action(uint32_t action)
{
    initTable[GET_PERIPHERAL(action)](action);
}

void write_action(uint32_t action)
{
    writeActionTable[GET_PERIPHERAL(action)](action);
}

void read_action(uint32_t *action)
{
    readActionTable[GET_PERIPHERAL(*action)](action);
}

void gpio_write(uint32_t action)
{
    //can be optimized
    uint8_t portIndex = GET_PORT_INDEX(action);
    uint8_t pinIndex = GET_PIN_INDEX(action);
    uint8_t state = GET_ARG_ONE(action);

    HAL_GPIO_WritePin(getGPIOPort(portIndex), PIN_MAP(pinIndex),  state);
}

void gpio_read(uint32_t action)
{
    // can be optimized
    uint8_t portIndex = GET_PORT_INDEX(action);
    uint8_t pinIndex = GET_PIN_INDEX(action);
    GPIO_PinState state = HAL_GPIO_ReadPin(getGPIOPort(portIndex), PIN_MAP(pinIndex));
    UPDATE_ARG_ONE(result, state);
}


void system()
{
    switch(GET_ACTION_TYPE(action)){

        case ACTION_INIT:
            init_action(action);
            break;
        case ACTION_WRITE:
            write_action(action);
            break;

        case ACTION_READ:
            read_action(action);
            break;
        
        default:
            break;
        }

        action = 0;
}



int main()
{

    return 0;
}
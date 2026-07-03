#include <stdio.h>
#include "stdint.h"

uint8_t R1 = 0x00;
uint8_t R2 = 0x00;
uint8_t R3 = 0x00;
uint8_t R4 = 0x00;

typedef struct Screen{

} Screen;

void sysCall(){
    switch (R1)
    {
        case 0x01:
            // HAL_gpio(GPIOA, PIN_13, HAL_GPIO_RESET);
            break;
        
        default:
            break;
    }
}
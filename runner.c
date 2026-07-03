#include <stdio.h>
#include <stdint.h>

#define RAM_SIZE 0xffff // 1024*8 bits (1KB)
#define STACK_CAPACITY 50

#define INS_HLT 0x10
#define INS_JUMP 0x20
#define INS_STORE 0x30
#define INS_LOAD 0x40
#define INS_ADD 0x50
#define INS_MOVE 0x60
#define INS_GET_ADDRESS 0x70
#define INS_JUMP_IF_ZERO 0x80
#define INS_INC 0x90
#define INS_DEC 0xA0
#define INS_SYSCALL 0xB0
#define INS_PUSH 0xC0
#define INS_POP 0xD0
#define INS_PUSH_IF_ZERO 0xE0
#define INS_LOAD_OF_ADDRESS 0x18
#define INS_SUBTRACT 0x28
#define INS_CLEAR_ACC 0x38
#define INS_NOP 0x48
#define INS_COMP 0x58
#define INS_JUMP_IF_LESS 0x68
#define INS_PUSH_IF_LESS 0x78
#define INS_OR 0x88
#define INS_AND 0x98
#define INS_LEFT_SHIFT 0xa8
#define INS_RIGHT_SHIFT 0xb8




#define INS_DATA_ADDRESS 0x04
#define INS_R1 0x00
#define INS_R2 0x01
#define INS_R3 0x02
#define INS_R4 0x03

#define SYS_STREAM 0x04

#define SYS_PRINT 0x00
#define SYS_SCAN 0x01

uint8_t R1 = 0x00; // First Register
uint8_t R2 = 0x00; // Second Register
uint8_t R3 = 0x00; // Second Register
uint8_t R4 = 0x00; // Second Register
uint8_t AR = 0x00; // Address Register
uint8_t AC = 0x00; // Accumulator
uint8_t PC = 0x00;
uint16_t IR = 0x00;
uint8_t SP = 0x00;
uint16_t SL = 0x00; // STACK_LENGTH
uint16_t RAM[RAM_SIZE];

enum FLAG
{
    ZERO,
    NEGATIVE,
    CARRY
};

uint8_t flags[3] = {0, 0, 0};


void execute()
{
    IR = RAM[PC];
    uint16_t instruction = (IR & 0xff00) >> 8;
    uint8_t data = IR & 0x00ff;

    switch (instruction)
    {

    case INS_SYSCALL:
        systemCall();
        break;

    case INS_DEC | INS_DATA_ADDRESS: // DEC
        RAM[data] = RAM[data] - 1;
        break;

    case INS_INC | INS_DATA_ADDRESS:
        RAM[data] = RAM[data] + 1;
        break;

    case INS_GET_ADDRESS | INS_DATA_ADDRESS | INS_R1:
        R1 = data;
        break;

    case INS_GET_ADDRESS | INS_DATA_ADDRESS | INS_R2:
        R2 = data;
        break;

    case INS_GET_ADDRESS | INS_DATA_ADDRESS | INS_R3:
        R3 = data;
        break;

    case INS_GET_ADDRESS | INS_DATA_ADDRESS | INS_R4:
        R4 = data;
        break;

    case INS_STORE | INS_DATA_ADDRESS | INS_R1:
        RAM[data] = R1;
        R1 = 0;
        break;

    case INS_STORE | INS_DATA_ADDRESS | INS_R2:
        RAM[data] = R2;
        R2 = 0;
        break;

    case INS_STORE | INS_DATA_ADDRESS | INS_R3:
        RAM[data] = R3;
        R3 = 0;
        break;

    case INS_STORE | INS_DATA_ADDRESS | INS_R4:
        RAM[data] = R4;
        R3 = 0;
        break;

    case INS_LOAD | INS_R1:
        R1 = data;
        break;

    case INS_LOAD | INS_R2:
        R2 = data;
        break;

    case INS_LOAD | INS_R3:
        R3 = data;
        break;

    case INS_LOAD | INS_R4:
        R4 = data;
        break;

    case INS_LOAD | INS_DATA_ADDRESS | INS_R1:
        R1 = RAM[data];
        break;

    case INS_LOAD | INS_DATA_ADDRESS | INS_R2:
        R2 = RAM[data];
        break;

    case INS_LOAD | INS_DATA_ADDRESS | INS_R3:
        R3 = RAM[data];
        break;

    case INS_LOAD | INS_DATA_ADDRESS | INS_R4:
        R4 = RAM[data];
        break;

    case INS_LOAD_OF_ADDRESS | INS_DATA_ADDRESS | INS_R1:
        R1 = RAM[RAM[data]];
        break;

    case INS_LOAD_OF_ADDRESS | INS_DATA_ADDRESS | INS_R2:
        R2 = RAM[RAM[data]];
        break;

    case INS_LOAD_OF_ADDRESS | INS_DATA_ADDRESS | INS_R3:
        R3 = RAM[RAM[data]];
        break;

    case INS_LOAD_OF_ADDRESS | INS_DATA_ADDRESS | INS_R4:
        R4 = RAM[RAM[data]];
        break;

    case INS_JUMP | INS_DATA_ADDRESS:
        PC = data;
        break;

    case INS_JUMP_IF_ZERO | INS_DATA_ADDRESS: // JUPZ
        PC = flags[ZERO] ? data : PC;
        break;

    case INS_JUMP_IF_LESS | INS_DATA_ADDRESS: // JUPN
        PC = flags[NEGATIVE] ? data : PC;
        break;

    case INS_ADD | INS_R1: // ADR1
        AC += R1;
        break;

    case INS_ADD | INS_R2: // ADR1
        AC += R2;

        break;

    case INS_ADD | INS_R3: // ADR1
        AC += R3;

        break;
    case INS_ADD | INS_R4: // ADR1
        AC += R4;
        break;

    case INS_SUBTRACT | INS_R1: // SBR1
        AC -= R1;
        break;

    case INS_SUBTRACT | INS_R2: // SBR2
        
        printf("Accumulator..[R2]=[%d] [AC]=[%d]\n", R2, AC);
        AC -= R2;
        printf("Accumulator..[R2]=[%d] [AC]=[%d]\n", R2, AC);

        break;

    case INS_SUBTRACT | INS_R3: // SBR3
        AC -= R3;

        break;

    case INS_SUBTRACT | INS_R4: // SBR4
        AC -= R4;
        break;

    case INS_OR | INS_R1: // SBR4
        AC |= R1;
        break;
    case INS_OR | INS_R2: // SBR4
        AC |= R2;
        break;
    case INS_OR | INS_R3: // SBR4
        AC |= R3;
        break;
    case INS_OR | INS_R4: // SBR4
        AC |= R4;
        break;

    case INS_AND | INS_R1: // SBR4
        AC &= R1;
        break;
    case INS_AND | INS_R2: // SBR4
        AC &= R2;
        break;
    case INS_AND | INS_R3: // SBR4
        AC &= R3;
        break;
    case INS_AND | INS_R4: // SBR4
        AC &= R4;
        break;

    case INS_LEFT_SHIFT | INS_DATA_ADDRESS | INS_R1:
        RAM[data] = RAM[data] << R1;
        break;

    case INS_LEFT_SHIFT | INS_DATA_ADDRESS | INS_R2:
        RAM[data] = RAM[data] << R2;
        break;
    case INS_LEFT_SHIFT | INS_DATA_ADDRESS | INS_R3:
        RAM[data] = RAM[data] << R3;
        break;
    case INS_LEFT_SHIFT | INS_DATA_ADDRESS | INS_R4:
        RAM[data] = RAM[data] << R4;
        break;

    case INS_RIGHT_SHIFT | INS_DATA_ADDRESS | INS_R1:
        RAM[data] = RAM[data] << R1;
        break;

    case INS_RIGHT_SHIFT | INS_DATA_ADDRESS | INS_R2:
        RAM[data] = RAM[data] >> R2;
        break;
    case INS_RIGHT_SHIFT | INS_DATA_ADDRESS | INS_R3:
        RAM[data] = RAM[data] >> R3;
        break;
    case INS_RIGHT_SHIFT | INS_DATA_ADDRESS | INS_R4:
        RAM[data] = RAM[data] >> R4;
        break;

    case INS_CLEAR_ACC: // CLR
        AC = 0;
        flags[ZERO] = 1;
        flags[NEGATIVE] = 0;
        flags[CARRY] = 0;
        break;

    case INS_COMP | INS_DATA_ADDRESS | INS_R1:
        flags[ZERO] = R1 == RAM[data];
        flags[NEGATIVE] = R1 > RAM[data];
        printf("R1 = %#x, RAM[%#x]=%#x\n", R1, data, RAM[data]);
        break;

    case INS_COMP | INS_DATA_ADDRESS | INS_R2:
        flags[ZERO] = R2 == RAM[data];
        flags[NEGATIVE] = R2 > RAM[data];

        break;

    case INS_COMP | INS_DATA_ADDRESS | INS_R3:
        flags[ZERO] = R3 == RAM[data];

        // printf("R3 is %d and RAM[data] is %d\n", static_cast<int>(R3), static_cast<int>(RAM[data]));
        flags[NEGATIVE] = R3 > RAM[data];
        break;
    case INS_COMP | INS_DATA_ADDRESS | INS_R4:
        flags[ZERO] = R4 == RAM[data];
        flags[NEGATIVE] = R4 > RAM[data];
        break;

    case INS_COMP | INS_R1:
        flags[ZERO] = R1 == data;
        flags[NEGATIVE] = R1 > data;
        break;

    case INS_COMP | INS_R2:
        flags[ZERO] = R2 == data;
        flags[NEGATIVE] = R2 > data;
        break;

    case INS_COMP | INS_R3:
        flags[ZERO] = R3 == data;
        flags[NEGATIVE] = R3 > data;
        break;
    case INS_COMP | INS_R4:
        flags[ZERO] = R4 == data;
        flags[NEGATIVE] = R4 > data;
        break;

    case INS_CLEAR_ACC | INS_DATA_ADDRESS: // CLR at address
        RAM[data] = 0;
        break;

    case INS_MOVE | INS_DATA_ADDRESS:
        RAM[data] = AC;
        break;

    case INS_NOP: // NOP

        break;

    case INS_PUSH | INS_DATA_ADDRESS: // PUSH
        SL++;
        check_overflow();
        RAM[SP + SL] = ((INS_JUMP | INS_DATA_ADDRESS) << 8) | (data - 1);
        break;

    case INS_PUSH_IF_LESS | INS_DATA_ADDRESS:
        if (flags[NEGATIVE])
        {
            SL++;
            check_overflow();
            RAM[SP + SL] = ((INS_JUMP | INS_DATA_ADDRESS) << 8) | (data - 1);
        }
        break;

    case INS_PUSH_IF_ZERO | INS_DATA_ADDRESS: // PUSZ
        if (flags[ZERO])
        {
            SL++;
            check_overflow();
            RAM[SP + SL] = ((INS_JUMP | INS_DATA_ADDRESS) << 8) | (data - 1);
        }

        break;

    case INS_POP: // POP
        PC = SP + SL - 1;
        SL--;

        break;

    default:
        // printf("Invalid %#x instruction found\n", instruction);
        break;
    }
}

void systemCall()
{
    int i = 0;
    char x;
    char text[40];
    // printf(
    //     "system called with %d\t R1 = %#x\n",
    //     static_cast<int>(IR & 0x00ff),
    //     static_cast<int>(R1));

    switch (IR & 0x00ff)
    {
    case SYS_PRINT:
    {
        break;
    }

    case SYS_SCAN:
    {

        break;
    }

    case SYS_PRINT | SYS_STREAM:
    {
        
        break;
    }

    case SYS_SCAN | SYS_STREAM:
    {

        
        break;
    }

    default:
        break;
    }
}

void saveProgram(uint16_t *program, uint16_t size){
    int i=0;
    while (i < size)
    {
        RAM[i] = program[i];
    }


}
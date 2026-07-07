

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define RAM_SIZE 0xffff
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
#define INS_LOAD_AS_ADDRESS 0x18
#define INS_SUBTRACT 0x28
#define INS_CLEAR_ACC 0x38
#define INS_NOP 0x48

#define INS_ADDRESS 0x04
#define INS_R1 0x00
#define INS_R2 0x01
#define INS_R3 0x02
#define INS_R4 0x03

#define MAX_FILE_LENGTH 1000

typedef struct CPU{
    uint8_t R1; // First Register
    uint8_t R2; // Second Register
    uint8_t R3; // Third Register
    uint8_t R4; // Fourth Register
    uint8_t AR; // Address Register
    uint8_t AC; // Accumulator
    uint8_t PC;
    uint16_t IR;
    uint8_t SP;
    uint16_t SL; // STACK_LENGTH

} CPU;


typedef enum {
    TOKEN_TYPE_INSTRUCTION,
    TOKEN_TYPE_REGISTER,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_FORMATTER,   
    TOKEN_TYPE_START
} TokenType;


typedef struct Token
{
    char value[20];
    TokenType type;
} Token;


void loadProgram(char *pProgram, const char *filename, int *actualLength)
{
    printf("filename %s\n", filename);
    FILE *fp;

    fp = fopen(filename, "r");

    if(fp == NULL){
        printf("Cannot open file!\n");
        return;
    }

    *(actualLength) = fread(pProgram, 1, MAX_FILE_LENGTH - 1, fp);

    pProgram[*(actualLength)] = '\0';

    fclose(fp);

}

int isOperator(char ch)
{

    return ch == '[' || ch == ']' || ch == '$' || ch == '+' || ch == '-' || ch == '=';
}

typedef struct Node
{
    Token token;
    struct Node *children[10];
    int childCount;
} Node;



void addChild(Node *parent, Node *child)
{
    parent->children[parent->childCount++] = child;
}

void tokenize(char* program, int programSize, Token* tokens,  int *tokenCount){

    char buffer[20];

    int tokenIndex = 0;

    int i = 0;
    while(i< programSize){
        while (program[i] == ' ')
        {
            i++;
        }

        if (isalnum(program[i]) || program[i] == '_')
        {

            memset(buffer, 0, sizeof(buffer));
            int bufferIndex = 0;

            while ((isalnum(program[i]) || program[i] == '_'))
            {
                buffer[bufferIndex++] = program[i];
                i++;
            }

            i--;
            buffer[bufferIndex++] = '\0';
            strcpy(tokens[tokenIndex].value, buffer);
            tokens[tokenIndex].type = TOKEN_TYPE_INSTRUCTION;
            tokenIndex++;

            //printf("TOKEN: \"%s\" \tTYPE alpha numeric\n", buffer);
        }
        else if (isOperator(program[i]))
        {
            memset(buffer, 0, sizeof(buffer));
            buffer[0] = program[i];
            buffer[1] = '\0';
            strcpy(tokens[tokenIndex].value, buffer);
            tokens[tokenIndex].type = TOKEN_TYPE_OPERATOR;
            tokenIndex++;
            //printf("TOKEN: \"%c\" \tTYPE operator\n", program[i]);
        }
        else if (program[i] == '\n')
        {
            memset(buffer, 0, sizeof(buffer));
            buffer[0] = program[i];
            buffer[1] = '\0';
            strcpy(tokens[tokenIndex].value, buffer);
            tokens[tokenIndex].type = TOKEN_TYPE_FORMATTER;
            tokenIndex++;
            //printf("TOKEN: \"NEW LINE\" \tTYPE New Line\n");
        }
        i++;
    }

    *(tokenCount) = tokenIndex;
}

void printToken(Token *tokens, int tokenCount)
{   int i = 0;
    while(i < tokenCount)
    //for (int i = 0; i < tokenCount; i++)
    {
        printf("[%d]=\t", i);

        printf("%s \t{", tokens[i].value[0] == '\n' ? "newline" : tokens[i].value);

        switch (tokens[i].type)
        {
        case TOKEN_TYPE_INSTRUCTION:
            printf("INSTRUCTION");
            break;

        case TOKEN_TYPE_REGISTER:
            printf("REGISTER");
            break;

        case TOKEN_TYPE_OPERATOR:
            printf("OPERATOR");
            break;

        case TOKEN_TYPE_IDENTIFIER:
            printf("IDENTIFIER");
            break;

        case TOKEN_TYPE_FORMATTER:
            printf("FORMATTER");
            break;

        default:
            printf("UNKNOWN");
            break;
        }

        printf("}\n\n");
        i++;
    }
}


Node *createNode(Token token)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->token = token;
    node->childCount = 0;
    return node;
}

void createTree(Node* root, Token* tokens, int tokenCount){

    Node* currentInstruction = NULL;
    Node* currentOperator = NULL;
    int i=0;
    while(i<tokenCount){

        Token token = tokens[i];

        if(token.type == TOKEN_TYPE_INSTRUCTION){
            Node* instructionNode = createNode(token);

            addChild(root, instructionNode);

            currentInstruction = instructionNode;
        }
        else if (token.type == TOKEN_TYPE_OPERATOR){
            if(currentInstruction != NULL){
                Node *operatorNode = createNode(token);
                addChild(currentInstruction, operatorNode);
                currentOperator = operatorNode;
            }
        }
        else if (token.type == TOKEN_TYPE_REGISTER)
        {
            if(currentInstruction != NULL && currentOperator != NULL){
                Node *registerNode = createNode(token);
                addChild(currentOperator, registerNode);
            }
            else if (currentInstruction != NULL){
                Node *registerNode = createNode(token);
                addChild(currentInstruction, registerNode);
            }
        }else if(token.type == TOKEN_TYPE_FORMATTER){
            currentInstruction = NULL;
            currentOperator = NULL;
        }
        i++;
    }
}

void printTreeRecursive(Node *root, int depth)
{
    int i = 0;
     while (i < depth)
    {
        printf("\t");
        i++;
    }
   

    printf("%s\n", root->token.value);
    i = 0;
    while ( i < root->childCount)
    {
        printTreeRecursive(
            root->children[i],
            depth + 1);
        i++;
    }
}

    void printTree(Node *root)
    {

        printTreeRecursive(root, 0);
    }


int main()
{
    char program[MAX_FILE_LENGTH];

    int programSize;
    loadProgram(program, "image.os", &programSize);

    printf("program: %s\n", program);

    Token tokens[100];
    int tokenCount;

    tokenize(program, programSize, tokens, &tokenCount);

    printToken(tokens, tokenCount);


    // Token rootToken;
    // strcpy(rootToken.value, "start");
    // rootToken.type = TOKEN_TYPE_START;

    // Node *rootNode = createNode(rootToken);

    // printf("Token count: %d\n", tokenCount);

    // createTree(rootNode, tokens, tokenCount);

    // printTree(rootNode);

    return 0;
}

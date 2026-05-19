#include <iostream>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string>
#include <map>
using namespace std;

#define RAM_SIZE 0xffff  //1024*8 bits (1KB)
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


class CPU
{
public:
    uint8_t R1 = 0x00; // First Register
    uint8_t R2 = 0x00; // Second Register
    uint8_t R3 = 0x00; // Second Register
    uint8_t R4 = 0x00; // Second Register
    uint8_t AR = 0x00; // Address Register
    uint8_t AC = 0x00; // Accumulator
    uint8_t PC = 0x00;
    uint16_t IR = 0x00;
    uint8_t SP = 0x00;
    uint16_t SL = 0x00; //STACK_LENGTH


    map<string, uint16_t> instructionMap;

    enum FLAG
    {
        ZERO,
        NEGATIVE,
        CARRY
    };

    uint8_t flags[3] = {0, 0, 0};

    uint16_t RAM[RAM_SIZE];

    CPU()
    {
        initMap();

        for (int address = 0; address < 0xFF; ++address)
        {
            RAM[address] = 0;
        }
    }

    void initMap()
    {
        // mapping =
        instructionMap["HLT"] = INS_HLT;
        instructionMap["STORE"] = INS_STORE; 
        instructionMap["LOAD"] = INS_LOAD; 
        instructionMap["LFAD"] = INS_LOAD_OF_ADDRESS;
        instructionMap["JUMP"] = INS_JUMP;
        instructionMap["JUPZ"] = INS_JUMP_IF_ZERO;
        instructionMap["JUPN"] = INS_JUMP_IF_LESS;
        instructionMap["ADD"] = INS_ADD;
        instructionMap["SUB"] = INS_SUBTRACT;
        instructionMap["CLR"] = INS_CLEAR_ACC; 
        instructionMap["MOV"] = INS_MOVE;
        instructionMap["PUSH"] = INS_PUSH;
        instructionMap["PUSZ"] = INS_PUSH_IF_ZERO;
        instructionMap["PUSN"] = INS_PUSH_IF_LESS;
        instructionMap["POP"] = INS_POP; 
        instructionMap["NOP"] = INS_NOP;  
        instructionMap["AOR"] = INS_GET_ADDRESS; 
        instructionMap["INC"] = INS_INC;
        instructionMap["DEC"] = INS_DEC;
        instructionMap["SYSCALL"] = INS_SYSCALL;
        instructionMap["COMP"] = INS_COMP;
        instructionMap["OR"] = INS_OR;
        instructionMap["AND"] = INS_AND;
    }

    void translateProgramAt(uint16_t i)
    {
        uint8_t instruction = ((RAM[i] >> 10) & 0x00ff) << 2;
        uint8_t middle = ((RAM[i] >> 8) & 0x03) + 1;
        uint8_t secondHalf = RAM[i] & 0x00ff;

        for (auto &pair : instructionMap)
        {
            if ((instructionMap[pair.first] == instruction))
            {
                printf("%#x: %s%d %#x\n",
                       static_cast<int>(i),
                       pair.first.c_str(), middle,
                       static_cast<int>(secondHalf));
                break;
            }
            else if (((instructionMap[pair.first] | INS_DATA_ADDRESS) == instruction))
            {
                printf("%#x: %s%d [%#x]\n",
                        static_cast<int>(i),
                        pair.first.c_str(), middle, static_cast<int>(secondHalf));
                break;
            }
        }
    }

    //0x43
    //0b01000011

    void loadProgram(const string &programname)
    {

        fstream file(programname);
        if (!file)
        {
            cout << "Error opening file\n";
        }

        string line;
        string program = "";
        uint8_t lineCount = 0;
        while (getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }
            program += line + "\n";
        }

        cout << "Program Loaded:" << programname << endl
             << program;

        uint16_t *token = parse(program, &lineCount);

        uint8_t i;
        for (i = 0; i < lineCount; i++)
        {
            RAM[i] = token[i];
        }

        SP = i;


        //printf("Stack pointer is pointing to : %#x\n", i);

                cout
            << "\n"
            << "================Translated program ==========\n"
            << endl;


        for (i = 0; i < lineCount; i++)
        {
            translateProgramAt(i);
        }

        try
        {
            // file.close();
        }
        catch (const exception &ecvt)
        {
            cout << "Some error occured\n";
        }

        cout << "Reading finished" << endl;

        run();
        showMemory(0x00, 0xFF);
    }


    uint8_t getRegister(char ch)
    {

        uint8_t register_value;

        switch (ch)
        {
        case '1':
            register_value = INS_R1;
            break;

        case '2':
            register_value = INS_R2;
            break;
        case '3':
            register_value = INS_R3;
            break;
        case '4':
            register_value = INS_R4;
            break;

        default:
            throw;
            break;
        }

        return register_value;
    }

    // parsing
    uint16_t *parse(string &program, uint8_t *pLineCount)
    {
        map<string, uint16_t> funtionMap;
        map<string, uint16_t> variableMap;
        map<uint16_t, string> variableLineNumberMap;
        map<string, string> arrayMap;

        uint16_t *binaries = new uint16_t(100);
        uint16_t binaryIndex = 2;

        string buffer = "";
        string currentInstruction = "";
        string middleData = "";
        string currentData = "";
        string currentAddress = "";
        string currentFunction = "";

        bool functionStarted = false;

        uint16_t variable_counter = 1;

        for (int i = 0; i < program.length(); i++)
        {
            if(program[i] == ';'){
                while (program[++i] != '\n'){
                    
                }
            }


            if(program[i] == '\t'){
                continue;
            }

            if (program[i] == ' ' || program[i] == '\n' || program[i] == ':')
            {

                if (buffer == "END")
                {
                    functionStarted = false;
                    buffer.clear();
                    *(binaries + binaryIndex++) = ((INS_POP) << 8);
                    continue;
                }

                if (buffer == "CALL")
                {
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    while (1)
                    {
                        // cout << "char: \"" << program[i+1] << "\"" << endl;
                        if (program[++i] == '\n')
                        {
                            if (!isKeyAvailable(buffer, funtionMap))
                            {
                                throw runtime_error("Function \"" + buffer + "\" Not found");
                            }
                            // cout << "JUMP Found function name: \"" << buffer << "\"" << endl;
                            *(binaries + binaryIndex) = ((INS_PUSH | INS_DATA_ADDRESS) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((INS_JUMP | INS_DATA_ADDRESS) << 8) | funtionMap[buffer];
                            break;
                        }
                        else
                        {
                            string s(1, program[i]);
                            buffer += s;
                        }
                    }

                    buffer.clear();
                    continue;
                }

                if (buffer == "CALZ")
                {
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    while (1)
                    {
                        // cout << "char: \"" << program[i+1] << "\"" << endl;
                        if (program[++i] == '\n')
                        {
                            if (!isKeyAvailable(buffer, funtionMap))
                            {
                                throw runtime_error("Function \"" + buffer + "\" Not found");
                            }
                            // cout << "JUMP Found function name: \"" << buffer << "\"" << endl;
                            *(binaries + binaryIndex) = ((INS_PUSH_IF_ZERO | INS_DATA_ADDRESS) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((INS_JUMP_IF_ZERO | INS_DATA_ADDRESS) << 8) | funtionMap[buffer];
                            break;
                        }
                        else
                        {
                            string s(1, program[i]);
                            buffer += s;
                        }
                    }

                    buffer.clear();
                    continue;
                }

                if (buffer == "CALN")
                {
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    while (1)
                    {
                        // cout << "char: \"" << program[i+1] << "\"" << endl;
                        if (program[++i] == '\n')
                        {
                            if (!isKeyAvailable(buffer, funtionMap))    
                            {
                                throw runtime_error("Function \"" + buffer + "\" Not found");
                            }
                            // cout << "JUMP Found function name: \"" << buffer << "\"" << endl;
                            *(binaries + binaryIndex) = ((INS_PUSH_IF_LESS | INS_DATA_ADDRESS) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((INS_JUMP_IF_LESS | INS_DATA_ADDRESS) << 8) | funtionMap[buffer];
                            break;
                        }
                        else
                        {
                            string s(1, program[i]);
                            buffer += s;
                        }
                    }

                    buffer.clear();
                    continue;
                }

                
                if(buffer == "DCLR"){
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    while (1)
                    {
                        // cout << "char: \"" << program[i+1] << "\"" << endl;
                        if (program[++i] == '\n')
                        {
                            string v_name = buffer.substr(1, buffer.length() - 1);
                            int startIndex = v_name.find("[");
                            int endIndex = v_name.find("]");
                            //printf("Vriable: %s start: %d, end : %d\n", v_name.c_str(), startIndex, endIndex);

                            if (startIndex != -1 && endIndex != -1)
                            {
                                string v_name_without_arr = v_name.substr(0, startIndex);

                                if (!isKeyAvailable(v_name_without_arr, variableMap))
                                {
                                    int size = stoi(v_name.substr(startIndex + 1, endIndex - startIndex - 1));
                                    variableMap[v_name_without_arr] = variable_counter;
                                    //printf("Buffer: \"%s\" Variable \"%s\" = declared on line number %#x:  address will be offset by [%#x] \n", buffer.c_str(), v_name_without_arr.c_str(), binaryIndex, variable_counter);
                                    variable_counter += size;
                                }else {
                                    throw runtime_error("Variable \"" + buffer + "\" Already declared");
                                }
                            }else if (!isKeyAvailable(buffer, variableMap))
                            {
                                //printf("Buffer: \"%s\" Variable \"%s\" = declared on line number %#x:  address will be offset by [%#x] \n", buffer.c_str(), buffer.substr(1, buffer.length() - 1).c_str(), binaryIndex, variable_counter);
                                variableMap[v_name] = variable_counter++;
                                
                                // *(binaries + binaryIndex) = ((instructionMap["NOP"] | 0x10) << 8);
                                // binaryIndex++;
                            }else {
                                throw runtime_error("Variable \"" + buffer + "\" Already declared");
                            }

                            // variableLineNumberMap[binaryIndex] = variable_counter;
                            // //printf("Variable %s = line number %#x:  address [%#x] \n", buffer.substr(1, buffer.length() - 1).c_str(), binaryIndex, variable_counter);
                            // variable_counter++;

                            break;
                        }
                        else
                        {
                            string s(1, program[i]);
                            buffer += s;
                        }
                    }

                    buffer.clear();
                    continue;
                }

                if (buffer.substr(0, 2) == "0x" || buffer.substr(0, 2) == "0X")
                {
                    currentData = buffer.substr(2, buffer.length() - 1);
                }
                else if (buffer.substr(0, 1) == "%")
                {
                    middleData = buffer.substr(1, 3);
                    //printf("Register found! R%c\n", buffer[2]);
                    buffer.clear();
                }
                else if (buffer.substr(0, 1) == "[")
                {
                    int indexOf = buffer.find("]");
                    if (indexOf == -1)
                    {
                        throw;
                    }
                    currentAddress = buffer.substr(3, indexOf - 1);
                }
                else if (buffer.substr(0, 1) == "\'" && buffer.substr(2, 3) == "\'")
                {
                    int value = static_cast<int>(buffer[1]);
                    stringstream ss;
                    ss << hex << value;
                    currentData =  "0x" +  ss.str();
                    buffer.clear();
                }
                
                else if (program[i] == ':')
                {
                    currentFunction = buffer;

                    if (!isKeyAvailable(buffer, funtionMap))
                    {
                        if (functionStarted)
                        {
                            throw runtime_error("Function not ended near: " + to_string(binaryIndex + 1));
                        }
                        funtionMap[buffer] = binaryIndex;
                        if (buffer == "_main")
                        {
                            *(binaries + 1) = ((INS_JUMP | INS_DATA_ADDRESS) << 8) | binaryIndex;
                        }
                        //printf("FUNCTION STARTED AND Address assigned: %s %#x\n", buffer.c_str(), static_cast<int>(binaryIndex));

                        functionStarted = true;
                    }

                    buffer.clear();
                }
                else if(buffer.substr(0, 1) == "$"){
                    
                    string v_name = buffer.substr(1, buffer.length() - 1);
                    int startIndex = v_name.find("[");
                    int endIndex = v_name.find("]");

                    if (startIndex != -1 && endIndex != -1)
                    {
                        string v_name_without_arr = v_name.substr(0, startIndex);

                        //printf("Using array %s\n", v_name_without_arr.c_str());

                        if (isKeyAvailable(v_name_without_arr, variableMap))
                        {
                            int index = stoi(v_name.substr(startIndex + 1, endIndex - startIndex - 1));

                            currentAddress = to_string(variableMap[v_name_without_arr] + index);
                            //printf("index: %d,  variableMap[v_name_without_arr]: %d\n", index, variableMap[v_name_without_arr]);
                            variableLineNumberMap[binaryIndex] = v_name_without_arr;
                        }else {
                            throw runtime_error("Array not found \"" + v_name_without_arr + "\" not found");
                        }
                    }
                    else if (isKeyAvailable(v_name, variableMap))
                    {
                        currentAddress = to_string(variableMap[v_name]);
                        variableLineNumberMap[binaryIndex] = v_name; 
                    }
                    else
                    {
                        throw runtime_error("Variable \"" + v_name + "\" not found");
                    }
                }else{
                    currentInstruction = buffer;
                    middleData.clear();
                    currentData.clear();
                    currentAddress.clear();
                }

                // //printf(" [%s, %s, %s]\n", currentInstruction.c_str(), middleData.c_str(), currentAddress.c_str());

                if (!currentFunction.empty())
                {
                    *(binaries + binaryIndex) = ((INS_NOP | INS_DATA_ADDRESS) << 8) | funtionMap[currentFunction];
                    binaryIndex++;
                    currentFunction.clear();
                }
                else if (!currentInstruction.empty() && !currentData.empty() && !middleData.empty())
                {
                    
                    *(binaries + binaryIndex) = (instructionMap[currentInstruction] | getRegister(middleData[1])) << 8 |  (stoi(currentData, nullptr, 16));
                    binaryIndex++;
                    currentInstruction.clear(); 
                    middleData.clear();
                    currentData.clear();
                }
                else if (!currentInstruction.empty() && !currentAddress.empty() && !middleData.empty())
                {
                    // //printf("Middledata %c", middleData[1]);
                    *(binaries + binaryIndex) = (((instructionMap[currentInstruction] | INS_DATA_ADDRESS) | getRegister(middleData[1])) << 8)  | stoi(currentAddress, nullptr, 16);
                    binaryIndex++;
                    currentInstruction.clear();
                    middleData.clear();
                    currentAddress.clear();
                }

                else if (!currentInstruction.empty() && !middleData.empty() && program[i] == '\n')
                {
                    // //printf("Middledata %c", middleData[1]);
                    *(binaries + binaryIndex) = (((instructionMap[currentInstruction]) | getRegister(middleData[1])) << 8) | 0x00;
                    binaryIndex++;
                    currentInstruction.clear();
                    middleData.clear();
                    currentData.clear();
                }

                else if (!currentInstruction.empty() && !currentData.empty())
                {
                    *(binaries + binaryIndex) = (instructionMap[currentInstruction] << 8) | (stoi(currentData, nullptr, 16));
                    binaryIndex++;
                    currentInstruction.clear();
                    currentData.clear();
                }
                else if (!currentInstruction.empty() && !currentAddress.empty())
                {
                    *(binaries + binaryIndex) = ((instructionMap[currentInstruction] | INS_DATA_ADDRESS) << 8) | stoi(currentAddress, nullptr, 16);
                    binaryIndex++;
                    currentInstruction.clear();
                    currentAddress.clear();
                }
                else if (!currentInstruction.empty() && program[i] == '\n')
                {
                    *(binaries + binaryIndex) = instructionMap[currentInstruction] << 8;
                    binaryIndex++;
                    currentInstruction.clear();
                    currentAddress.clear();
                }

                buffer.clear();

                continue;
            }

            string s(1, program[i]);
            buffer += s;
        }


        for (auto &pair : variableLineNumberMap)
        {
            uint16_t lineNumber = pair.first;
            uint16_t oldOffset = (*(binaries + lineNumber) & 0x00ff);
            *(binaries + lineNumber) = (*(binaries + lineNumber ) & 0xff00) | ((oldOffset + binaryIndex + STACK_CAPACITY + 1));
        }


        int programSize =  binaryIndex + variable_counter + STACK_CAPACITY;

        printf("Program size is  %d bytes (%f%%)\n",programSize,  (float)(programSize * 100) / RAM_SIZE);

        *(pLineCount) = ++binaryIndex;

        return binaries;
    }

    bool isKeyAvailable(string &key, map<string, uint16_t> &map)
    {
        for (auto &pair : map)
        {
            if (key == pair.first)
            {
                return true;
            }
        }

        return false;
    }

    bool isKeyAvailable(string &key, map<string, string> &map)
    {
        for (auto &pair : map)
        {
            if (key == pair.first)
            {
                return true;
            }
        }

        return false;
    }

    void run()
    {
        while (RAM[++PC] & 0xff00)
        {

            // //printf("Pointing to : %#x\n", static_cast<int>(PC));
                
                execute();
        }

        //printf("\n\nProgramm ended at: %#x\n", static_cast<int>(PC));
    }

    void execute()
    {
        IR = RAM[PC];
        uint16_t instruction = (IR & 0xff00) >> 8;
        uint8_t data = IR & 0x00ff;

        // //printf("-->");
        // int a;
        // scanf("\n%d", &a);
        // translateProgramAt(PC);

        //running

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

        case INS_SUBTRACT | INS_R1:  //SBR1
            AC -= R1;
            break;

        case INS_SUBTRACT | INS_R2: // SBR2

            printf("Accumulator..[R2]=[%d] [AC]=[%d]\n",R2, AC);
            AC -= R2;
            printf("Accumulator..[R2]=[%d] [AC]=[%d]\n",R2, AC);
            
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
            flags[NEGATIVE] =  0;
            flags[CARRY] = 0;
            break;

        case INS_COMP | INS_DATA_ADDRESS | INS_R1:
            flags[ZERO] = R1 == RAM[data];
            flags[NEGATIVE] = R1 > RAM[data];
            printf("R1 = %#x, RAM[%#x]=%#x\n", R1,data, RAM[data]);
            break;

        case INS_COMP | INS_DATA_ADDRESS | INS_R2:
            flags[ZERO] = R2 == RAM[data];
            flags[NEGATIVE] = R2 > RAM[data];

           
            break;

        case INS_COMP | INS_DATA_ADDRESS | INS_R3:
            flags[ZERO] = R3 == RAM[data];

            //printf("R3 is %d and RAM[data] is %d\n", static_cast<int>(R3), static_cast<int>(RAM[data]));
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
            //printf("Invalid %#x instruction found\n", instruction);
            break;
        }
    }

    void systemCall()
    {
        char x;
        char text[40];
        printf(
            "system called with %d\t R1 = %#x\n",
            static_cast<int>(IR & 0x00ff),
            static_cast<int>(R1));

        switch (IR & 0x00ff)
        {
        case SYS_PRINT:
        {
            printf("%c", RAM[R1]);
            break;
        }

        case SYS_SCAN:
        {

            scanf(" %c", &x);

            RAM[R1] = x;

            break;
        }

        case SYS_PRINT | SYS_STREAM:
        {
            for (int i = 0; i < R2; i++)
            {
                printf("%c", RAM[R1 + i]);
            }

            break;
        }

        case SYS_SCAN | SYS_STREAM:
        {
           
            scanf("%39s", text);

            for (
                int i = 0;
                i < R2 &&
                text[i] != '\0';
                i++)
            {
                RAM[R1 + i] = text[i];
            }

            break;
        }

        default:
            break;
        }
    }

    void showMemory(uint8_t start = 0x00, uint8_t limit = 0xFF)
    {
        printf("\n");
        for (int address = start; address <= limit; ++address)
        {
            printf("[%#x]:%#x\n", address, RAM[address]);
        }
    }

    void check_overflow(){
        if(SL > STACK_CAPACITY){
            throw runtime_error("Stack overflowed....\n");
        }
    }

    void initFlags(uint8_t register_value){
        flags[ZERO] = AC == 0;
        flags[NEGATIVE] = register_value > AC;
        // flags[CARRY] = result > 255;
    }

};

int main()
{

    CPU cpu;

    const string programs[4] = {"prog.os", "calc.os", "image.os", "condition.os"};

    printf("Available programs:\n");

    int size = sizeof(programs) / sizeof(programs[0]);

    for(int i=0; i<size; i++){
        printf("(%d). %s:\n", i + 1, programs[i].c_str());
    }

    int x;

    scanf("%d", &x);

    cpu.loadProgram(programs[x - 1]);
    
    int i;
    cin >> i;
    return 0;
}

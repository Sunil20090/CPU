#include <iostream>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <bitset>
#include <map>
using namespace std;

#define RAM_SIZE 0xffff
#define STACK_CAPACITY 0x32

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
        // instructionMap[cmd] =
        instructionMap["HLT"] = 0b00000001;  // 0x01
        instructionMap["STR1"] = 0b10000000; // 0x80
        instructionMap["STR2"] = 0b10000001; // 0x81
        instructionMap["LDR1"] = 0b01000000; // 0x40
        instructionMap["LDR2"] = 0b01000001; // 0x41
        instructionMap["JUMP"] = 0b00100000; // 0x20
        instructionMap["JUPZ"] = 0b00100001; // 0x31
        instructionMap["JUPN"] = 0b00100010; // 0x32
        instructionMap["ADR1"] = 0b00001000; // 0x08
        instructionMap["ADR2"] = 0b00001001; // 0x09
        instructionMap["SBR1"] = 0b00001010; // 0x0A
        instructionMap["SBR2"] = 0b00001011; // 0x0B
        instructionMap["PRNT"] = 0b00000101; // 0x05
        instructionMap["PRTC"] = 0b00000111; // 0x07
        instructionMap["CLR"] = 0b10100000;  // 0xA0
        instructionMap["MOVR"] = 0b11000000; // 0xD0
        instructionMap["PUSH"] = 0b01110000; // 0x70
        instructionMap["PUSZ"] = 0b01110011; // 0x73
        instructionMap["PUSN"] = 0b01110100; // 0x74
        instructionMap["POP"] = 0b01110001;  // 0x71
        instructionMap["NOP"] = 0b00010011;  // 0x13
    }

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

        cout << "Program :" << endl
             << program;

        uint16_t *token = parse(program, &lineCount);
        uint8_t i;
        for (i = 0; i < lineCount; i++)
        {
            RAM[i] = token[i];
        }

        SP = i;


        printf("Stack pointer is pointing to : %#x\n", i);

                cout
            << "\n"
            << "================Translated program ==========\n"
            << endl;


        for (i = 0; i < lineCount; i++)
        {
            uint8_t firstHalf = (RAM[i] >> 8) & 0x00ff;
            uint8_t secondHalf = RAM[i] & 0x00ff;

            for (auto &pair : instructionMap)
            {
                if ((instructionMap[pair.first] == firstHalf))
                {
                    printf("%#x: %s %#x\n",
                           static_cast<int>(i),
                           pair.first.c_str(),
                           static_cast<int>(secondHalf));
                    break;
                }
                else if (((instructionMap[pair.first] | 0x10) == firstHalf))
                {

                    printf("%#x: %s [%#x]\n",
                           static_cast<int>(i),
                           pair.first.c_str(),
                           static_cast<int>(secondHalf));
                    break;
                }
            }
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
        showMemory(0x9F);
    }

    uint16_t *parse(string &program, uint8_t *pLineCount)
    {
        map<string, uint16_t> funtionMap;
        map<string, string> variableMap;
        map<uint16_t, string> variableLineNumberMap;

        uint16_t *binaries = new uint16_t(100);
        uint16_t binaryIndex = 2;

        string buffer = "";

        string currentInstruction = "";
        string currentData = "";
        string currentAddress = "";
        string currentFunction = "";

        bool functionStarted = false;

        uint16_t variable_counter = 1;

        for (int i = 0; i < program.length(); i++)
        {

            if (program[i] == ' ' || program[i] == '\n' || program[i] == ':')
            {

                if (buffer == "END")
                {
                    functionStarted = false;

                    buffer.clear();
                    *(binaries + binaryIndex++) = ((instructionMap["POP"] | 0x10) << 8);
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
                            *(binaries + binaryIndex) = ((instructionMap["PUSH"] | 0x10) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((instructionMap["JUMP"] | 0x10) << 8) | funtionMap[buffer];
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
                            *(binaries + binaryIndex) = ((instructionMap["PUSZ"] | 0x10) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((instructionMap["JUPZ"] | 0x10) << 8) | funtionMap[buffer];
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
                            *(binaries + binaryIndex) = ((instructionMap["PUSN"] | 0x10) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((instructionMap["JUPN"] | 0x10) << 8) | funtionMap[buffer];
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
                            if (!isKeyAvailable(buffer, variableMap))
                            {
                                printf("Buffer: \"%s\" Variable \"%s\" = declared on line number %#x:  address will be offset by [%#x] \n", buffer.c_str(), buffer.substr(1, buffer.length() - 1).c_str(), binaryIndex, variable_counter);
                                variableMap[v_name] = to_string(variable_counter++);
                                // *(binaries + binaryIndex) = ((instructionMap["NOP"] | 0x10) << 8);
                                // binaryIndex++;
                            }else {
                                throw runtime_error("Variable \"" + buffer + "\" Already declared");
                            }

                            // variableLineNumberMap[binaryIndex] = variable_counter;
                            // printf("Variable %s = line number %#x:  address [%#x] \n", buffer.substr(1, buffer.length() - 1).c_str(), binaryIndex, variable_counter);
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

                if (buffer.substr(0, 2) == "0x")
                {
                    currentData = buffer.substr(2, buffer.length() - 1);
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
                            *(binaries + 1) = ((instructionMap["JUMP"] | 0x10) << 8) | binaryIndex;
                        }
                        printf("FUNCTION STARTED AND Address assigned: %s %#x\n", buffer.c_str(), static_cast<int>(binaryIndex));

                        functionStarted = true;
                    }

                    buffer.clear();
                }
                else if(buffer.substr(0, 1) == "$"){
                    string v_name = buffer.substr(1, buffer.length() - 1);
                    if (!isKeyAvailable(v_name, variableMap))
                    {
                        throw runtime_error("Variable \"" + v_name+ "\" not found");
                    }else {
                        currentAddress = variableMap[v_name];
                        variableLineNumberMap[binaryIndex] = v_name;

                        // printf("Current line number %d \n", static_cast<int>(binaryIndex));
                        // printf("current instruction %s Variable is used %s\n current variable data is \"%s\" \n", currentInstruction.c_str(), v_name.c_str(), currentAddress.c_str());
                    }
                }else{
                    currentInstruction = buffer;
                }

                if (!currentFunction.empty())
                {
                    *(binaries + binaryIndex) = ((instructionMap["NOP"] | 0x10) << 8) | funtionMap[currentFunction];
                    binaryIndex++;
                    currentFunction.clear();
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
                    *(binaries + binaryIndex) = ((instructionMap[currentInstruction] | 0x10) << 8) | stoi(currentAddress, nullptr, 16);
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
            string address = variableMap[pair.second];
            uint16_t lineNumber = pair.first;
            *(binaries + lineNumber) = *(binaries + lineNumber) & 0xff00 | (stoi(address, nullptr, 16) + binaryIndex + 2 + STACK_CAPACITY);
            printf("Variable %s used at line --> %d\n", pair.second.c_str(), pair.first);
        }

        cout << "Length of program: " << binaryIndex << endl;
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

            // printf("Pointing to : %#x\n", static_cast<int>(PC));
                execute();
        }

        printf("\n\nProgramm ended at: %#x\n", static_cast<int>(PC));
    }

    void execute()
    {
        IR = RAM[PC];
        uint16_t instruction = (IR & 0xff00) >> 8;
        uint8_t data = IR & 0x00ff;
        int result = 0;

        switch (instruction)
        {

        case 0b10000000: // STR1
            
            break;

        case 0b10000001: // STR2
            
            break;

        case 0b10010000: // STR1 adress
            RAM[data] = R1;
            break;
            
        case 0b10010001: // STR2 address
            RAM[data] = R2;
            break;

        case 0b01000000: // LDR1
            R1 = data;
            break;

        case 0b01000001: // LDR2
            R2 = data;
            break;

        case 0b01010000: // LDR1 address
            R1 = RAM[data];
            break;

        case 0b01010001: // LDR2 address
            R2 = RAM[data];
            break;

        case 0b00110000: // JUMP
            PC = data;
            break;

        case 0b00110001: // JUPZ
            
            
            PC = flags[ZERO] ? data : PC;
            break;

        case 0b00110010: // JUPN
            PC = flags[NEGATIVE] ? data : PC;
            break;

        case 0b00000101: // PRNT
            printf("OUT - %x\n", data);
            break;

        case 0b00010101: // PRNT Adress
            printf("OUT - %x\n", RAM[data]);
            break;

        case 0b00000111: // PRTC
            printf("OUT - %c\n", data);
            // printf("OUT - %x\n", RAM[data]);
            break;

        case 0b00010111: // PRTC Adress
            printf("OUT - %c\n", RAM[data]);
            
            break;

        case 0b00001000: // ADR1
            result = AC + R1;

            printf("Result is : %d\n", result);
                AC += R1;
            flags[ZERO] = result == 0;
            flags[NEGATIVE] = result < 0;
            flags[CARRY] = result > 255;
            break;

        case 0b00001010:  //SBR1
            result = AC - R1;

            printf("Result is : %d\n", result);
            AC -= R1;
            flags[ZERO] = result == 0;
            flags[NEGATIVE] = result < 0;
            flags[CARRY] = result > 255;
            break;

        case 0b00001011: // SBR2
            result = AC - R2;
            printf("Result is : %d\n", result);
            AC -= R2;
            flags[ZERO] = result == 0;
            flags[NEGATIVE] = result < 0;
            flags[CARRY] = result > 255;
            break;

        case 0b00001001:  //ADR2
            result = AC + R2;
            
            AC += R2;
            flags[ZERO] = result == 0;
            flags[NEGATIVE] = result < 0;
            flags[CARRY] = result > 255;
            break;

        case 0b10100000: // CLR
            AC = 0;
            flags[ZERO] = 1;
            flags[NEGATIVE] =  0;
            flags[CARRY] = 0;
            break;

        case 0b10110000: // CLR at address
            RAM[data] = 0;
            break;

        case 0b11010000: // Move result of Accumulator
            RAM[data] = AC;
            break;

        case 0b00000011: // NOP

            break;

        case 0b01110000: // PUSH
            
            SL++;
            check_overflow();
            RAM[SP + SL] = ((instructionMap["JUMP"] | 0x10) << 8) | (data - 1);


            printf("PUSH %#x at %#x \n", data, SP + SL);
            break;

        case 0b01110011: // PUSZ
            if (flags[ZERO])
            {
                SL++;
                check_overflow();
                RAM[SP + SL] = ((instructionMap["JUMP"] | 0x10) << 8) | (data - 1);
            }

            break;

        case 0b01110100: // PUSN
            if (flags[NEGATIVE])
            {
                SL++;
                check_overflow();
                RAM[SP + SL] = ((instructionMap["JUMP"] | 0x10) << 8) | (data - 1);
            }

            break;

        case 0b01110001: // POP
            PC = SP + SL - 1;
            SL--;

            break;

        default:

            printf("Instruction %#x Not found", instruction);
            break;
        }
    }

    void add(uint8_t value)
    {
        if (AC + value > 0xffff)
        {
            flags[CARRY] = 1;
        }
        AC += value;
    }

    void subtract(uint8_t value)
    {
        if (AC - value == 0)
        {
            flags[ZERO] = 0;
        }
        AC -= value;
    }

    void showMemory(uint8_t limit = 0xFF)
    {
        for (int address = 0; address < limit; ++address)
        {

            printf("[%#x]:%#x\n", address, RAM[address]);
        }
    }

    void check_overflow(){
        if(SL > STACK_CAPACITY){
            throw runtime_error("Stack overflowed....\n");
        }
    }
};

int main()
{

    CPU cpu;
    cpu.loadProgram("prog.os");
    
    int i;
    cin >> i;
    return 0;
}

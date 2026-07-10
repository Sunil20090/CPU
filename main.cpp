#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string>
#include <cstring>
#include <map>
using namespace std;

#define RAM_SIZE 0xff // 1024*8 bits (1KB)
#define STACK_CAPACITY 5
#define META_DATA_LENGTH 6
#define META_DATA_INDEX_META_DATA_LENGTH 0
#define META_DATA_INDEX_PROGRAMM_LENGTH 1
#define META_DATA_INDEX_STACK_START_ADDRESS 2
#define META_DATA_INDEX_USED_HEAP 3
#define META_DATA_INDEX_API_START_ADDRESS 4
#define META_DATA_INDEX_USED_MEMORY 5
#define MAX_APIS 10

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

#define SYS_STREAM 0x04 // 00000100
#define SYS_PRINT 0x00  // 00000000
#define SYS_SCAN 0x01   // 00000001



class CPU
{
public:
    uint8_t R[4] = {0x00, 0x00, 0x00, 0x00};

    #define R1 R[0]
    #define R2 R[1]
    #define R3 R[2]
    #define R4 R[3]


    uint8_t AR = 0x00; // Address Register
    uint8_t AC = 0x00; // Accumulator
    uint8_t PC = META_DATA_LENGTH - 1;
    uint16_t IR = 0x00;
    uint8_t SP = 0x00;
    uint16_t SL = 0x00; // STACK_LENGTH
    uint16_t API_POINTER = 0x00;
    uint16_t API_STACK_LENGTH = 0x00;

    uint8_t IS_DEBUG_PRINT = 1;

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

        for (int address = 0; address < RAM_SIZE; ++address)
        {
            RAM[address] = 0;
        }
    }

    void initMap()
    {
        // mapping
        instructionMap["hlt"] = INS_HLT;
        instructionMap["store"] = INS_STORE;
        instructionMap["load"] = INS_LOAD;
        instructionMap["lfad"] = INS_LOAD_OF_ADDRESS;
        instructionMap["jump"] = INS_JUMP;
        instructionMap["jupz"] = INS_JUMP_IF_ZERO;
        instructionMap["jupn"] = INS_JUMP_IF_LESS;
        instructionMap["add"] = INS_ADD;
        instructionMap["sub"] = INS_SUBTRACT;
        instructionMap["clr"] = INS_CLEAR_ACC;
        instructionMap["mov"] = INS_MOVE;
        instructionMap["push"] = INS_PUSH;
        instructionMap["pusz"] = INS_PUSH_IF_ZERO;
        instructionMap["pusn"] = INS_PUSH_IF_LESS;
        instructionMap["pop"] = INS_POP;
        instructionMap["nop"] = INS_NOP;
        instructionMap["aor"] = INS_GET_ADDRESS;
        instructionMap["inc"] = INS_INC;
        instructionMap["dec"] = INS_DEC;
        instructionMap["syscall"] = INS_SYSCALL;
        instructionMap["comp"] = INS_COMP;
        instructionMap["or"] = INS_OR;
        instructionMap["and"] = INS_AND;
        instructionMap["lts"] = INS_LEFT_SHIFT;
        instructionMap["rts"] = INS_RIGHT_SHIFT;
    }

    void saveProgram(uint8_t limit)
    {

        std::ofstream file("output.txt");

        if (!file)
        {
            std::cout << "Failed to open file.\n";
            return;
        }

        file << "uint16_t program[] = {\n";
        for (int address = 0; address <= limit; ++address)
        {
            file << "0x" << hex << RAM[address] << ",\n";
            // printf("%#x, \n", RAM[address]);
        }
        // printf("]");
        file << "};\n\n";

        file << std::dec;

        file << "META DATA LENGTH      : " << RAM[META_DATA_INDEX_META_DATA_LENGTH]
             << " (" << RAM[META_DATA_INDEX_META_DATA_LENGTH] * sizeof(uint16_t) << " bytes)\n";

        file << "PROGRAM LENGTH        : " << RAM[META_DATA_INDEX_PROGRAMM_LENGTH]
             << " (" << RAM[META_DATA_INDEX_PROGRAMM_LENGTH] * sizeof(uint16_t) << " bytes)\n";

        file << "STACK START ADDRESS   : 0x" << std::hex
             << RAM[META_DATA_INDEX_STACK_START_ADDRESS] << std::dec << "\n";

        file << "HEAP SIZE             : " << RAM[META_DATA_INDEX_USED_HEAP]
             << " (" << RAM[META_DATA_INDEX_USED_HEAP] * sizeof(uint16_t) << " bytes)\n";

        file << "API START ADDRESS     : 0x" << std::hex
             << RAM[META_DATA_INDEX_API_START_ADDRESS] << std::dec << "\n";

        file << "META USED MEMORY      : " << RAM[META_DATA_INDEX_USED_MEMORY]
             << " (" << RAM[META_DATA_INDEX_USED_MEMORY] * sizeof(uint16_t) << " bytes)\n";

        file.close(); // Optional (automatically called when file goes out of scope)


        

        std::cout << "File written successfully.\n";
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
                if (IS_DEBUG_PRINT)
                {
                    printf("%#x: %s%d %#x\n",
                           static_cast<int>(i),
                           pair.first.c_str(), middle,
                           static_cast<int>(secondHalf));
                    return;
                }
            }
            else if (((instructionMap[pair.first] | INS_DATA_ADDRESS) == instruction))
            {
                if (IS_DEBUG_PRINT)
                {
                    printf("%#x: %s%d [%#x]\n",
                           static_cast<int>(i),
                           pair.first.c_str(), middle, static_cast<int>(secondHalf));
                    return;
                }
            }
        }

        printf("%#x: %#x\n",
               static_cast<int>(i),
               RAM[i]);
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

        if (IS_DEBUG_PRINT)
        {
            cout << "Program Loaded:" << programname << endl
                 << program;
        }

        map<uint16_t, string> variableLineNumberMap;

        uint16_t *token = parse(programname, program, variableLineNumberMap, &lineCount);

        uint8_t i;
        for (i = 0; i < lineCount; i++)
        {
            RAM[i] = token[i];
        }

        SP = RAM[META_DATA_INDEX_STACK_START_ADDRESS];

        // printf("Stack pointer is pointing to : %#x\n", i);

        cout
            << "\n"
            << "============== Translated program ==========\n"
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

        if (IS_DEBUG_PRINT)
        {
            cout << "Reading finished" << endl;
        }

        // saveProgram(0xff);
        saveProgram(RAM[META_DATA_INDEX_USED_MEMORY]);

        run();

        if (IS_DEBUG_PRINT)
        {
            showMemory(0x00, 0xff);
            printf("Api length = %d\n", apiLength);
        }
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

    void runAsApi()
    {
        int pushCounter = 0;
        SL = 0;
        while (RAM[++PC] & 0xff00)
        {
            // //printf("Pointing to : %#x\n", static_cast<int>(PC));

            IR = RAM[PC];
            uint16_t instruction = (IR & 0xff00) >> 8;
            uint8_t data = IR & 0x00ff;

            if(instruction == (INS_PUSH | INS_DATA_ADDRESS)){
                pushCounter++;
            }else if(instruction == INS_POP){
                pushCounter--;
            }

            if(pushCounter < 0){
                break;
            }else {
                execute(instruction, data);
            }
            
        }
    }

    char *apis[MAX_APIS];
    uint16_t apiPointers[MAX_APIS];
    int apiLength = 0;

    void register_api(char *name, uint16_t pointer, int offset)
    {
        if (apiLength >= MAX_APIS)
        {
            printf("API list is full!\n");
            return;
        }
        // Allocate memory for the name
        apis[apiLength] = (char *)malloc(strlen(name) + 1);

        if (apis[apiLength] == NULL)
        {
            printf("Memory allocation failed!\n");
            return;
        }

        // Copy the string
        strcpy(apis[apiLength], name);

        apiPointers[apiLength] = pointer;

        printf("Registered: %s at %d\n", apis[apiLength], apiPointers[apiLength]);
        int k = 0;

        RAM[offset + API_STACK_LENGTH++] = pointer;
        while (*(name + k) != '\0')
        {
            printf("Saving API... %#x : %c\n", API_STACK_LENGTH + offset, *(name + k));
            RAM[offset + API_STACK_LENGTH++] = *(name + k);
            k++;
        }
        printf("Saving API... %#x : %c\n", API_STACK_LENGTH + offset, *(name + k));
        RAM[offset + API_STACK_LENGTH++] = *(name + k);

        apiLength++;
    }

    void call_api(char *name)
    {
        for (int i = 0; i < apiLength; i++)
        {
            if (strcmp(apis[i], name) == 0)
            {
                // printf("API Found!\n");
                // printf("Name    : %s\n", apis[i]);
                // printf("Pointer : %d\n", apiPointers[i]);

                // Your code
                
                uint8_t currentPointer = PC;

                printf("===================\n", name);
                printf("API Running for: %s\n", name);
                printf("===================\n", name);

                PC = apiPointers[i];
                runAsApi();
                // run();

                printf("API Running completed!\n");

                return;
            }
        }

        printf("API '%s' not found!\n", name);
    }

    // parsing
    uint16_t *parse(string fileName, string &program, map<uint16_t, string> variableLineNumberMap, uint8_t *pLineCount)
    {
        map<string, uint16_t> funtionMap;
        map<string, uint16_t> variableMap;
        map<string, uint16_t> arrayMap;
        map<string, string> staticVarMap;
        map<string, uint16_t> apiMap;
        map<string, string> memoryMap;

        uint16_t *binaries = new uint16_t[300];
        memset(binaries, 0, 256 * sizeof(uint16_t));
        uint16_t binaryIndex = META_DATA_LENGTH;

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
            if (program[i] == ';')
            {
                while (program[++i] != '\n');
            }

            if (program[i] == '\t')
            {
                continue;
            }

            if (program[i] == ' ' || program[i] == '\n' || program[i] == ':')
            {
                if (buffer == "end")
                {
                    functionStarted = false;                
                    *(binaries + binaryIndex++) = ((INS_POP) << 8) | funtionMap[currentFunction];
                    currentFunction = "";
                    buffer.clear();
                    continue;
                }

                if (buffer == "define")
                {
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    while (1)
                    {
                        if (IS_DEBUG_PRINT)
                        {
                            // cout << "char: \"" << program[i + 1] << "\"" << endl;
                        }

                        if (program[++i] == '\n')
                        {

                            if (isKeyAvailable(fileName + buffer, staticVarMap))
                            {
                                throw runtime_error("Already defined \"" + fileName + buffer + "\" ");
                            }

                            int index = buffer.find("$");
                            int wideSpaceIndex = buffer.find(" ");
                            int newLineIndex = buffer.find("\n");

                            string variableName = buffer.substr(index + 1, wideSpaceIndex - index - 1);

                            string value = buffer.substr(wideSpaceIndex + 1, newLineIndex - wideSpaceIndex);
                            staticVarMap[fileName + variableName] = value;
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

                if (buffer == "memory")
                {
                    buffer.clear();

                    while (1)
                    {
                        if (program[++i] == '\n')
                        {
                            if (IS_DEBUG_PRINT)
                            {
                                printf("[buffer]=[%s]\n", buffer.c_str());
                            }

                            int variableNameStartIndex = buffer.find("$");
                            int dataStartIndex = buffer.find("[");
                            int dataEndIndex = buffer.find("]");

                            string variableName = buffer.substr(variableNameStartIndex + 1, dataStartIndex - variableNameStartIndex - 1);
                            string dataArray = buffer.substr(dataStartIndex + 1, dataEndIndex - dataStartIndex - 1);

                            if (isKeyAvailable(fileName + variableName, arrayMap))
                            {
                                memoryMap[fileName + variableName] = dataArray;
                            }
                            else if (isKeyAvailable(fileName + variableName, variableMap)){
                                memoryMap[fileName + variableName] = dataArray;
                            }
                            else
                            {
                                throw;
                            }

                            printf("memory will be save to %s--\n", variableName.c_str());
                            printf("data in memory will be %s--\n", dataArray.c_str());

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

                if (buffer == "access")
                {
                    buffer.clear();
                    while (1)
                    {
                        if (program[++i] == '\n')
                        {
                            if (IS_DEBUG_PRINT)
                            {
                                printf("File name found %s\n", buffer.c_str());
                            }
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

                if (buffer == "call")
                {
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    while (1)
                    {
                        if (IS_DEBUG_PRINT)
                        {
                            // cout << "char: \"" << program[i + 1] << "\"" << endl;
                        }

                        if (program[++i] == '\n')
                        {
                            if (!isKeyAvailable(fileName + buffer, funtionMap))
                            {
                                throw runtime_error("CALL Function \"" + fileName + buffer + "\" Not found");
                            }

                            *(binaries + binaryIndex) = ((INS_PUSH | INS_DATA_ADDRESS) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((INS_JUMP | INS_DATA_ADDRESS) << 8) | funtionMap[fileName + buffer];
                            break;
                        }
                        else
                        {
                            string s(1, program[i]);
                            buffer += s;
                        }
                    }

                    // buffer.clear();
                    continue;
                }

                if (buffer == "calz")
                {
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    while (1)
                    {
                        if (IS_DEBUG_PRINT)
                        {
                            // cout << "char: \"" << program[i + 1] << "\"" << endl;
                        }

                        if (program[++i] == '\n')
                        {
                            if (!isKeyAvailable(fileName + buffer, funtionMap))
                            {
                                throw runtime_error("CALLZ Function \"" + fileName + buffer + "\" Not found");
                            }

                            *(binaries + binaryIndex) = ((INS_PUSH_IF_ZERO | INS_DATA_ADDRESS) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((INS_JUMP_IF_ZERO | INS_DATA_ADDRESS) << 8) | funtionMap[fileName + buffer];
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

                if (buffer == "caln")
                {
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    while (1)
                    {
                        if (program[++i] == '\n')
                        {
                            if (!isKeyAvailable(fileName + buffer, funtionMap))
                            {
                                throw runtime_error("CALN Function \"" + buffer + "\" Not found");
                            }

                            *(binaries + binaryIndex) = ((INS_PUSH_IF_LESS | INS_DATA_ADDRESS) << 8) | binaryIndex + 2;
                            binaryIndex++;
                            *(binaries + binaryIndex++) = ((INS_JUMP_IF_LESS | INS_DATA_ADDRESS) << 8) | funtionMap[fileName + buffer];
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

                if (buffer == "dclr")
                {
                    // finding the function name
                    buffer.clear();
                    // string functionBuffer = "";

                    if (IS_DEBUG_PRINT)
                    {
                        printf("CURRENT FUNCTION NAME: --%s--\n", currentFunction.c_str());
                    }

                    while (1)
                    {
                        // cout << "char: \"" << program[i+1] << "\"" << endl;
                        if (program[++i] == '\n')
                        {
                            string v_name = buffer.substr(1, buffer.length() - 1);
                            int startIndex = v_name.find("[");
                            int endIndex = v_name.find("]");
                            // printf("Vriable: %s start: %d, end : %d\n", v_name.c_str(), startIndex, endIndex);

                            if (startIndex != -1 && endIndex != -1)
                            {
                                string v_name_without_arr = v_name.substr(0, startIndex);

                                if (!isKeyAvailable(fileName + currentFunction + v_name_without_arr, variableMap))
                                {
                                    int size = stoi(v_name.substr(startIndex + 1, endIndex - startIndex - 1));
                                    variableMap[fileName + currentFunction + v_name_without_arr] = variable_counter;

                                    // printf("Buffer: \"%s\" Variable \"%s\" = declared on line number %#x:  address will be offset by [%#x] \n", buffer.c_str(), v_name_without_arr.c_str(), binaryIndex, variable_counter);
                                    variable_counter += size;
                                    arrayMap[fileName + currentFunction + v_name_without_arr] = size;
                                }
                                else
                                {
                                    throw runtime_error("Variable \"" + buffer + "\" Already declared");
                                }
                            }
                            else if (!isKeyAvailable(fileName + currentFunction + buffer, variableMap))
                            {
                                // printf("Buffer: \"%s\" Variable \"%s\" = declared on line number %#x:  address will be offset by [%#x] \n", buffer.c_str(), buffer.substr(1, buffer.length() - 1).c_str(), binaryIndex, variable_counter);
                                variableMap[fileName + currentFunction + v_name] = variable_counter++;

                                // *(binaries + binaryIndex) = ((instructionMap["NOP"] | 0x10) << 8);
                                // binaryIndex++;
                            }
                            else
                            {
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
                else if (buffer.substr(0, 1) == "#")
                {
                    // string decimal = buffer.substr(1, buffer.length() - 1);
                    // sprintf(currentData, "%x", stoi(decimal));
                }
                else if (buffer.substr(0, 1) == "%")
                {
                    middleData = buffer.substr(1, 3);
                    buffer.clear();
                }
                // else if (buffer.substr(0, 1) == "[")
                // {
                //     int indexOf = buffer.find("]");
                //     if (indexOf == -1)
                //     {
                //         throw;
                //     }
                //     currentAddress = buffer.substr(3, indexOf - 1);
                // }
                else if (buffer.substr(0, 1) == "\'" && buffer.substr(2, 3) == "\'")
                {
                    int value = static_cast<int>(buffer[1]);
                    stringstream ss;
                    ss << hex << value;
                    currentData = "0x" + ss.str();
                    buffer.clear();
                }

                else if (program[i] == ':')
                {
                    int apiStartIndex = buffer.find("{");
                    string apiName = "";
                    if (apiStartIndex != -1)
                    {
                        currentFunction = buffer.substr(0, apiStartIndex);
                        apiName = buffer.substr(apiStartIndex + 1, buffer.length() - apiStartIndex - 2);

                        printf("API found ! %s [%s]\n", buffer.c_str(), currentFunction.c_str());
                        
                    }
                    else
                    {
                        currentFunction = buffer;
                    }
                    
                    if (!isKeyAvailable(fileName + currentFunction, funtionMap))
                    {
                        if (functionStarted)
                        {
                            throw runtime_error("Function not ended near: " + to_string(binaryIndex + 1));
                        }
                        funtionMap[fileName + currentFunction] = binaryIndex;
                        if(apiStartIndex != -1){
                            apiMap[apiName] = binaryIndex;
                        }
                        if (currentFunction == "_main")
                        {
                            *(binaries + META_DATA_LENGTH) = ((INS_JUMP | INS_DATA_ADDRESS) << 8) | binaryIndex;
                        }
                        if (IS_DEBUG_PRINT)
                        {
                            printf("FUNCTION STARTED AND Address assigned: %s %#x\n", (fileName + currentFunction).c_str(), static_cast<int>(binaryIndex));
                        }

                        functionStarted = true;
                    }

                    buffer.clear();
                }
                else if (buffer.substr(0, 1) == "$")
                {
                    string v_name = buffer.substr(1, buffer.length() - 1);
                    int startIndex = v_name.find("[");
                    int endIndex = v_name.find("]");

                    if (startIndex != -1 && endIndex != -1)
                    {
                        string v_name_without_arr = v_name.substr(0, startIndex);

                        if (isKeyAvailable(fileName + currentFunction + v_name_without_arr, variableMap))
                        {
                            int index = stoi(v_name.substr(startIndex + 1, endIndex - startIndex - 1));

                            if (index >= arrayMap[fileName + currentFunction + v_name_without_arr])
                            {
                                throw runtime_error("Index is out of range");
                            }
                            int decimal_num = variableMap[fileName + currentFunction + v_name_without_arr] + index;
                            char hex_string[10];

                            snprintf(hex_string, sizeof(hex_string), "%X", decimal_num);

                            currentAddress = hex_string;

                            variableLineNumberMap[binaryIndex] = fileName + currentFunction + v_name_without_arr;
                        }
                        else
                        {
                            throw runtime_error("Array not found \"" + v_name_without_arr + "\" not found");
                        }
                    }
                    else if (isKeyAvailable(fileName + v_name, staticVarMap))
                    {
                        currentData = staticVarMap[fileName + v_name];
                    }
                    else if (isKeyAvailable(fileName + currentFunction + v_name, variableMap))
                    {
                        int decimal_num = variableMap[fileName + currentFunction + v_name];
                        char hex_string[10];
                        snprintf(hex_string, sizeof(hex_string), "%x", decimal_num);
                        currentAddress = hex_string;
                        variableLineNumberMap[binaryIndex] = fileName + currentFunction + v_name;
                    }
                    else
                    {
                        throw runtime_error("Variable \"" + v_name + "\" not found");
                    }
                }
                else
                {

                    currentInstruction = buffer;
                    if (IS_DEBUG_PRINT)
                    {
                        // printf("I am in else instruction \"%s, buffer = \"%s\n", currentInstruction.c_str(), buffer.c_str());
                    }

                    middleData.clear();
                    currentData.clear();
                    currentAddress.clear();
                }

                if (!currentFunction.empty())
                {
                    *(binaries + binaryIndex) = ((INS_NOP | INS_DATA_ADDRESS) << 8) | funtionMap[currentFunction];
                    binaryIndex++;
                    currentFunction.clear();
                }
                else if (!currentInstruction.empty() && !currentData.empty() && !middleData.empty())
                {
                    *(binaries + binaryIndex) = (instructionMap[currentInstruction] | getRegister(middleData[1])) << 8 | (stoi(currentData, nullptr, 16));
                    binaryIndex++;
                    currentInstruction.clear();
                    middleData.clear();
                    currentData.clear();
                }
                else if (!currentInstruction.empty() && !currentAddress.empty() && !middleData.empty())
                {
                    // //printf("Middledata %c", middleData[1]);
                    *(binaries + binaryIndex) = (((instructionMap[currentInstruction] | INS_DATA_ADDRESS) | getRegister(middleData[1])) << 8) | stoi(currentAddress, nullptr, 16);
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
            uint16_t variableAddressAssigned = (*(binaries + lineNumber) & 0x00ff);
            string variableName = pair.second;
            if (isKeyAvailable(variableName, arrayMap))
            {
                uint16_t arraySize = arrayMap[variableName];
                if (arraySize < variableAddressAssigned - variableMap[variableName])
                {
                    if (IS_DEBUG_PRINT)
                    {
                        printf("OUT OF RANGE ARRAY: %s < %d \n", variableName.c_str(), variableAddressAssigned - variableMap[variableName]);
                    }
                    throw runtime_error("OUT OF RANGE ARRAY: " + variableAddressAssigned + variableName);
                }
            }
            *(binaries + lineNumber) = (*(binaries + lineNumber) & 0xff00) | ((variableAddressAssigned + binaryIndex + STACK_CAPACITY + 1));
        }

        int heapSize = 0;
        for (auto &pair : variableMap)
        {
            if(isKeyAvailable(pair.first, arrayMap)){
                heapSize += arrayMap[pair.first];
            }else {
                heapSize++;
            }

            if(isKeyAvailable(pair.first, memoryMap)){
                const char * data = memoryMap[pair.first].c_str();
                int i = 0;
                int bufferIndex = 0;
                int dataIndex = 0;
                char* dataString = new char[20];
                while(*(data + i) != '\0'){
                    if (*(data + i) == ','){
                        *(dataString + bufferIndex) = '\0';
                        printf("found data! memoryMap[%s] = %#x | actual %#x |  --%s-- %#x\n", pair.first.c_str(), (pair.second + binaryIndex + STACK_CAPACITY + 1 + dataIndex), pair.second, dataString, stoi(dataString, nullptr, 16));

                        RAM[(pair.second + binaryIndex + STACK_CAPACITY + 1 + dataIndex)] = stoi(dataString, nullptr, 16);
                        i++;
                        dataIndex++;
                        bufferIndex = 0;
                    }
                    *(dataString + bufferIndex) = *(data + i);
                    bufferIndex++;
                    i++;
                }

                *(dataString + bufferIndex) = '\0';
                RAM[(pair.second + binaryIndex + STACK_CAPACITY + 1 + dataIndex)] = stoi(dataString, nullptr, 16);
                printf("found data! memoryMap[%s] = %#x | actual %#x |  --%s-- %#x\n", pair.first.c_str(), (pair.second + binaryIndex + STACK_CAPACITY + 1 + dataIndex), pair.second, dataString, stoi(dataString, nullptr, 16));
            }
        }

        *(pLineCount) = binaryIndex;        

        for (auto &pair : apiMap)
        {
            register_api((char *)(pair.first.c_str()), pair.second, binaryIndex + variable_counter + STACK_CAPACITY + 1);
        }

        

        int programSize = binaryIndex + variable_counter + STACK_CAPACITY + API_STACK_LENGTH;

        *(binaries + META_DATA_INDEX_META_DATA_LENGTH) = META_DATA_LENGTH;
        *(binaries + META_DATA_INDEX_PROGRAMM_LENGTH) = *(pLineCount)-META_DATA_LENGTH;
        *(binaries + META_DATA_INDEX_STACK_START_ADDRESS) = *(pLineCount);
        *(binaries + META_DATA_INDEX_USED_HEAP) = heapSize;
        *(binaries + META_DATA_INDEX_API_START_ADDRESS) = binaryIndex + variable_counter + STACK_CAPACITY + 1;
        *(binaries + META_DATA_INDEX_USED_MEMORY) = programSize;

        printf("Program size is  %d bytes (%f%%)\n", programSize * 2, (float)(programSize * 100) / RAM_SIZE);

        return binaries;

    }

    bool isKeyAvailable(const string &key, map<string, uint16_t> &map)
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

    bool isKeyAvailable(const string &key, map<string, string> &map)
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

            IR = RAM[PC];
            uint16_t instruction = (IR & 0xff00) >> 8;
            uint8_t data = IR & 0x00ff;

            execute(instruction, data);
        }

        // printf("\n\nProgramm ended at: %#x\n", static_cast<int>(PC));
    }

    void execute(uint16_t instruction, uint8_t data)
    {

        uint8_t reg = instruction & 0x03;
        uint8_t opcode = instruction & ~0x03;

        printf("===========\n");
        translateProgramAt(PC);

        // running
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

        case INS_INC | INS_R1:
        case INS_INC | INS_R2:
        case INS_INC | INS_R3:
        case INS_INC | INS_R4:
            R[reg]++;
            break;

        case INS_DEC | INS_R1:
        case INS_DEC | INS_R2:
        case INS_DEC | INS_R3:
        case INS_DEC | INS_R4:
            R[reg]--;
            break;

        case INS_GET_ADDRESS | INS_DATA_ADDRESS | INS_R1:
        case INS_GET_ADDRESS | INS_DATA_ADDRESS | INS_R2:
        case INS_GET_ADDRESS | INS_DATA_ADDRESS | INS_R3:
        case INS_GET_ADDRESS | INS_DATA_ADDRESS | INS_R4:
            R[reg] = data;
            break;

        case INS_STORE | INS_DATA_ADDRESS | INS_R1:
        case INS_STORE | INS_DATA_ADDRESS | INS_R2:
        case INS_STORE | INS_DATA_ADDRESS | INS_R3:
        case INS_STORE | INS_DATA_ADDRESS | INS_R4:
            RAM[data] = R[reg];
            R[reg] = 0;
            break;

        case INS_LOAD | INS_R1:
        case INS_LOAD | INS_R2:
        case INS_LOAD | INS_R3:
        case INS_LOAD | INS_R4:
            R[reg] = data;
            break;

        case INS_LOAD | INS_DATA_ADDRESS | INS_R1:
        case INS_LOAD | INS_DATA_ADDRESS | INS_R2:
        case INS_LOAD | INS_DATA_ADDRESS | INS_R3:
        case INS_LOAD | INS_DATA_ADDRESS | INS_R4:
            R[reg] = RAM[data];
            break;

        case INS_LOAD_OF_ADDRESS | INS_DATA_ADDRESS | INS_R1:
        case INS_LOAD_OF_ADDRESS | INS_DATA_ADDRESS | INS_R2:
        case INS_LOAD_OF_ADDRESS | INS_DATA_ADDRESS | INS_R3:
        case INS_LOAD_OF_ADDRESS | INS_DATA_ADDRESS | INS_R4:
            R[reg] = RAM[RAM[data]];
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

        case INS_ADD | INS_R1:
        case INS_ADD | INS_R2:
        case INS_ADD | INS_R3:
        case INS_ADD | INS_R4:
            AC += R[reg];
            break;

        case INS_ADD | INS_DATA_ADDRESS:
            AC += RAM[data];
            if (IS_DEBUG_PRINT)
            {
                printf("adding address..AC += RAM[%#x] (%#x)\n", data, RAM[data]);
            }
            break;

        case INS_SUBTRACT | INS_DATA_ADDRESS:
            AC -= RAM[data];
            break;

        case INS_SUBTRACT | INS_R1:
        case INS_SUBTRACT | INS_R2:
        case INS_SUBTRACT | INS_R3:
        case INS_SUBTRACT | INS_R4:
            AC -= R[reg];
            break;

        case INS_OR | INS_R1:
        case INS_OR | INS_R2:
        case INS_OR | INS_R3:
        case INS_OR | INS_R4:
            AC |= R[reg];
            break;

        case INS_AND | INS_R1:
        case INS_AND | INS_R2:
        case INS_AND | INS_R3:
        case INS_AND | INS_R4:
            AC &= R[reg];
            break;

        case INS_LEFT_SHIFT | INS_DATA_ADDRESS | INS_R1:
        case INS_LEFT_SHIFT | INS_DATA_ADDRESS | INS_R2:
        case INS_LEFT_SHIFT | INS_DATA_ADDRESS | INS_R3:
        case INS_LEFT_SHIFT | INS_DATA_ADDRESS | INS_R4:
            RAM[data] = RAM[data] << R[reg];
            break;

        case INS_RIGHT_SHIFT | INS_DATA_ADDRESS | INS_R1:
        case INS_RIGHT_SHIFT | INS_DATA_ADDRESS | INS_R2:
        case INS_RIGHT_SHIFT | INS_DATA_ADDRESS | INS_R3:
        case INS_RIGHT_SHIFT | INS_DATA_ADDRESS | INS_R4:
            RAM[data] = RAM[data] >> R[reg];
            break;

        case INS_CLEAR_ACC: // CLR
            AC = 0;
            flags[ZERO] = 1;
            flags[NEGATIVE] = 0;
            flags[CARRY] = 0;
            break;

        case INS_COMP | INS_DATA_ADDRESS | INS_R1:
        case INS_COMP | INS_DATA_ADDRESS | INS_R2:
        case INS_COMP | INS_DATA_ADDRESS | INS_R3:
        case INS_COMP | INS_DATA_ADDRESS | INS_R4:
            flags[ZERO] = R[reg] == RAM[data];
            flags[NEGATIVE] = R[reg] > RAM[data];
            break;

        case INS_COMP | INS_R1:
        case INS_COMP | INS_R2:
        case INS_COMP | INS_R3:
        case INS_COMP | INS_R4:
            flags[ZERO] = R[reg] == data;
            flags[NEGATIVE] = R[reg] > data;
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
        char x;
        char text[40];
        if (IS_DEBUG_PRINT)
        {
            printf(
                "\nsystem called with %d\t R1 = %#x\n",
                static_cast<int>(IR & 0x00ff),
                static_cast<int>(R1));
        }

        switch (IR & 0x00ff)
        {
        case SYS_PRINT:
        {
            printf("{%c}\n", RAM[R1]);
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

        case 0x03:
            for(int i=0; i<4; ++i){
                printf("Sending PWM...%#x\n", RAM[R1 + i]);
            }
            
            break;

        default:
            break;
        }
    }

    void showMemory(uint8_t start = 0x00, uint16_t limit = 0xFF)
    {
        printf("\n");
        for (int address = start; address <= limit; ++address)
        {
            printf("[%#x]:%#x\n", address, RAM[address]);
        }
    }

    void showShareableProgramm(uint8_t start = 0x00, uint16_t limit = 0xFF)
    {
        printf("uint16_t RAM[] = [\n");
        for (int address = start; address <= limit; ++address)
        {

            printf("%#x, \n", RAM[address]);
        }
        printf("]\n\n");
    }

    void check_overflow()
    {
        if (SL > STACK_CAPACITY)
        {
            throw runtime_error("Stack overflowed....\n");
        }
    }

    void initFlags(uint8_t register_value)
    {
        flags[ZERO] = AC == 0;
        flags[NEGATIVE] = register_value > AC;
        // flags[CARRY] = result > 255;
    }
};

int main()
{

    CPU cpu;

    const string programs[] = {"prog.os", "calc.os", "image.os", "condition.os", "mult.os", "gpio.os"};

    printf("Available programs:\n");

    int size = sizeof(programs) / sizeof(programs[0]);

    for (int i = 0; i < size; i++)
    {
        printf("(%d). %s:\n", i + 1, programs[i].c_str());
    }

    cpu.loadProgram(programs[5]);
    cpu.call_api((char *)"POWER");
    cpu.call_api((char *)"VOLUME[+]");
    cpu.call_api((char *)"VOLUME[-]");
    cpu.call_api((char *)"UP_ARROW");

    int i;
    cin >> i;

    return 0;
}

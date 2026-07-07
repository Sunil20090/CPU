#include <iostream>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string>
#include <map>
using namespace std;

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

#define SYS_STREAM 0x04 // 00000100
#define SYS_PRINT 0x00  // 00000000
#define SYS_SCAN 0x01   // 00000001

typedef struct API
{
    const char name[10];
    uint16_t pointer;
    uint16_t args[10];
} API;

API* list[10];

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
    uint16_t SL = 0x00; // STACK_LENGTH

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

        for (int address = 0; address < 0xFF; ++address)
        {
            RAM[address] = 0;
        }
    }

    void initMap()
    {
        // mapping =
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

        int length = sizeof(list) / sizeof(API);
        file << "API:\n" << length;
        for(int i=0; i<length; i++){
            file << list[i]->name << "\n";
        }

        file << "\n\n";

        file << "uint16_t program[] = {\n";
        for (int address = 0; address <= limit; ++address)
        {
            file << "0x" << hex << RAM[address] << ",\n";
            // printf("%#x, \n", RAM[address]);
        }
        // printf("]");
        file << "};";

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
                    break;
                }
            }
            else if (((instructionMap[pair.first] | INS_DATA_ADDRESS) == instruction))
            {
                if (IS_DEBUG_PRINT)
                {
                    printf("%#x: %s%d [%#x]\n",
                           static_cast<int>(i),
                           pair.first.c_str(), middle, static_cast<int>(secondHalf));
                    break;
                }
            }
        }
    }

    // 0x43
    // 0b01000011

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

        SP = i;

        // printf("Stack pointer is pointing to : %#x\n", i);

        cout
            << "\n"
            << "================ Translated program ==========\n"
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
        // saveProgram(lineCount);
        

        run();
        if (IS_DEBUG_PRINT)
        {
            showShareableProgramm(0x00, lineCount + STACK_CAPACITY + 0x20);

            int length = sizeof(list) / sizeof(API);

            printf("APi length: %d\n", length);
            for(int i=0; i<length; i++){
                printf("hello");
                printf("API: /%s\n", list[i]->name);
                printf("API: /%d\n", list[i]->pointer);
            }

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

   
    // void addToApis(const char* name, int apiPointer){
    //     int nextIndex = sizeof(list) / sizeof(API);
    //     API* api = (API *)malloc(sizeof(API));
    //     api->name = name;
    //     api->pointer = apiPointer;
    // }

    // parsing
    uint16_t *parse(string fileName, string &program, map<uint16_t, string> variableLineNumberMap, uint8_t *pLineCount)
    {
        map<string, uint16_t> funtionMap;
        map<string, uint16_t> variableMap;
        map<string, uint16_t> arrayMap;
        map<string, string> staticVarMap;

        uint16_t *binaries = new uint16_t[300];
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

                if (IS_DEBUG_PRINT)
                {
                    // printf("buffer = [%s], \t\tprogram[i] = %c\t CI=[%s]\tCD=[%s]\tMD=[%s] \tAD=[%s]\n", buffer.c_str(), program[i] == '\n' ? 'N' : program[i] == ' ' ? 'S': program[i], currentInstruction.c_str(), currentData.c_str(), middleData.c_str(), currentAddress.c_str());
                }

                if (buffer == "end")
                {
                    functionStarted = false;
                    currentFunction = "";
                    buffer.clear();
                    *(binaries + binaryIndex++) = ((INS_POP) << 8);
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

                            if (IS_DEBUG_PRINT)
                            {
                                printf("defined buffer found %s", buffer.c_str());
                            }

                            if (isKeyAvailable(fileName + buffer, staticVarMap))
                            {
                                throw runtime_error("Already defined \"" + fileName + buffer + "\" ");
                            }

                            int index = buffer.find("$");
                            int wideSpaceIndex = buffer.find(" ");
                            int newLineIndex = buffer.find("\n");

                            string variableName = buffer.substr(index + 1, wideSpaceIndex - index - 1);

                            string value = buffer.substr(wideSpaceIndex + 1, newLineIndex - wideSpaceIndex);

                            printf("variable name %s and value : %s\" [%x]\n", variableName.c_str(), value.c_str(), stoi(value, nullptr, 16));
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

                if (buffer == "save")
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

                            if (buffer.substr(0, 1) == "\"" && buffer.substr(buffer.length() - 1, 1) == "\"")
                            {

                                int index = buffer.find("$");
                                int wideSpaceIndex = buffer.find(" ");

                                string variableName = buffer.substr(index + 1, wideSpaceIndex - index - 1);

                                if (isKeyAvailable(fileName + currentFunction + variableName, variableMap) && isKeyAvailable(fileName + currentFunction + variableName, arrayMap))
                                {

                                    int stringStartIndex = buffer.find("\"");
                                    int lastStringIndex = buffer.find_last_of("\"");

                                    string stringData = buffer.substr(stringStartIndex + 1, lastStringIndex - stringStartIndex - 1);

                                    if (IS_DEBUG_PRINT)
                                    {
                                        printf("String data: [%s]--\n", stringData.c_str());
                                    }
                                    int i = 0;
                                    while (i < lastStringIndex - stringStartIndex - 1)
                                    {
                                        // printf("index: %d,  variableMap[v_name_without_arr]: %d\n", index, variableMap[v_name_without_arr]);
                                        *(binaries + binaryIndex) = ((INS_LOAD | INS_R1) << 8) | stringData[i];
                                        binaryIndex++;
                                        *(binaries + binaryIndex) = ((INS_STORE | INS_DATA_ADDRESS | INS_R1) << 8) | variableMap[fileName + currentFunction + variableName] + i;
                                        variableLineNumberMap[binaryIndex] = fileName + currentFunction + variableName;
                                        binaryIndex++;
                                        i++;
                                    }
                                    *(binaries + binaryIndex) = ((INS_LOAD | INS_R1) << 8);
                                    binaryIndex++;
                                    *(binaries + binaryIndex) = ((INS_STORE | INS_DATA_ADDRESS | INS_R1) << 8) | variableMap[fileName + currentFunction + variableName] + i;
                                    variableLineNumberMap[binaryIndex] = fileName + currentFunction + variableName;
                                    binaryIndex++;
                                }
                                else
                                {
                                    throw runtime_error("Variable not defined as an array");
                                }
                                if (IS_DEBUG_PRINT)
                                {
                                    printf("Variable found [%s]\n", variableName.c_str());
                                }
                            }

                            else
                            {
                                throw runtime_error("Invalid string");
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
                    // printf("Register found! R%c\n", buffer[2]);
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
                    currentData = "0x" + ss.str();
                    buffer.clear();
                }

                else if (program[i] == ':')
                {
                    int apiStartIndex = buffer.find("{");
                    if (apiStartIndex != -1)
                    {
                        currentFunction = buffer.substr(0, apiStartIndex);

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
                            // addToApis(currentFunction.c_str(), binaryIndex);


                        }
                        if (currentFunction == "_main")
                        {
                            *(binaries + 1) = ((INS_JUMP | INS_DATA_ADDRESS) << 8) | binaryIndex;
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

                        // printf("Using array %s\n", v_name_without_arr.c_str());

                        if (isKeyAvailable(fileName + currentFunction + v_name_without_arr, variableMap))
                        {
                            int index = stoi(v_name.substr(startIndex + 1, endIndex - startIndex - 1));

                            if (index >= arrayMap[fileName + currentFunction + v_name_without_arr])
                            {
                                throw runtime_error("Index is out of range");
                            }

                            currentAddress = to_string(variableMap[fileName + currentFunction + v_name_without_arr] + index);
                            // printf("index: %d,  variableMap[v_name_without_arr]: %d\n", index, variableMap[v_name_without_arr]);
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
                        currentAddress = to_string(variableMap[fileName + currentFunction + v_name]);
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

                // //printf(" [%s, %s, %s]\n", currentInstruction.c_str(), middleData.c_str(), currentAddress.c_str());

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

        for (auto &pair : variableMap)
        {
            if (IS_DEBUG_PRINT)
            {
                printf("variables found:[%s]\n", pair.first.c_str());
            }
        }

        for (auto &pair : variableLineNumberMap)
        {
            uint16_t lineNumber = pair.first;
            uint16_t oldOffset = (*(binaries + lineNumber) & 0x00ff);
            if (isKeyAvailable(pair.second, arrayMap))
            {
                if (IS_DEBUG_PRINT)
                {
                    printf("ARRAY %s[%d] variableMap[%s]=[%d]\n", pair.second.c_str(), arrayMap[pair.second], pair.second.c_str(), variableMap[pair.second]);
                }
                if (arrayMap[pair.second] < oldOffset - variableMap[pair.second])
                {
                    if (IS_DEBUG_PRINT)
                    {
                        printf("OUT OF RANGE ARRAY: %s < %d \n", pair.second.c_str(), oldOffset - variableMap[pair.second]);
                    }
                    throw runtime_error("OUT OF RANGE ARRAY: " + oldOffset + pair.second);
                }
            }
            *(binaries + lineNumber) = (*(binaries + lineNumber) & 0xff00) | ((oldOffset + binaryIndex + STACK_CAPACITY + 1));
        }

        int programSize = binaryIndex + variable_counter + STACK_CAPACITY;

        printf("Program size is  %d bytes (%f%%)\n", programSize, (float)(programSize * 100) / RAM_SIZE);

        *(pLineCount) = ++binaryIndex;

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

            execute();
        }

        // printf("\n\nProgramm ended at: %#x\n", static_cast<int>(PC));
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
            R1 += 1;
            break;

        case INS_INC | INS_R2:
            R2 += 1;
            break;

        case INS_INC | INS_R3:
            R3 += 1;
            break;

        case INS_INC | INS_R4:
            R4 += 1;
            break;

        case INS_DEC | INS_R1:
            R1 -= 1;
            break;

        case INS_DEC | INS_R2:
            R2 -= 1;
            break;

        case INS_DEC | INS_R3:
            R3 -= 1;
            break;

        case INS_DEC | INS_R4:
            R4 -= 1;
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

        case INS_SUBTRACT | INS_R1: // SBR1
            AC -= R1;
            break;

        case INS_SUBTRACT | INS_R2: // SBR2
            if (IS_DEBUG_PRINT)
            {

                printf("Accumulator..[R2]=[%d] [AC]=[%d]\n", R2, AC);
            }
            AC -= R2;
            if (IS_DEBUG_PRINT)
            {
                printf("Accumulator..[R2]=[%d] [AC]=[%d]\n", R2, AC);
            }

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
            if (IS_DEBUG_PRINT)
            {
                printf("Accumulator..[R2]=[%d] [AC]=[%d]\n", R2, AC);
            }
            AC &= R2;
            if (IS_DEBUG_PRINT)
            {
                printf("Accumulator..[R2]=[%d] [AC]=[%d]\n", R2, AC);
            }

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
            printf("RIGHTSHIFT..[R1]=[%d] RAM[%d]=[%#x]\n", R1, data, RAM[data]);
            RAM[data] = RAM[data] >> R1;
            printf("RIGHTSHIFT..[R1]=[%d] RAM[%d]=[%#x]\n", R1, data, RAM[data]);
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
            if (IS_DEBUG_PRINT)
            {
                printf("R1 = %#x, RAM[%#x]=%#x\n", R1, data, RAM[data]);
            }
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
            printf("Comparing: \t %#x > %#x\n", R1, data);
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

    int x;

    scanf("%d", &x);

    cpu.loadProgram(programs[x - 1]);

    int i;
    cin >> i;
    return 0;
}

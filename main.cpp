#include<iostream>
#include<stdint.h>
#include<fstream>
#include <bitset>
#include <map>
using namespace std;


class CPU{
    public:
        uint8_t R1 = 0x00; // First Register
        uint8_t R2 = 0x00; // Second Register
        uint8_t R3 = 0x00; // Second Register
        uint8_t R4 = 0x00; // Second Register
        uint8_t AR = 0x00; //Address Register
        uint8_t AC = 0x00; //Accumulator
        uint8_t PC = 0x00;
        uint16_t IR = 0x00;
        uint8_t SP = 0x00;
        uint8_t SL = 0x00;

        map<string, uint16_t> instructionMap;
        enum FLAG{
            ZERO,
            NEGATIVE,
            CARRY
        };

        uint8_t flags[3] = {0, 0, 0};

        uint16_t RAM[0xFF];

        CPU(){
            initMap();

            for (int address = 0; address < 0xFF; ++address)
            {
                RAM[address] = 0;
            }

        }

        void initMap(){
            //instructionMap[cmd] =
            instructionMap["HLT"] =  0b00000000;  // 0x00
            instructionMap["STR1"] = 0b10000000; // 0x80
            instructionMap["STR2"] = 0b10000001; // 0x81
            instructionMap["LDR1"] = 0b01000000; // 0x40
            instructionMap["LDR2"] = 0b01000001; // 0x41
            instructionMap["JUMP"] = 0b00100000; // 0x20
            instructionMap["ADR1"] = 0b00001000; // 0x08
            instructionMap["ADR2"] = 0b00001001; // 0x09
            instructionMap["PRNT"] = 0b00000101; // 0x05
            instructionMap["CLR"] =  0b10100000; //0xA0
            instructionMap["MOVR"] = 0b11010000; 
            instructionMap["PUSH"] = 0b01110000;  
            instructionMap["POP"] =  0b01110001;
            instructionMap["NOP"] =  0b00010011;
        }

        void loadProgram(){

            fstream file("prog.s");
            if (!file)
            {
                cout << "Error opening file\n";
            }

            string line;
            string program = "";
            uint8_t lineCount = 0;
            while(getline(file, line)){
                if(line.empty()){
                    continue;
                }
                program += line + "\n";
                lineCount++;
            }

            cout << "Program :" << endl << program;

            uint16_t* token = parse(program);
            uint8_t i;
            for (i = 0; i < lineCount; i++)
            {
                RAM[i] = token[i];
            }

            SP = i;

            cout << "\n\n\n"
                 << "Translated program\n\n\n"
                 << endl;

            for (i = 0; i < lineCount; i++)
            {
                uint8_t firstHalf = (RAM[i] >> 8) & 0x00ff;
                uint8_t secondHalf = RAM[i] & 0x00ff;

                bitset<8> firstHalfbin(firstHalf);
                bitset<8> secondHalfbin(secondHalf);

                // cout << firstHalfbin << " : " << secondHalfbin << endl;

                for (auto &pair : instructionMap)
                {
                    if ((instructionMap[pair.first] == firstHalf))
                    {
                        cout << pair.first << " 0x" << hex << static_cast<int>(secondHalf) << endl;
                        break;
                    }
                    else if (((instructionMap[pair.first] | 0x10) == firstHalf)){
                        cout << pair.first << " [0x" << hex << static_cast<int>(secondHalf)  << "]" << endl;
                        break;
                    }
                }
            }

            file.close();   
            
        }

        
        uint16_t* parse(string &program)
        {

            map<string, uint16_t> funtionMap;
            uint16_t *binaries = new uint16_t(100);
            uint16_t binaryIndex = 0;

            string buffer = "";
            int counter = 0;

            string currentInstruction = "";
            string currentData = "";
            string currentAddress = "";
            string currentFunction = "";

            for (char alphabet : program){

                if (program[counter] == ' ' || program[counter] == '\n' || program[counter] == ':')
                {
                    if (buffer.substr(0, 2) == "0x")
                    {
                        currentData = buffer;
                    }
                    else if (buffer.substr(0, 1) == "[")
                    {
                        int indexOf = buffer.find("]");
                        if (indexOf == -1){
                            throw;
                        }
                        currentAddress = buffer.substr(1, indexOf - 1);
                    }
                   
                    else if (program[counter] == ':')
                    {
                        currentFunction = buffer;
                        if (funtionMap.find(buffer) != funtionMap.end())
                        {
                            funtionMap[buffer] = counter;
                        }
                        buffer.clear();
                    }
                    else
                    {
                        currentInstruction = buffer;
                    }

                    if(!currentFunction.empty()){
                        *(binaries + binaryIndex) = ((instructionMap["NOP"] | 0x10) << 8) | funtionMap[currentFunction];
                        binaryIndex++;
                        currentFunction.clear();
                    }else if(!currentInstruction.empty() && !currentData.empty()){
                        *(binaries + binaryIndex) = (instructionMap[currentInstruction] << 8) | (stoi(currentData.substr(2, 4), nullptr, 16));
                        binaryIndex++;
                        currentInstruction.clear();
                        currentData.clear();
                    }
                    else if (!currentInstruction.empty() && !currentAddress.empty())
                    {
                        *(binaries + binaryIndex) = ((instructionMap[currentInstruction] | 0x10) << 8) |  stoi(currentAddress.substr(2, 4),nullptr,16);
                        binaryIndex++;
                        currentInstruction.clear();
                        currentAddress.clear();
                    }
                    else if (!currentInstruction.empty() && program[counter] == '\n')
                    {
                        *(binaries + binaryIndex) = instructionMap[currentInstruction] << 8;
                        binaryIndex++;
                        currentInstruction.clear();
                        currentAddress.clear();
                    }
                    buffer.clear();
                    counter++;
                    continue;
                }

                string s(1, alphabet);
                buffer += s;
                counter++;
			}

            return binaries;
        }

        void run(){
            while(RAM[PC] & 0xff00){
                execute();
                PC++;
            }
		}
    
        void execute(){
            IR = RAM[PC];
            uint16_t instruction = (IR & 0xff00) >> 8;
            uint16_t data = IR & 0x00ff;

            // cout << "IR:  " << hex << IR << "Instruction:  " << hex << instruction << endl;

            switch (instruction){
                
            case 0b10000000: // STR1
                R1 = data;
                break;

            case 0b10000001: // STR2
                R2 = data;
                break;

            case 0b10010000: // STR1 adress
                R1 = RAM[data];
                
                break;
            case 0b10010001: // STR2
                R2 = RAM[data];
                
                break;

            case 0b01010000: // LOADR1
                RAM[data] = (RAM[data] & 0xff00) + R1;

                break;
            case 0b01010001: // LOADR2
                RAM[data] = (RAM[data] & 0xff00) + R2;
                break;

            case 0b00110000: // JUMP
                PC = data;
                break;

            case 0b00000101: // PRT
                cout << data;
                break;

            case 0b00010101: // PRT Adress
                cout << hex << (RAM[data] & 0x00ff) << endl;
                break;

            case 0b00001000: // ADD Adress
                AC += R1;
                break;

            case 0b00001001: // ADD Adress
                AC += R2;
                break;

            case 0b10100000: // CLR
                AC = 0;
                break;

            case 0b10110000: // CLR at address
                RAM[data] = 0;
                break;

            case 0b11110000: // Move result of Accumulator
                RAM[data] = AC;
                break;

            case 0b00000011: // NOP
                
                break;

            // case 0b01110000: // PUSH
            //     RAM[SP + SL] = (instructionMap["JUMP"] << 8) | data;
            //     SL++;
            //     break;

            // case 0b01110001: // POP
            //     RAM[SP + SL] = data;
            //     SL--;
            //     break;

            default:
                cout << " Instruction 0x" << hex << instruction << " Not found";
                break;
            }
        }

        void add(uint8_t value){
            if(AC + value > 0xffff){
                flags[CARRY] = 1;
            }
            AC += value;
        }

        void subtract(uint8_t value){
            if(AC - value == 0){
                flags[ZERO] = 0;
            }
            AC -= value;
        }

        void showMemory(uint8_t limit=0xFF){
            for (int address = 0; address < limit; ++address){
                bitset<16> bin(RAM[address]);
                cout << "[0x" << hex << address << "]: 0x" << hex << RAM[address] << "\t  [" << bin << "]" <<  endl;
            }
        }

    };

int main(){

    CPU cpu;
    // cpu.toHex("0x56");
    cpu.loadProgram();
    // cpu.run();
    cpu.showMemory();
	int i;
   	cin >> i;
    return 0;
}

/*
0000 : STR1 --> store in R1 from MEMORY data
0001 : STR2 --> store in R2 from MEMORY data
0002 : LOAD R1
0003 : JUMP
0004 : ADD
0005 : HLT


main:
    STR1 1
    STR2 3
    LOAD R1
    LOAD R2
*/

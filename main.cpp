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
            instructionMap["HLT"] = 0b00000000;  // 0x00
            instructionMap["STR1"] = 0b10000000; // 0x80
            instructionMap["STR2"] = 0b10000001; // 0x81
            instructionMap["LDR1"] = 0b01000000; // 0x40
            instructionMap["LDR2"] = 0b01000001; // 0x41
            instructionMap["JUMP"] = 0b00100000; // 0x20
            instructionMap["ADR1"] = 0b00001000; // 0x08
            instructionMap["ADR2"] = 0b00001001; // 0x09
            instructionMap["PRNT"] = 0b00000101; // 0x05
            instructionMap["CLR"] =  0b10100000; //0xA0
            instructionMap["MOVR"] = 0b11110000; //0xF0
            instructionMap["RET"] =  0b11100000; //0xE0
        }

        void loadProgram(){

            fstream file("prog.os");
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

            for (uint8_t i = 0; i < lineCount; i++)
            {
                RAM[i] = token[i];
            }

            file.close();   
            
        }

        uint16_t* parse(string &program){
            uint16_t *binaries = new uint16_t(100);
            uint16_t binaryIndex = 0;

            string buffer = "";
            int counter = 0;

            string currentInstruction = "";
            string currentData = "";
            string currentAddress = "";
            
            for (char alphabet : program){

                if (program[counter] == ' ' || program[counter] == '\n')
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
                    else
                    {
                        currentInstruction = buffer;
                    }
                
                    if(!currentInstruction.empty() && !currentData.empty()){
                        
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
    cpu.run();
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

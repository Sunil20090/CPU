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
        uint8_t AR = 0x00; //Address Register
        uint8_t AC = 0x00; //Accumulator
        uint8_t PC = 0x00;
        uint16_t IR = 0x00;

        map<string, int> instructionMap;
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
            instructionMap["HLT"] = 0x00;
            instructionMap["STDR1"] = 0x01;
            instructionMap["STDR2"] = 0x02;
            instructionMap["ADDR1"] = 0x03;
            instructionMap["ADDR2"] = 0x04;
            instructionMap["ADAR1"] = 0x05;
            instructionMap["ADAR2"] = 0x06;
            instructionMap["JUMPA"] = 0x07;
            instructionMap["CLRA"] = 0x08;
        }

        void loadProgram(){

            fstream file("prog.os");
            if (!file)
            {
                cout << "Error opening file\n";
            }

            string line;
            string program = "";
            uint8_t lineNumber =0;
            while(getline(file, line)){
                if(line.empty()){
                    continue;
                }
                program += line + "\n";
                int ramValue = parse(line);
                cout << "Ram Value: " << hex << ramValue << endl;
                RAM[lineNumber++] = ramValue;
            }

            file.close();   

            cout << "programLoadded:\n" << program << endl;
            
        }

        uint16_t parse(string &line){
            string buffer = "";
            string instruction, data;
            int c_index = 0;
            
            for(char a : line){
                if(a == ' '){
                    instruction = buffer;
                    buffer.clear();
                }

                string currentChar(1, a);
                buffer += currentChar;

                if(c_index == line.length() - 1){
                    if(!instruction.empty()){
                        data = buffer;
                    }
                }

                c_index++;
            }

            cout << "Instruction: " << instructionMap[instruction] << " | Data: " << data << endl;
            
            return 0x01 << 8;

            if(data.empty()){
                // cout << "Instruction: " << instructionMap[instruction] << endl;
                return instructionMap[instruction] << 8;
            }else{
                // cout << "Instruction: " << instructionMap[instruction] << " | Data: " << stoi(data, nullptr, 16) << endl;
                return instructionMap[instruction] << 8 + stoi(data, nullptr, 16);
            }
        
            

            // if (index != -1)
            // {
            //     string ins = line.substr(0, index);
            //     string data = line.substr(index+1, line.length());
            //     return instructionMap[ins] << 2 + stoi(data, nullptr, 16);
            // }
            // else{
            //     
            // }

            return 0x00;
        }
		
		void tick(){
            if(RAM[PC] & 0xff00)
			PC++;
		}
    
        void execute(){
            IR = RAM[PC];
            AR = IR >> 8;
            uint8_t data = IR & 0x00ff;

            switch (AR)
            {
            case 1: //STR1
                R1 = data;
                break;
            case 2: // STR2
                R2 = data;
                break;
            case 3: // LOADR1
                add(R1);
                break;
            case 4: // LOADR2
                break;

            default:
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
                 cout << "[0x" << hex << address << "]: " << bitset<16>() << endl;
            }
        }
        
        void run(){
        	tick();
        	execute();
		}
    };

int main(){
    uint16_t prog[] = {
        0x0001, 0x0101, 0x0001, 0x0001};

    CPU cpu;
    cpu.loadProgram();
    cpu.showMemory(0x0f);

    // cpu.run();
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

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
            uint8_t lineNumber = 0;
            while(getline(file, line)){
                if(line.empty()){
                    continue;
                }
                program += line + "\n";
               
            }

            uint16_t token = parse(program);

            file.close();   

            cout << "ProgramLoadded:\n" << program << endl;
            
        }

        uint16_t parse(string &line){
            // uint16_t* leftTokens;
            string buffer = "";
            int counter = 0;
            int mode = 0; //0 instruction mode, 1 data mode
			for (char token : line){
				if(token == ' '){
                    if(mode == 0){
                        // *(leftTokens + counter) = instructionMap[buffer];
                        // counter++;
                        cout << "Buffer: " << buffer << endl;
                        buffer.clear();

                    }
                    mode = 1;
                    continue;
                }

                if(token == '\n'){
                    mode = 0;
                    cout << "Buffer: " << buffer << endl;
                    buffer.clear();
                }
                string s(1, token);
                buffer += s;
			}

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

#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"

using namespace std;

uint64_t GTUOS::handleCall(const CPU8080 & cpu){
	uint64_t cycle=10;
	uint16_t adress;
	uint8_t temp;
	uint8_t random;
	string str;

	const char *array;

	switch (cpu.state->a){
		case 1:
			cout<<(int)cpu.state->b<<endl;
			break;
		case 2:
			//uint16_t b=cpu.state->b;
			cout << (int)cpu.memory->physicalAt((cpu.state->b<<8 | cpu.state->c))<<endl;
			break;
		case 3:
			cin>>temp;
			cpu.state->b=temp;
			break;
		case 4:
			cin>>temp;
			cpu.memory->physicalAt(cpu.state->b<<8 | cpu.state->c)=temp;break;
		case 5:
            cycle=100;
			adress=cpu.state->b<<8 | cpu.state->c;
			uint8_t karakter;
			do{
				karakter=cpu.memory->physicalAt(adress);
				adress++;
				cout<<karakter;
			}while(karakter!='\0');break;
		case 6:
            cycle=100;
			cin>>str;
			adress=cpu.state->b<<8 | cpu.state->c;
			array=str.c_str();
			do{
				cpu.memory->physicalAt(adress)=*array;
				adress++;
				array++;
			}while(*array!='\0');break;
		case 7:
			random=(uint8_t)rand()%256;
			cpu.state->b=random;
			break;
		default:cout<<"Unimplemented OS call"<<endl;

	}

	return cycle;
}

void GTUOS::saveTheMemory(const CPU8080 &cpu) {
    FILE* file = fopen("exe.mem","w");

    for (int i = 0x00000; i < 0x10000 ; i += 0x00010) {
        fprintf(file,"%04x    %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x\n",
                i,cpu.memory->physicalAt(i),
                cpu.memory->physicalAt(i+0x00001),
                cpu.memory->physicalAt(i+0x00002),
                cpu.memory->physicalAt(i+0x00003),
                cpu.memory->physicalAt(i+0x00004),
                cpu.memory->physicalAt(i+0x00005),
                cpu.memory->physicalAt(i+0x00006),
                cpu.memory->physicalAt(i+0x00007),
                cpu.memory->physicalAt(i+0x00008),
                cpu.memory->physicalAt(i+0x00009),
                cpu.memory->physicalAt(i+0x0000A),
                cpu.memory->physicalAt(i+0x0000B),
                cpu.memory->physicalAt(i+0x0000C),
                cpu.memory->physicalAt(i+0x0000D),
                cpu.memory->physicalAt(i+0x0000E),
                cpu.memory->physicalAt(i+0x0000F));
    }
}

void GTUOS::printMemory(const CPU8080 &cpu) {
    for (int i = 0x00000; i < 0x10000 ; i += 0x00010) {
        printf("%04x    %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x\n",
                i,cpu.memory->physicalAt(i),
                cpu.memory->physicalAt(i+0x00001),
                cpu.memory->physicalAt(i+0x00002),
                cpu.memory->physicalAt(i+0x00003),
                cpu.memory->physicalAt(i+0x00004),
                cpu.memory->physicalAt(i+0x00005),
                cpu.memory->physicalAt(i+0x00006),
                cpu.memory->physicalAt(i+0x00007),
                cpu.memory->physicalAt(i+0x00008),
                cpu.memory->physicalAt(i+0x00009),
                cpu.memory->physicalAt(i+0x0000A),
                cpu.memory->physicalAt(i+0x0000B),
                cpu.memory->physicalAt(i+0x0000C),
                cpu.memory->physicalAt(i+0x0000D),
                cpu.memory->physicalAt(i+0x0000E),
                cpu.memory->physicalAt(i+0x0000F));
    }
}

void GTUOS::printCpuState(const CPU8080 &cpu) {
    printf("\n");
    printf("STATES:\n");
    printf("reg_a:%01x, ",cpu.state->a);
    printf("reg_b:%01x, ",cpu.state->b);
    printf("reg_c:%01x, ",cpu.state->c);
    printf("reg_d:%01x, ",cpu.state->d);
    printf("reg_e:%01x, ",cpu.state->e);
    printf("reg_h:%01x, ",cpu.state->h);
    printf("reg_l:%01x, ",cpu.state->l);
    printf("reg_sp:%02x, ",cpu.state->sp);
    printf("reg_pc:%02x\n",cpu.state->pc);
    cout<<endl;
    printf("ConditionCodes:\n");
    printf("cy:%01x, ",cpu.state->cc.cy);
    printf("pad:%01x, ",cpu.state->cc.pad);
    printf("p:%01x, ",cpu.state->cc.p);
    printf("pad2:%01x, ",cpu.state->cc.pad2);
    printf("ac:%01x, ",cpu.state->cc.ac);
    printf("pad3:%01x, ",cpu.state->cc.pad3);
    printf("z:%01x, ",cpu.state->cc.z);
    printf("s:%01x \n",cpu.state->cc.s);
    printf("---------------------------------\n");

}


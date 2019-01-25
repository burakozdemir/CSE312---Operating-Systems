#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"

using namespace std;

static const int QUANTUMTIME = 100;

void copyMem(MemoryBase *ilk,MemoryBase* ikinci){
    int i;
    for (i =0; i < 0x10000; ++i) {
        ilk->physicalAt((uint32_t)i)=ikinci->physicalAt((uint32_t)i);
    }
}
uint64_t GTUOS::handleCall(CPU8080 & cpu){
	uint64_t cycle=10;
	uint16_t adress;
	uint8_t temp;
	uint8_t random;
	string str;
    uint16_t address;

	const char *array;

	switch (cpu.state->a){
		case 1:// print_b
			cout<<(int)cpu.state->b<<endl;
			break;
		case 2://print_mem
			//uint16_t b=cpu.thread_state->b;
			cout << (int)cpu.memory->physicalAt((cpu.state->b<<8 | cpu.state->c))<<endl;
			break;
		case 3://read_B
			cin>>temp;
			cpu.state->b=temp;
			break;
		case 4://read_mem
			cin>>temp;
			cpu.memory->physicalAt(cpu.state->b<<8 | cpu.state->c)=temp;break;
		case 5://print_Str
            cycle=100;
			adress=cpu.state->b<<8 | cpu.state->c;
			uint8_t karakter;
			do{
				karakter=cpu.memory->physicalAt(adress);
				adress++;
				cout<<karakter;
			}while(karakter!='\0');break;
		case 6://read_Str
            cycle=100;
			cin>>str;
			adress=cpu.state->b<<8 | cpu.state->c;
			array=str.c_str();
			do{
				cpu.memory->physicalAt(adress)=*array;
				adress++;
				array++;
			}while(*array!='\0');break;
		case 7://get_rnd
			random=(uint8_t)rand()%256;
			cpu.state->b=random;
			break;
        case 8://TExit
        {
            Thread *exitThread=new Thread;
            exitThread=runningThread;
            cpu.state->b=exitThread->getThreadState();

            if(!threadQueue.empty()){
                runningThread=threadQueue.front();
                assigntoState(runningThread->stateOfRegister,threadQueue.front()->stateOfRegister);
                copyMem(runningThread->getThreadMem(),threadQueue.front()->getThreadMem());
                threadQueue.pop();
                runningThread->setThreadState(RUNNING);
            }

            cycle=50;
        }

            break;
        case 9://TJoin
        {
            runningThread->setThreadState(BLOCK);
            runningThread->setWaitingThreadID(runningThread->stateOfRegister.b);
            cycle=40;
        }
            break;
        case 10://TYield.Threadi sıranın sonuna koyar.
        {
            Thread *tempThread=new Thread;
            tempThread=runningThread;
            runningThread=threadQueue.front();
            threadQueue.pop();

            tempThread->setThreadState(READY);
            threadQueue.push(tempThread);

            cycle=40;
        }
            break;
        case 11://TCreate
            //param:thread_state.a da 11 var thread_state.bc de fonksyion pointer adresi
            //return:threadİD or return 0 in to bc register
        {
            Thread *thread=new Thread;
            *thread=*runningThread;

            thread->setThreadID(++threadcounter);
            thread->setStartTime(totalElapsedTime);
            thread->setThreadState(READY);
            std::string filename;
            address = calculateTheAddress(cpu.state->b, cpu.state->c);

            for (int i = 0; cpu.memory->at(address) != '\n' && cpu.memory->at(address) != '\0' ; ++i, address++)
                filename.append(1,cpu.memory->at(address));

            FILE *f= fopen(filename.c_str(), "rb");
            if (f==NULL)
            {
                printf("error: Couldn't open(my) %s\n", filename);
                runningThread->stateOfRegister.b=0;
            }

            thread->readFileIntoThreadMemory(filename.c_str(),0x0000);
            thread->setFilename(filename);
            thread->stateOfRegister.pc=0;
            cpu.state->b=(uint8_t) threadcounter;
            runningThread->stateOfRegister.b=(uint8_t)threadcounter;
            thread->stateOfRegister.b=0;

            threadQueue.push(thread);
            controlhasAThread=true;

            cycle=80;
        }
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
    printf("reg_a:%02x, ",cpu.state->a);
    printf("reg_b:%02x, ",cpu.state->b);
    printf("reg_c:%02x, ",cpu.state->c);
    printf("reg_d:%02x, ",cpu.state->d);
    printf("reg_e:%02x, ",cpu.state->e);
    printf("reg_h:%02x, ",cpu.state->h);
    printf("reg_l:%02x, ",cpu.state->l);
    printf("reg_sp:%04x, ",cpu.state->sp);
    printf("reg_pc:%04x\n",cpu.state->pc);
    cout<<endl;
    /*
    printf("ConditionCodes:\n");
    printf("cy:%01x, ",cpu.state->cc.cy);
    printf("pad:%01x, ",cpu.state->cc.pad);
    printf("p:%01x, ",cpu.state->cc.p);
    printf("pad2:%01x, ",cpu.state->cc.pad2);
    printf("ac:%01x, ",cpu.state->cc.ac);
    printf("pad3:%01x, ",cpu.state->cc.pad3);
    printf("z:%01x, ",cpu.state->cc.z);
    printf("s:%01x \n",cpu.state->cc.s);
    printf("---------------------------------\n");*/

}
bool GTUOS::isThreadDead(int threadID) {
    bool flag=false;
    int count=0;
    for (int i = 0; i < threadQueue.size() ; ++i) {
        if(threadQueue.front()->getThreadID()!=threadID){
            flag=true;
        }
        threadQueue.push(threadQueue.front());
        threadQueue.pop();
    }
    bool flag2=runningThread->getThreadID() != threadID;
    return !(flag2 && flag);
}
bool GTUOS::execute(std::string filename, int debugMode, CPU8080 &cpu) {
    char killed;
    int cost;
    int switchValue = 0;
    runningThread=new Thread;
    runningThread->setCycle(0);
    runningThread->setThreadState(READY);
    runningThread->setThreadID(0);
    runningThread->setStartTime(0);
    runningThread->setFilename(filename);
    assigntoState(runningThread->stateOfRegister,*cpu.state);
    copyMem(runningThread->getThreadMem(),cpu.memory);

    do{
        do{
            runningThread->setThreadState(RUNNING);
            if (debugMode == 1) {
                cout<<runningThread->getThreadID()<<"::>";

            }

            cost = cpu.Emulate8080p(debugMode);
            totalElapsedTime += cost;
            switchValue += cost;
            runningThread->addToCycle(cost);
            updateRunningThread(cpu);

            if (cpu.isSystemCall()) {
                cost += handleCall(cpu);
                totalElapsedTime += cost;
                switchValue += cost;
                updateRunningThread(cpu);
            }
            if(runningThread!=NULL){
                if (runningThread->getThreadState() == BLOCK) {
                    contextSwitch(cpu);
                    switchValue= 0;
                }
                else if(switchValue>QUANTUMTIME){
                    contextSwitch(cpu);
                    switchValue=0;
                }
            }
            runningThread->stateOfRegister=*cpu.state;
            copyMem(runningThread->getThreadMem(),cpu.memory);

        }while (!cpu.isHalted());
        cout << "id numbber :  " << runningThread->getThreadID() << "  Thread Exitted " << endl;
        if(threadQueue.size() == 0)
            break;
        runningThread = threadQueue.front();
        assigntoState(runningThread->stateOfRegister,threadQueue.front()->stateOfRegister);
        copyMem(runningThread->getThreadMem(),threadQueue.front()->getThreadMem());

        threadQueue.pop();
        assigntoState(*cpu.state,runningThread->stateOfRegister);
        copyMem(cpu.memory,runningThread->getThreadMem());
        cout << "Switching to :: " << runningThread->getThreadID() << endl;
    }while(runningThread!=NULL);
    //printMemory(cpu);
    //free(mymem);
}
void GTUOS::updateRunningThread(CPU8080 &cpu) {
    if(runningThread!=NULL)
    assigntoState(runningThread->stateOfRegister,*cpu.state);
}
void GTUOS::contextSwitch(CPU8080 & cpu8080) {
    if(threadQueue.size()==0)
        return;
    Thread *threadTemp=new Thread;
    threadTemp=threadQueue.front();
    int count=0;
    while(threadTemp->getThreadState()==BLOCK && count<threadQueue.size()){
        if(isThreadDead(threadTemp->getWaitingThredID())){
            threadTemp->setWaitingThreadID(0);
            threadTemp->setThreadState(READY);
            break;
        }
        threadQueue.push(threadTemp);
        threadQueue.pop();
        threadTemp=threadQueue.front();
        count++;
    }
    if(threadTemp->getThreadState()==BLOCK)
        return;
    if(runningThread->getThreadState()==RUNNING)
        runningThread->setThreadState(READY);
    Thread *pushTemp=new Thread;
    pushTemp=runningThread;
    assigntoState(pushTemp->stateOfRegister,runningThread->stateOfRegister);
    copyMem(pushTemp->getThreadMem(),runningThread->getThreadMem());

    threadQueue.pop();
    threadQueue.push(pushTemp);

    threadTemp->setThreadState(RUNNING);
    runningThread=threadTemp;
    assigntoState(runningThread->stateOfRegister,threadTemp->stateOfRegister);
    copyMem(runningThread->getThreadMem(),threadTemp->getThreadMem());

    assigntoState(*cpu8080.state,runningThread->stateOfRegister);
    copyMem(cpu8080.memory,runningThread->getThreadMem());


}
int GTUOS::calculateTheAddress(uint8_t i, uint8_t c) {
    uint16_t address = 0;
    address = i << 8;
    address = address + c;
    return address;
}
void GTUOS::assigntoState(State8080 &ilk, State8080 &ikinci) {
    ilk.a=ikinci.a;
    ilk.b=ikinci.b;
    ilk.c=ikinci.c;
    ilk.d=ikinci.d;
    ilk.e=ikinci.e;
    ilk.h=ikinci.h;
    ilk.l=ikinci.l;
    ilk.int_enable=ikinci.int_enable;
    ilk.pc=ikinci.pc;
    ilk.sp=ikinci.sp;
    ilk.cc.ac=ikinci.cc.ac;
    ilk.cc.cy=ikinci.cc.cy;
    ilk.cc.p=ikinci.cc.p;
    ilk.cc.pad=ikinci.cc.pad;
    ilk.cc.pad2=ikinci.cc.pad2;
    ilk.cc.pad3=ikinci.cc.pad3;
    ilk.cc.s=ikinci.cc.s;
    ilk.cc.z=ikinci.cc.z;
}
Thread &Thread::operator=(const Thread &other) {
    if(&other != this )
    {
        this->thread_ID = other.thread_ID;
        this->cycle = other.cycle;
        this->filename = other.filename;
        this->start_Time = other.start_Time;
        this->isDead=other.isDead;
        this->thread_state = other.thread_state;
        this->stateOfRegister= other.stateOfRegister;
    }
    return *this;
}
const string &Thread::getFilename() const {
    return filename;
}
void Thread::setFilename(const string &file) {
    this->filename=file;
}
int Thread::getThreadID() const {
    return this->thread_ID;
}
void Thread::setThreadID(int threadID) {
    this->thread_ID=threadID;
}
int Thread::getStartTime() const {
    return this->start_Time;
}
void Thread::setStartTime(int time) {
    this->start_Time=time;
}
Thread_State Thread::getThreadState() {
    return this->thread_state;
}
void Thread::setThreadState(Thread_State state) {
    this->thread_state=state;
}
int Thread::getCycle() {
    return this->cycle;
}
void Thread::setCycle(int cycle) {
    this->cycle=cycle;
}
void Thread::addToCycle(int cycle) {
    this->cycle+=cycle;
}
int Thread::getWaitingThredID() {
    return this->WaitingThreadID;
}
void Thread::setWaitingThreadID(int thrID) {
    this->WaitingThreadID=thrID;
}
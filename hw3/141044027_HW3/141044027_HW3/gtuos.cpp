#include <iostream>
#include <fstream>
#include "8080emuCPP.h"
#include "gtuos.h"

using namespace std;

static const int QUANTUMTIME = 100;
static FILE *file;


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
    char karakter;
    string filename;

	const char *array;

    time_t rawtime;
    struct tm * timeinfo;
    char curTime[32];


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
	    case 12://File create
            {
                adress=cpu.state->b<<8 | cpu.state->c;
                do{
                    karakter=cpu.memory->physicalAt(adress);
                    adress++;
                    filename.append(sizeof(karakter),karakter);
                    //cout<<karakter;
                }while(karakter!='\0');


                if(strcmp(filename.c_str(),"")!=0){

                    time ( &rawtime );
                    timeinfo = localtime ( &rawtime );
                    strcpy(curTime,asctime(timeinfo));

                    myFile newFile(filename.c_str(), countFile);
                    strcpy(newFile.creationTime,curTime);
                    strcpy(newFile.lastModificTime,curTime);
                    strcpy(newFile.lastAccesTime,curTime);

                    allFiles[countFile]=newFile;

                    countFile++;
                    cpu.state->b=1;
                }else{
                    cpu.state->b=0;
                }

            }break;
        case 13: //FileClose
            {
                int id=cpu.state->b;
                if(id>=0 && id<10 && allFiles[id].ID!=-1){

                    time ( &rawtime );
                    timeinfo = localtime ( &rawtime );
                    strcpy(curTime,asctime(timeinfo));
                    strcpy(allFiles[id].lastAccesTime,curTime);

                    allFiles[id].isOpen=0;
                    cpu.state->b=1;
                }else{
                    cpu.state->b=0;
                    cout<<"Wrong id file"<<endl;
                }
            }break;
        case 14://FileOpen
            {
                adress=cpu.state->b<<8 | cpu.state->c;
                do{
                    karakter=cpu.memory->physicalAt(adress);
                    adress++;
                    filename.append(sizeof(karakter),karakter);
                    //cout<<karakter;
                }while(karakter!='\0');

                int id=-1;
                for (int i = 0; i < 10; ++i) {
                    if(strcmp(filename.c_str(),allFiles[i].name)==0){
                        id=i;
                    }
                }
                if(id!=-1){

                    time ( &rawtime );
                    timeinfo = localtime ( &rawtime );
                    strcpy(curTime,asctime(timeinfo));
                    strcpy(allFiles[id].lastAccesTime,curTime);

                    allFiles[id].isOpen=1;
                    allFiles[id].filePointer=0;
                    cpu.state->b=id;

                }else{
                    cout<<"There is no file"<<endl;
                    cpu.state->b=0;
                }

            }
            break;
        case 15://FileRead
            {
                int id=cpu.state->e;
                int numberByte=cpu.state->d;
                if(id>=0 && id<10){
                    if(allFiles[id].isOpen==1){
                        adress=cpu.state->b<<8 | cpu.state->c;
                        string readed;

                        int i;
                        int count=0;
                        for (i = allFiles[id].filePointer ; count< numberByte && i<256; ++i) {
                            if(disk[allFiles[id].startOfBlock][i]=='\0'){
                                readed.append(sizeof(char),disk[allFiles[id].startOfBlock][i]);
                                break;
                            }else{
                                readed.append(sizeof(char),disk[allFiles[id].startOfBlock][i]);
                            }

                            count++;
                            allFiles[id].filePointer++;
                        }

                        time ( &rawtime );
                        timeinfo = localtime ( &rawtime );
                        strcpy(curTime,asctime(timeinfo));
                        strcpy(allFiles[id].lastAccesTime,curTime);

                        array=readed.c_str();
                        do{
                            cpu.memory->physicalAt(adress)=*array;
                            adress++;
                            array++;
                        }while(*array!='\0');

                        cpu.state->b=(uint8_t)i;
                    }else{
                        cout<<"File is not opened"<<endl;
                        cpu.state->b=0;
                    }
                }else{
                    printf("There is no file\n");
                    cpu.state->b=0;
                }
            }break;
            case 16://filewrite
                {
                    int id=cpu.state->e;
                    int numberByte=cpu.state->d;
                    if(id>=0 && id<10){
                        if(allFiles[id].isOpen==1){
                            adress=cpu.state->b<<8 | cpu.state->c;
                            string writed;

                            do{
                                karakter=cpu.memory->physicalAt(adress);
                                adress++;
                                writed.append(sizeof(char),karakter);
                                //cout<<karakter;
                            }while(karakter!='\0');

                            int j;
                            int count=0;
                            const char *writedChar=writed.c_str();
                            for (j = allFiles[id].filePointer; count<numberByte && j <256; ++j) {
                                disk[allFiles[id].startOfBlock][count]=writedChar[j];
                                allFiles[id].filePointer++;
                                count++;
                            }

                            time ( &rawtime );
                            timeinfo = localtime ( &rawtime );
                            strcpy(curTime,asctime(timeinfo));
                            strcpy(allFiles[id].lastAccesTime,curTime);
                            strcpy(allFiles[id].lastModificTime,curTime);

                            cpu.state->b=(uint8_t)numberByte;

                        }else{
                            cout<<"File is not opened"<<endl;
                            cpu.state->b=0;
                        }
                    }else{
                        printf("There is no file\n");
                        cpu.state->b=0;
                    }
                }break;
            case 17://fileseek
                    {
                        int seekVal=cpu.state->b<<8 | cpu.state->c;
                        int id=cpu.state->d;
                        if(id>=0 && id<10){
                            if(allFiles[id].isOpen==1){
                                allFiles[id].filePointer=seekVal;

                                time ( &rawtime );
                                timeinfo = localtime ( &rawtime );
                                strcpy(curTime,asctime(timeinfo));
                                strcpy(allFiles[id].lastAccesTime,curTime);

                                cpu.state->b=1;

                            }else{
                                cout<<"File is not opened"<<endl;
                                cpu.state->b=0;
                            }
                        }else{
                            printf("There is no file\n");
                            cpu.state->b=0;
                        }
                    }
                    break;
        case 18://dirRead
            {
                adress=cpu.state->b<<8 | cpu.state->c;
                string result;
                for (int i = 0; i < 10; ++i) {
                    if(allFiles[i].ID!=-1){
                        result.append("name:");
                        result.append(allFiles[i].name);
                        result.append(", creation: ");
                        result.append(allFiles[i].creationTime);
                        result.append(sizeof(char),'\n');
                    }
                }


            }break;
	    default:cout<<"Unimplemented OS call"<<endl;break;

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
bool GTUOS::execute(std::string filename, int debugMode, CPU8080 &cpu,char * systemFile) {
    char killed;
    int cost;
    int switchValue = 0;
    string temp;
    do{

        if(debugMode==1){
            writeToFileSystem()
        }

        cost = cpu.Emulate8080p(debugMode);
        totalElapsedTime += cost;

        if (cpu.isSystemCall()) {
            cost += handleCall(cpu);
            totalElapsedTime += cost; }

    }while (!cpu.isHalted());

    fromDiskToFile(systemFile);



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

int GTUOS::numberOfLine(const char*file){
    string s;
    ifstream myfile (file);
    int count=0;
    if (myfile.is_open())
    {
        while ( getline (myfile,s) ) {
            count++;
        }

        myfile.close();
    }
    else{ cout << "Unable to open file";}
    return count;

}

void GTUOS::fromFileToDisk(const char * file) {
    string s;
    ifstream myfile (file);

    int fileNumber=0;

    if (myfile.is_open())
    {

        getline(myfile,s);
        fileNumber=atoi(s.c_str());

        for (int i = 0; i < fileNumber; ++i) {
            getline(myfile,s);
            allFiles[i].ID=atoi(s.c_str());

            getline(myfile,s);
            strcpy(allFiles[i].name,s.c_str());

            getline(myfile,s);
            strcpy(allFiles[i].creationTime,s.c_str());


            getline(myfile,s);
            strcpy(allFiles[i].lastAccesTime,s.c_str());


            getline(myfile,s);
            strcpy(allFiles[i].lastModificTime,s.c_str());

            getline(myfile,s);
            allFiles[i].isOpen=atoi(s.c_str());

            getline(myfile,s);
            allFiles[i].filePointer=atoi(s.c_str());

            getline(myfile,s);
            allFiles[i].startOfBlock=atoi(s.c_str());

            getline(myfile,s);
            const char *data=s.c_str();

            for (int j = 0; j < s.length(); ++j) {
                disk[i][j]=data[j];
            }

            getline(myfile,s);
        }
        myfile.close();
    }

    else cout << "Unable to open file";
}

void GTUOS::fromDiskToFile(const char * file) {
    ofstream myfile;
    myfile.open (file);
    if (myfile.is_open())
    {
        myfile << countFile <<"\n";
        for (int i = 0; i < 10; ++i) {
            if(allFiles[i].ID!=-1){
                myfile << allFiles[i].ID << "\n"<<
                       allFiles[i].name <<"\n"<<
                       allFiles[i].creationTime<<
                       allFiles[i].lastAccesTime<<
                       allFiles[i].lastModificTime<<
                       allFiles[i].isOpen<<"\n"<<
                       allFiles[i].filePointer<<"\n"<<
                       allFiles[i].startOfBlock<<"\n";
                for (int j = 0; j < 256; ++j) {
                    myfile<<disk[allFiles[i].ID][j];
                }
                myfile<<"\n";
            }
        }
    }
    else cout << "Unable to open file";

}

void GTUOS::writeToFileSystem() {
    for (int i = 0; i < 10; ++i) {
        if(allFiles[i].ID!=-1){
            cout<<"FİleID:"<<allFiles[i].ID<<endl;
            cout<<"Fİlename:"<<allFiles[i].name<<endl;
            cout<<"FİleCreatıonTime:"<<allFiles[i].creationTime<<endl;
            cout<<"FİleLastAccessTİme:"<<allFiles[i].lastAccesTime<<endl;
            cout<<"FİleLastModificTİme:"<<allFiles[i].lastModificTime<<endl;
            cout<<"FİleisOPen:"<<allFiles[i].isOpen<<endl;
            cout<<"FİlefilePointer:"<<allFiles[i].filePointer<<endl;
        }
        cout<<"--------------------------"<<endl;
    }
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
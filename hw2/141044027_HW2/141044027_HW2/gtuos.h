#ifndef H_GTUOS
#define H_GTUOS

#include <queue>
#include "8080emuCPP.h"
#include "memory.h"

using  namespace std;
typedef enum{READY,BLOCK,RUNNING} Thread_State;
class Thread {
private:
    Thread_State thread_state;
    int cycle=0;
    int WaitingThreadID = -1;
    int thread_ID=0;//
    int start_Time=0;
    std::string filename;//
    MemoryBase* threadMem;

public:
    Thread & operator=(const Thread & other);
    State8080 stateOfRegister;
    bool isDead=false;

    Thread(){
        threadMem=new memory;
    }

    MemoryBase *getThreadMem(){ return threadMem;}

    void readFileIntoThreadMemory(const char* filename, uint32_t offset){
        FILE *f= fopen(filename, "rb");
        if (f==NULL)
        {
            printf("error: Couldn't open %s\n", filename);
            exit(1);
        }
        fseek(f, 0L, SEEK_END);
        int fsize = ftell(f);
        fseek(f, 0L, SEEK_SET);

        uint8_t *buffer = &threadMem->physicalAt(offset);
        fread(buffer, fsize, 1, f);
        fclose(f);
    }

    const string &getFilename()const;
    void setFilename(const string &file);
    int getThreadID()const;
    void setThreadID(int threadID);
    int getStartTime()const;
    void setStartTime(int time);
    Thread_State getThreadState();
    void setThreadState(Thread_State state);
    int getCycle();
    void setCycle(int cycle);
    void addToCycle(int c);
    int getWaitingThredID();
    void setWaitingThreadID(int thrID);

};

class GTUOS{
	public:
		uint64_t handleCall(CPU8080 & cpu);
        void saveTheMemory (const CPU8080 & cpu);
        void printMemory(const CPU8080 & cpu);
        void printCpuState(const CPU8080 & cpu);
		int calculateTheAddress(uint8_t i, uint8_t c);
		bool execute(std::string filename,int debugMode,CPU8080 &cpu);
		void updateRunningThread(CPU8080 &cpu);
		bool controlhasAThread=false;
		bool controlRunningThread=false;
		void assigntoState(State8080 & ilk,State8080 & ikinci);

private:
        void contextSwitch(CPU8080 &cpu8080);
        bool isThreadDead(int threadID);

        Thread *runningThread;
		queue<Thread*> threadQueue;
		int threadcounter=0;
		int totalElapsedTime=0;


};

#endif

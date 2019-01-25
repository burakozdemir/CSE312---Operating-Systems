#ifndef H_GTUOS
#define H_GTUOS

#include <queue>
#include <cmath>
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

class myFile{
public:
    char name[32];
    char creationTime[32];
    char lastAccesTime[32];
    char lastModificTime[32];
    int ID;
    int isOpen=0;
    int filePointer=0;
    int startOfBlock=0;
    myFile(){
        myFile("",-1);
    }
    myFile(const char *filename,int ID){
        strcpy(name,filename);
        this->ID=ID;
        strcpy(this->creationTime,"");
        strcpy(this->lastAccesTime,"");
        strcpy(this->lastModificTime,"");
        this->isOpen=0;
        this->filePointer=0;
        this->startOfBlock=0;
    }
    myFile & operator=(const myFile & other){
        if(&other != this ) {
            strcpy(this->name,other.name);
            strcpy(this->creationTime,other.creationTime);
            strcpy(this->lastAccesTime,other.lastAccesTime);
            strcpy(this->lastModificTime,other.lastModificTime);
            this->ID=other.ID;
            this->isOpen=other.isOpen;
            this->filePointer=other.filePointer;
            this->startOfBlock=other.startOfBlock;
        }
        return *this;
    }

};


class GTUOS{
	public:
		uint64_t handleCall(CPU8080 & cpu);
        void saveTheMemory (const CPU8080 & cpu);
        void printMemory(const CPU8080 & cpu);
        void printCpuState(const CPU8080 & cpu);
		int calculateTheAddress(uint8_t i, uint8_t c);
		bool execute(std::string filename,int debugMode,CPU8080 &cpu,char * systemFile);
        ////HW3

        void writeToFileSystem();
        int numberOfLine(const char *file);
        void fromFileToDisk(const char * file);
        void fromDiskToFile(const char * file);
        char disk[4096][256];
        myFile allFiles[10];
        int countFile=0;
        void refactor(){
            for (int i = 0; i < 10; ++i) {
                allFiles[i].ID=-1;
            }
        }


		/////HW2
		void updateRunningThread(CPU8080 &cpu);
		bool controlhasAThread=false;
		bool controlRunningThread=false;
		void assigntoState(State8080 & ilk,State8080 & ikinci);

private:


        ///////HW2
        void contextSwitch(CPU8080 &cpu8080);
        bool isThreadDead(int threadID);
        Thread *runningThread;
		queue<Thread*> threadQueue;
		int threadcounter=0;
		int totalElapsedTime=0;


};

#endif

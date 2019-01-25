#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"
#include <cstdlib>

using namespace std;
int start(const char* file,int debug);

int main(int argc,char *argv[]){
    if(argc!=3){
        cerr<<"Wrong argument number"<<endl;
        exit(1);
    }

    int res=start(argv[1],atoi(argv[2]));
    if(res==-1){
        cout<<"error"<<endl;
        return -1;
    }
    return 0;
}

int start(const char* file,int debug){
    int flag=debug;
    srand(time(NULL));
    int cycleTime=0;
    string temp;

    memory myMemory;
    CPU8080 cpu8080(&myMemory);
    GTUOS gtuOS;


    cpu8080.ReadFileIntoMemoryAt(file,0x0000);

    do{
        cycleTime+=cpu8080.Emulate8080p(flag);
        if(cpu8080.isSystemCall()){
            cycleTime+=gtuOS.handleCall(cpu8080);
        };
        if(debug==1){
            gtuOS.printCpuState(cpu8080);
        }
        if(debug==2){
            gtuOS.printCpuState(cpu8080);
            cin>>temp;
            cout<<endl;
        }
    }while(!cpu8080.isHalted());

    if(debug==1 || debug==0 || debug==2){
        gtuOS.saveTheMemory(cpu8080);
        cout<<"DONE"<<endl;
        cout<<"Total Cycle Time: "<<cycleTime<<endl;
        return 0;
    }
    else
        return -1;
}
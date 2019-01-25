#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"


class GTUOS{
	public:
		uint64_t handleCall(const CPU8080 & cpu);
        void saveTheMemory (const CPU8080 & cpu);
        void printMemory(const CPU8080 & cpu);
        void printCpuState(const CPU8080 & cpu);
};

#endif

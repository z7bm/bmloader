
#include <stdint.h>
#include "ps7int.h"

extern "C"
{

intptr_t PrefetchAbortAddr;
intptr_t DataAbortAddr;
intptr_t UndefinedExceptionAddr;

void IRQInterrupt()             
{ 
    const uint32_t INT_ID = read_mmr(GIC_ICCIAR);
    
    asm volatile ("    nop");
    
    write_mmr(GIC_ICCEOIR, INT_ID);
}
void FIQInterrupt()             { }
void UndefinedException()       { }
void SWInterrupt()              { }
void DataAbortInterrupt()       { }
void PrefetchAbortInterrupt()   { }

}




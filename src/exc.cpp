
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
    //clr_mmr_bits(GPIO_DATA_0_REG, 1ul << 7);
    write_mmr(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | (1ul << 7) );
    write_mmr(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | 0 );
    
    write_mmr(GPIO_INT_STAT_1_REG, 1ul << 18);
    const uint32_t GPIO_INT_STS = read_mmr(GPIO_INT_STAT_1_REG);
    //__asm__ __volatile__("    dsb");
    //__asm__ __volatile__("    isb");

    
    asm volatile ("    nop");

    
    write_mmr(GIC_ICCEOIR, INT_ID);
}
void FIQInterrupt()             { }
void UndefinedException()       { }
void SWInterrupt()              { }
void DataAbortInterrupt()       { }
void PrefetchAbortInterrupt()   { }

}




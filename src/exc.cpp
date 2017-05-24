
#include <stdint.h>
#include "ps7int.h"

extern TISRHandler PS7Handlers[PS7_MAX_IRQ_ID];

extern "C"
{

intptr_t PrefetchAbortAddr;
intptr_t DataAbortAddr;
intptr_t UndefinedExceptionAddr;

void IRQInterrupt()             
{ 
    const uint32_t INT_ID = read_pa(GIC_ICCIAR);
    //clr_mmr_bits(GPIO_DATA_0_REG, 1ul << 7);
    write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | (1ul << 7) );
    write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | 0 );
    
    (*PS7Handlers[INT_ID])();
        
    asm volatile ("    nop");
    
    write_pa(GIC_ICCEOIR, INT_ID);
}
void FIQInterrupt()             { }
void UndefinedException()       { }
void SWInterrupt()              { }
void DataAbortInterrupt()       { }
void PrefetchAbortInterrupt()   { }

}




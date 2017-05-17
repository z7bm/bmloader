
#include <ps7mmrs.h>
#include "ps7_init.h"
#include "ps7int.h"

//------------------------------------------------------------------------------
int main() 
{ 
    ps7_init();
    
    {
    //    TCritSect cs;
        gic_int_enable(PS7IRQ_ID_UART0);
        gic_set_target(PS7IRQ_ID_UART0, 0x01);
        set_mmr_bits(GIC_ICCPMR, 0xff);
        asm volatile ("    nop");
        gic_set_priority(PS7IRQ_ID_SW7, 0x10);
        
        
        set_mmr_bits(GIC_ICDDCR, 0x1);
        set_mmr_bits(GIC_ICCICR, 0x3);
    }

    enable_interrupts();
    
    
//  write_mmr_bits( GIC_ICDSGIR,                                  // 0b10: send the interrupt on only to the CPU
//                 (2 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
//                 PS7IRQ_ID_SW15                                 // rise software interrupt ID15
//                 );
          
    //write_mmr_bits(GIC_ICDISR0, 1ul << PS7IRQ_ID_SW15);                                                      
    write_mmr( GIC_ICDSGIR,                                  // 0b10: send the interrupt on only to the CPU
              (0 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
              (0x01 << GIC_ICDSGIR_CPU_TARGET_LIST_BPOS) +   //
              //(1ul << GIC_ICDSGIR_SBZ_BPOS)              +   //
               PS7IRQ_ID_SW7                                 // rise software interrupt ID15
             );
    
//    gic_set_pending(PS7IRQ_ID_UART0);    
                                                                    
    //set_mmr_bits( GIC_ICDSGIR, 0x02F00000, GIC_ICDSGIR_TARGET_LIST_FILTER_MASK);                            
    
  //  volatile uint32_t isr_id = 0;
    for(;;)
    {
        asm volatile ("    nop");
        asm volatile ("    nop");
        asm volatile ("    nop");
        asm volatile ("    nop");
//        isr_id = *(reinterpret_cast<uint32_t*>(GIC_ICCIAR));
    }
}
//------------------------------------------------------------------------------


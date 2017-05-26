
#include <ps7mmrs.h>
#include "ps7_init.h"
#include "ps7int.h"

               
const uint32_t PIN_INT = 50;

void swi_isr_handler();
void gpio_isr_handler();
void default_isr_handler();

//------------------------------------------------------------------------------
int main() 
{ 
    ps7_init();
    
    //-----------------------------------------------
    // set up output pins
    set_bits_pa(GPIO_DIRM_0_REG, 1ul << 7);
    set_bits_pa(GPIO_OEN_0_REG,  1ul << 7);
    set_bits_pa(GPIO_DATA_0_REG, 1ul << 7);
    
    set_bits_pa(GPIO_DIRM_0_REG, 1ul << 16);
    set_bits_pa(GPIO_OEN_0_REG,  1ul << 16);
    set_bits_pa(GPIO_DATA_0_REG, 1ul << 16);
    
    //  JE1
    set_bits_pa(GPIO_DIRM_0_REG, 1ul << 13);
    set_bits_pa(GPIO_OEN_0_REG,  1ul << 13);
    set_bits_pa(GPIO_DATA_0_REG, 1ul << 13);
    
    //  JE2
    set_bits_pa(GPIO_DIRM_0_REG, 1ul << 10);
    set_bits_pa(GPIO_OEN_0_REG,  1ul << 10);
    set_bits_pa(GPIO_DATA_0_REG, 1ul << 10);

        
    //-----------------------------------------------
    // initialize interrupt handlers table
    for(uint32_t i = 0; i < PS7_MAX_IRQ_ID; ++i)
    {
        ps7_register_isr_handler(&default_isr_handler, i);
    }
    
    ps7_register_isr_handler(&swi_isr_handler, PS7IRQ_ID_SW7);
    ps7_register_isr_handler(&gpio_isr_handler, PS7IRQ_ID_GPIO);
    
    //-----------------------------------------------
    // set up GPIO interrupt
    gpio_clr_int_sts(PIN_INT);
    gpio_int_pol(PIN_INT, GPIO_INT_POL_HIGH_RISE);
    gpio_int_en(PIN_INT);
        
    
    {
    //    TCritSect cs;
        gic_int_enable(PS7IRQ_ID_UART0);
        gic_set_target(PS7IRQ_ID_UART0, 0x01);
        
        gic_set_target(PS7IRQ_ID_GPIO, 1ul << GIC_CPU0);
        gic_set_config(PS7IRQ_ID_GPIO, GIC_EDGE_SINGLE);
        gic_int_enable(PS7IRQ_ID_GPIO);
        
        set_bits_pa(GIC_ICCPMR, 0xff);
        asm volatile ("    nop");
        gic_set_priority(PS7IRQ_ID_SW7, 0x10);
        
        
        set_bits_pa(GIC_ICDDCR, 0x1);
        set_bits_pa(GIC_ICCICR, 0x1);
    }

    enable_interrupts();
    
    for(;;)
    {
        asm volatile ("    nop");
        asm volatile ("    nop");

        write_pa( GIC_ICDSGIR,                                  // 0b10: send the interrupt on only to the CPU
                 (2 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
                  PS7IRQ_ID_SW7                                 // rise software interrupt ID15
                );

//      write_pa( GIC_ICDSGIR,                                  // 0b10: send the interrupt on only to the CPU
//               (0 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
//               (0x01 << GIC_ICDSGIR_CPU_TARGET_LIST_BPOS) +   //
//                PS7IRQ_ID_SW7                                 // rise software interrupt ID15
//              );
    }
}
//------------------------------------------------------------------------------
void gpio_isr_handler()
{
    //write_pa(GPIO_INT_STAT_1_REG, 1ul << 18);
    gpio_clr_int_sts(PIN_INT);

    write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | (1ul << 7) );
    write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | 0 );

    write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | (1ul << 13) );  // JE1
    write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 13) << 16) | 0 );
}
//------------------------------------------------------------------------------
void swi_isr_handler()
{
    __nop();
    write_pa(GPIO_MASK_DATA_0_MSW_REG, (~(1ul << 0) << 16) | (1ul << 0) );
    write_pa(GPIO_MASK_DATA_0_MSW_REG, (~(1ul << 0) << 16) | 0 );
    
    write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | (1ul << 10) );  // JE2
    write_pa(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 10) << 16) | 0 );

}
//------------------------------------------------------------------------------
void default_isr_handler()
{
    __nop();
    __dsb();
    __isb();
}
//------------------------------------------------------------------------------


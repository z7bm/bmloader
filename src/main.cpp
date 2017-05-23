
#include <ps7mmrs.h>
#include "ps7_init.h"
#include "ps7int.h"

               
const uint32_t PIN_INT = 50;

//------------------------------------------------------------------------------
INLINE void gpio_int_en(const uint32_t id)
{
    const uint32_t  REG_ADDR = GPIO_INT_EN_0_REG + id/32*0x40;
    const uint32_t  BIT_MASK = 0x1ul << id%32;

    write_mmr(REG_ADDR, BIT_MASK);
}
//------------------------------------------------------------------------------
enum TGpioIntPol : uint32_t
{
    GPIO_INT_POL_LOW_FALL  = 0,
    GPIO_INT_POL_HIGH_RISE = 1
};

INLINE void gpio_int_pol(const uint32_t id, const TGpioIntPol)
{
    const uint32_t  REG_ADDR = GPIO_INT_POLARITY_0_REG + id/32*0x40;
    const uint32_t  BIT_MASK = 0x1ul << id%32;

    set_mmr_bits(REG_ADDR, BIT_MASK);
}
//------------------------------------------------------------------------------
int main() 
{ 
    ps7_init();
    
    set_mmr_bits(GPIO_DIRM_0_REG, 1ul << 7);
    set_mmr_bits(GPIO_OEN_0_REG,  1ul << 7);
    set_mmr_bits(GPIO_DATA_0_REG, 1ul << 7);
    
    set_mmr_bits(GPIO_DIRM_0_REG, 1ul << 16);
    set_mmr_bits(GPIO_OEN_0_REG,  1ul << 16);
    set_mmr_bits(GPIO_DATA_0_REG, 1ul << 16);

    //clr_mmr_bits(MIO_PIN_50_REG, MIO_PIN_50_PULLUP_MASK); // turn off pullup resistor
    
    
    asm volatile ("    nop");
    // set up GPIO interrupt
    gpio_int_pol(PIN_INT, GPIO_INT_POL_HIGH_RISE);
    gpio_int_en(PIN_INT);
    
    
    {
    //    TCritSect cs;
        gic_int_enable(PS7IRQ_ID_UART0);
        gic_set_target(PS7IRQ_ID_UART0, 0x01);
        
        gic_int_enable(PS7IRQ_ID_GPIO);
        gic_set_target(PS7IRQ_ID_GPIO, 0x01);
        
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
//  write_mmr( GIC_ICDSGIR,                                  // 0b10: send the interrupt on only to the CPU
//            (0 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
//            (0x01 << GIC_ICDSGIR_CPU_TARGET_LIST_BPOS) +   //
//            //(1ul << GIC_ICDSGIR_SBZ_BPOS)              +   //
//             PS7IRQ_ID_SW7                                 // rise software interrupt ID15
//           );
    
//    gic_set_pending(PS7IRQ_ID_UART0);    
                                                                    
    //set_mmr_bits( GIC_ICDSGIR, 0x02F00000, GIC_ICDSGIR_TARGET_LIST_FILTER_MASK);                            
    
  //  volatile uint32_t isr_id = 0;
//    uint16_t val = 1ul << 7;
    for(;;)
    {
        asm volatile ("    nop");
        asm volatile ("    nop");

//        write_mmr(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | val );

//      write_mmr(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | (1ul << 7) );
//      write_mmr(GPIO_MASK_DATA_0_LSW_REG, (~(1ul << 7) << 16) | (0ul << 7) );
        
//      write_mmr( GIC_ICDSGIR,                                  // 0b10: send the interrupt on only to the CPU
//                (0 << GIC_ICDSGIR_TARGET_LIST_FILTER_BPOS) +   // interface that requested the interrupt
//                (0x01 << GIC_ICDSGIR_CPU_TARGET_LIST_BPOS) +   //
//                 PS7IRQ_ID_SW7                                 // rise software interrupt ID15
//               );

//      asm volatile ("    nop");
//      asm volatile ("    nop");

//        isr_id = *(reinterpret_cast<uint32_t*>(GIC_ICCIAR));
    }
}
//------------------------------------------------------------------------------


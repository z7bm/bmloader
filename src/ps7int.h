//******************************************************************************
//*
//*      Xilinx zynq7000 PS7 Interrupts Support Header File
//*
//*      Version 1.0
//*
//*      Copyright (c) 2017, emb-lib Project Team
//*
//*      This file is part of the zynq7000 library project.
//*      Visit https://github.com/emb-lib/zynq7000 for new versions.
//*
//*      Permission is hereby granted, free of charge, to any person obtaining
//*      a copy of this software and associated documentation files (the
//*      "Software"), to deal in the Software without restriction, including
//*      without limitation the rights to use, copy, modify, merge, publish,
//*      distribute, sublicense, and/or sell copies of the Software, and to
//*      permit persons to whom the Software is furnished to do so, subject to
//*      the following conditions:
//*
//*      The above copyright notice and this permission notice shall be included
//*      in all copies or substantial portions of the Software.
//*
//*      THE SOFTWARE  IS PROVIDED  "AS IS", WITHOUT  WARRANTY OF  ANY KIND,
//*      EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//*      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//*      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//*      CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//*      TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
//*      THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//------------------------------------------------------------------------------


#ifndef PS7INT_H
#define PS7INT_H

#include "ps7common.h"
#include <ps7mmrs.h>

//------------------------------------------------------------------------------
INLINE void gic_int_enable(const uint32_t id)
{
    const uint32_t  REG_INDEX    = id/32;
    const uint32_t  BIT_MASK     = 0x1ul << id%32;
    const uintptr_t ICDISER_ADDR = GIC_ICDISER0 + REG_INDEX*4;

    write_pa(ICDISER_ADDR, BIT_MASK);
}
//------------------------------------------------------------------------------
INLINE void gic_int_disable(const uint32_t id)
{
    const uint32_t  REG_INDEX    = id/32;
    const uint32_t  BIT_POS      = id%32;
    const uintptr_t ICDISER_ADDR = GIC_ICDISER0 + REG_INDEX*4;

    write_pa(ICDISER_ADDR, 0x1ul << BIT_POS);
}
//------------------------------------------------------------------------------
INLINE void gic_set_target(const uint32_t id, uint32_t trg)
{
    const uint32_t  REG_INDEX    = id/4;
    const uint32_t  BIT_MASK     = trg << (id%4)*8;
    const uintptr_t ICDIPTR_ADDR = GIC_ICDIPTR0 + REG_INDEX*4;

    set_bits_pa(ICDIPTR_ADDR, BIT_MASK);
}
//------------------------------------------------------------------------------
INLINE void gic_set_config(const uint32_t id, uint32_t cfg)
{
    const uint32_t  REG_INDEX    = id/16;
    const uint32_t  BIT_MASK     = cfg << (id%16)*2;
    const uintptr_t ICDIPTR_ADDR = GIC_ICDIPTR0 + REG_INDEX*4;

    set_bits_pa(ICDIPTR_ADDR, BIT_MASK);
}
//------------------------------------------------------------------------------
INLINE void gic_set_priority(const uint32_t id, uint32_t pr)
{
    const uint32_t  REG_INDEX    = id/4;
    const uint32_t  BIT_MASK     = pr << (id%4)*8;
    const uintptr_t ICDIPR_ADDR = GIC_ICDIPR0 + REG_INDEX*4;

    set_bits_pa(ICDIPR_ADDR, BIT_MASK);
}
//------------------------------------------------------------------------------
INLINE void gic_set_pending(const uint32_t id)
{
    const uint32_t  REG_INDEX    = id/32;
    const uint32_t  BIT_MASK     = 0x1ul << id%32;
    const uintptr_t ICDISPR_ADDR = GIC_ICDISPR0 + REG_INDEX*4;

    write_pa(ICDISPR_ADDR, BIT_MASK);
}
//------------------------------------------------------------------------------
enum TGicConfigField
{
    GIC_LEVEL_ALL    = 0,
    GIC_LEVEL_SINGLE = 1,
    GIC_EDGE_ALL     = 2,
    GIC_EDGE_SINGLE  = 3,
    GIC_CFG_SDI      = GIC_EDGE_ALL,
    GIC_CFG_CPU      = GIC_EDGE_SINGLE,
    GIC_CFG_SWDT     = GIC_EDGE_SINGLE,
    GIC_CFG_GEM_WUP  = GIC_EDGE_SINGLE,
    GIC_CFG_SCU      = GIC_EDGE_SINGLE,
    GIC_CFG_PERIPH   = GIC_LEVEL_SINGLE
};

enum TGicCpuID
{
    GIC_CPU0  = 0,
    GIC_CPU1  = 1
};
//------------------------------------------------------------------------------
INLINE void enable_interrupts()  { asm volatile ("    cpsie i \n"); }
INLINE void disable_interrupts() { asm volatile ("    cpsid i \n"); }
//------------------------------------------------------------------------------
INLINE uint32_t get_int_state()
{
    uint32_t cpsr;
    asm volatile 
    (
        "    mrs %0, cpsr \n"
        "    cpsid i;     \n"
        : "=r"(cpsr)
        : 
    );
    return cpsr;
}
//------------------------------------------------------------------------------
INLINE void set_int_state(uint32_t state)
{
    asm volatile
    (
        "    msr cpsr_c, %0 \n"
        : 
        : "r"(state)
    );
}
//------------------------------------------------------------------------------
class TCritSect
{
    uint32_t CPSR;
    
public:
    TCritSect() : CPSR( get_int_state() ) { }
    ~TCritSect() { set_int_state(CPSR); }
};
//------------------------------------------------------------------------------
//
//   Software Generated Interrupts (SGI)
//
//  Each CPU can interrupt itself, the other CPU, or both CPUs using a software generated
//  interrupt (SGI). There are 16 software generated interrupts (see Table 7-2 in ug585).
//  An SGI is generated by writing the SGI interrupt number to the ICDSGIR register and
//  specifying the target CPU(s). This write occurs via the CPU's own private bus.
//  Each CPU has its own set of SGI registers to generate one or more of the 16 software
//  generated interrupts. The interrupts are cleared by reading the ICCIAR (Interrupt
//  Acknowledge)  register or writing a 1 to the corresponding bits of the ICDICPR
//  (Interrupt Clear-Pending) register.
//
//  All SGIs are edge triggered. The sensitivity types for SGIs are fixed and cannot be
//  changed; the ICDICFR0 register is read-only, since it specifies the sensitivity types
//  of all the 16 SGIs.
//
//  A set of 16 interrupt sources that are private to each CPU that can be routed to up
//  to 16 common interrupt  destinations where each destination can be one or more CPUs.
//
const uint32_t PS7IRQ_ID_SW0   = 0;   // Rising edge
const uint32_t PS7IRQ_ID_SW1   = 1;   // Rising edge
const uint32_t PS7IRQ_ID_SW2   = 2;   // Rising edge
const uint32_t PS7IRQ_ID_SW3   = 3;   // Rising edge
const uint32_t PS7IRQ_ID_SW4   = 4;   // Rising edge
const uint32_t PS7IRQ_ID_SW5   = 5;   // Rising edge
const uint32_t PS7IRQ_ID_SW6   = 6;   // Rising edge
const uint32_t PS7IRQ_ID_SW7   = 7;   // Rising edge
const uint32_t PS7IRQ_ID_SW8   = 8;   // Rising edge
const uint32_t PS7IRQ_ID_SW9   = 9;   // Rising edge
const uint32_t PS7IRQ_ID_SW10  = 10;  // Rising edge
const uint32_t PS7IRQ_ID_SW11  = 11;  // Rising edge
const uint32_t PS7IRQ_ID_SW12  = 12;  // Rising edge
const uint32_t PS7IRQ_ID_SW13  = 13;  // Rising edge
const uint32_t PS7IRQ_ID_SW14  = 14;  // Rising edge
const uint32_t PS7IRQ_ID_SW15  = 15;  // Rising edge

//------------------------------------------------------------------------------
//
//   CPU Private Peripheral Interrupts (PPI)
//
//  Each CPU connects to a private set of five peripheral interrupts. The PPIs are listed
//  in Table 7-3, ug585. The sensitivity types for PPIs are fixed and cannot be changed;
//  therefore, the ICDICFR1 register is read-only, since it specifies the sensitivity types
//  of all the 5 PPIs. Note that the fast interrupt (FIQ) signal and the interrupt (IRQ)
//  signal from the PL are inverted and then sent to the interrupt controller. Therefore,
//  they are active High at the PS-PL interface, although the ICDICFR1 register reflects
//  them as active Low level.
//
const uint32_t PS7IRQ_ID_GTMR  = 27;  // PPI0, Rising edge
const uint32_t PS7IRQ_ID_nFIQ  = 28;  // PPI1, Active Low level (active High at PS-PL interface)
const uint32_t PS7IRQ_ID_PTMR  = 29;  // PPI2, Rising edge
const uint32_t PS7IRQ_ID_AWDT  = 30;  // PPI3, Rising edge
const uint32_t PS7IRQ_ID_nIRQ  = 31;  // PPI4, Active Low level (active High at PS-PL interface)

//------------------------------------------------------------------------------
//
//   Shared Peripheral Interrupts (SPI)
//
//  A group of approximately 60 interrupts from various modules can be routed to one or
//  both of the CPUs or the PL. The interrupt controller manages the prioritization and
//  reception of these interrupts for the CPUs. Except for IRQ #61 through #68 and #84
//  through #91, all interrupt sensitivity types are fixed by the requesting sources and
//  cannot be changed. The GIC must be programmed to accommodate this. The boot ROM does
//  not program these registers; therefore the SDK device drivers must program the GIC to
//  accommodate these sensitivity types.
//
//  For an interrupt of level sensitivity type, the requesting source must provide a mechanism
//  for the interrupt handler to clear the interrupt after the interrupt has been acknowledged.
//  This requirement applies to any IRQF2P[n] (from PL) with a high level sensitivity type.
//
//  For an interrupt of rising edge sensitivity, the requesting source must provide a pulse
//  wide enough for the GIC to catch. This is normally at least 2 CPU_2x3x periods. This
//  requirement applies to any IRQF2P[n] (from PL) with a rising edge sensitivity type.
//
//  The ICDICFR2 through ICDICFR5 registers configure the interrupt types of all the SPIs.
//  Each interrupt has a 2-bit field, which specifies sensitivity type and handling model.
//
const uint32_t PS7IRQ_ID_CPU0        = 32;   // Rising edge, SPI STS0 [0]
const uint32_t PS7IRQ_ID_CPU1        = 33;   // Rising edge, SPI STS0 [1]
const uint32_t PS7IRQ_ID_L2CACHE     = 34;   // Hight level, SPI STS0 [2]
const uint32_t PS7IRQ_ID_OCM         = 35;   // Hight level, SPI STS0 [3]
const uint32_t PS7IRQ_ID_PMU0        = 37;   // Hight level, SPI STS0 [5]
const uint32_t PS7IRQ_ID_PMU1        = 38;   // Hight level, SPI STS0 [6]
const uint32_t PS7IRQ_ID_XADC        = 39;   // Hight level, SPI STS0 [7]
const uint32_t PS7IRQ_ID_DEVC        = 40;   // Hight level, SPI STS0 [8]
const uint32_t PS7IRQ_ID_SWDT        = 41;   // Rising edge, SPI STS0 [9]

const uint32_t PS7IRQ_ID_TTC0_0      = 42;   // Hight level, SPI STS0 [10]
const uint32_t PS7IRQ_ID_TTC0_1      = 43;   // Hight level, SPI STS0 [11]
const uint32_t PS7IRQ_ID_TTC0_2      = 44;   // Hight level, SPI STS0 [12]

const uint32_t PS7IRQ_ID_DMAC_ABORT  = 45;   // Hight level, SPI STS0 [13]
const uint32_t PS7IRQ_ID_DMAC0       = 46;   // Hight level, SPI STS0 [14]
const uint32_t PS7IRQ_ID_DMAC1       = 47;   // Hight level, SPI STS0 [15]
const uint32_t PS7IRQ_ID_DMAC2       = 48;   // Hight level, SPI STS0 [16]
const uint32_t PS7IRQ_ID_DMAC3       = 49;   // Hight level, SPI STS0 [17]

const uint32_t PS7IRQ_ID_SMC         = 50;   // Hight level, SPI STS0 [18]
const uint32_t PS7IRQ_ID_QSPI        = 51;   // Hight level, SPI STS0 [19]

const uint32_t PS7IRQ_ID_GPIO        = 52;   // Hight level, SPI STS0 [20]
const uint32_t PS7IRQ_ID_USB0        = 53;   // Hight level, SPI STS0 [21]
const uint32_t PS7IRQ_ID_GEM0        = 54;   // Hight level, SPI STS0 [22]
const uint32_t PS7IRQ_ID_GEM0_WAKEUP = 55;   // Rising edge, SPI STS0 [23]
const uint32_t PS7IRQ_ID_SD0         = 56;   // Hight level, SPI STS0 [24]
const uint32_t PS7IRQ_ID_I2C0        = 57;   // Hight level, SPI STS0 [25]
const uint32_t PS7IRQ_ID_SPI0        = 58;   // Hight level, SPI STS0 [26]
const uint32_t PS7IRQ_ID_UART0       = 59;   // Hight level, SPI STS0 [27]
const uint32_t PS7IRQ_ID_CAN0        = 60;   // Hight level, SPI STS0 [28]

const uint32_t PS7IRQ_ID_PL0         = 61;   // Rising edge/Hight level, SPI STS0 [29]
const uint32_t PS7IRQ_ID_PL1         = 62;   // Rising edge/Hight level, SPI STS0 [30]
const uint32_t PS7IRQ_ID_PL2         = 63;   // Rising edge/Hight level, SPI STS0 [31]
const uint32_t PS7IRQ_ID_PL3         = 64;   // Rising edge/Hight level, SPI STS1 [0]
const uint32_t PS7IRQ_ID_PL4         = 65;   // Rising edge/Hight level, SPI STS1 [1]
const uint32_t PS7IRQ_ID_PL5         = 66;   // Rising edge/Hight level, SPI STS1 [2]
const uint32_t PS7IRQ_ID_PL6         = 67;   // Rising edge/Hight level, SPI STS1 [3]
const uint32_t PS7IRQ_ID_PL7         = 68;   // Rising edge/Hight level, SPI STS1 [4]

const uint32_t PS7IRQ_ID_TTC1_0      = 69;   // Hight level, SPI STS1 [5]
const uint32_t PS7IRQ_ID_TTC1_1      = 70;   // Hight level, SPI STS1 [6]
const uint32_t PS7IRQ_ID_TTC1_2      = 71;   // Hight level, SPI STS1 [7]

const uint32_t PS7IRQ_ID_DMAC4       = 72;   // Hight level, SPI STS1 [8]
const uint32_t PS7IRQ_ID_DMAC5       = 73;   // Hight level, SPI STS1 [9]
const uint32_t PS7IRQ_ID_DMAC6       = 74;   // Hight level, SPI STS1 [10]
const uint32_t PS7IRQ_ID_DMAC7       = 75;   // Hight level, SPI STS1 [11]

const uint32_t PS7IRQ_ID_USB1        = 76;   // Hight level, SPI STS1 [12]
const uint32_t PS7IRQ_ID_GEM1        = 77;   // Hight level, SPI STS1 [13]
const uint32_t PS7IRQ_ID_GEM1_WAKEUP = 78;   // Rising edge, SPI STS1 [14]
const uint32_t PS7IRQ_ID_SD1         = 79;   // Hight level, SPI STS1 [15]
const uint32_t PS7IRQ_ID_I2C1        = 80;   // Hight level, SPI STS1 [16]
const uint32_t PS7IRQ_ID_SPI1        = 81;   // Hight level, SPI STS1 [17]
const uint32_t PS7IRQ_ID_UART1       = 82;   // Hight level, SPI STS1 [18]
const uint32_t PS7IRQ_ID_CAN1        = 83;   // Hight level, SPI STS1 [19]

const uint32_t PS7IRQ_ID_PL8         = 84;   // Rising edge/Hight level, SPI STS1 [20]
const uint32_t PS7IRQ_ID_PL9         = 85;   // Rising edge/Hight level, SPI STS1 [21]
const uint32_t PS7IRQ_ID_PL10        = 86;   // Rising edge/Hight level, SPI STS1 [22]
const uint32_t PS7IRQ_ID_PL11        = 87;   // Rising edge/Hight level, SPI STS1 [23]
const uint32_t PS7IRQ_ID_PL12        = 88;   // Rising edge/Hight level, SPI STS1 [24]
const uint32_t PS7IRQ_ID_PL13        = 89;   // Rising edge/Hight level, SPI STS1 [25]
const uint32_t PS7IRQ_ID_PL14        = 90;   // Rising edge/Hight level, SPI STS1 [26]
const uint32_t PS7IRQ_ID_PL15        = 91;   // Rising edge/Hight level, SPI STS1 [27]

const uint32_t PS7IRQ_ID_SCU_PARITY  = 92;   // Rising edge, SPI STS1 [28]

const uint32_t PS7_MAX_IRQ_ID        = PS7IRQ_ID_SCU_PARITY;

//------------------------------------------------------------------------------
//
//    ISR Handlers
//
typedef void (*TISRHandler)();

void ps7_register_isr_handler(TISRHandler ptr, uint32_t id);


//------------------------------------------------------------------------------
//
//    GPIO support
// 
INLINE void gpio_int_en(const uint32_t id)
{
    const uint32_t  REG_ADDR = GPIO_INT_EN_0_REG + id/32*0x40;
    const uint32_t  BIT_MASK = 0x1ul << id%32;

    write_pa(REG_ADDR, BIT_MASK);
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

    set_bits_pa(REG_ADDR, BIT_MASK);
}
//------------------------------------------------------------------------------

#endif  // PS7INT_H


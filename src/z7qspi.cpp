//******************************************************************************
//*
//*      Xilinx zynq7000 QSPI Support Source File
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


#include <z7qspi.h>

//------------------------------------------------------------------------------
void TQspi::init(bool manmode)
{
    write_pa(QSPI_EN_REG, 0);                                          // disable QSPI module
    //clr_bits_pa(QSPI_CONFIG_REG, QSPI_MANUAL_CS_MASK | QSPI_PCS_MASK | (1ul << 11)); // turn off nCS

    //  software reset
    
    // reset QSPI clocks
    slcr_unlock();
    const uint32_t RST_CLK_MASK = QSPI_RST_CTRL_REF_RST_MASK | QSPI_RST_CTRL_CPU1X_RST_MASK;
    write_pa(QSPI_RST_CTRL_REG, RST_CLK_MASK);            
    write_pa(QSPI_RST_CTRL_REG, 0);
    slcr_lock();
    
    write_pa(QSPI_RX_THRES_REG, 1);
    write_pa(QSPI_TX_THRES_REG, 1);
    
    //  set up configuration registers
    
    clr_bits_pa(QSPI_LQSPI_CFG_REG, QSPI_LQ_MODE_MASK);  // turn off linear mode
    
    const uint32_t MAN_MODE = manmode ? QSPI_MAN_START_EN_MASK : 0;
    const uint32_t SET_MASK = QSPI_IFMODE_MASK     +     //  flash interface in Flash I/O Mode
                              MAN_MODE             +     //  
                              QSPI_PCS_MASK        +     //  set nCS to 1
                              QSPI_FIFO_WIDTH_MASK +     //  0b11: 32 bit, the only this value supported
                              QSPI_MODE_SEL_MASK   +     //  Master Mode on
                              QSPI_HOLDB_DR_MASK   +     //  
                              QSPI_CLK_PH_MASK     +     //
                              QSPI_CLK_POL_MASK;         //
        
    
    const uint32_t CLR_MASK = QSPI_BAUD_RATE_DIV_MASK +  //  set value 000: divide by 2
                              (7ul << 11)             +  //  reserved, 0
                              QSPI_ENDIAN_MASK        +  //  little endian
                              QSPI_REF_CLK_MASK;         //  reserved, must be 0
                                                         //
    __nop();
    __nop();
    set_bits_pa(QSPI_CONFIG_REG, SET_MASK, CLR_MASK);
    
    write_pa(QSPI_EN_REG, 1);                            // enable QSPI module
    
    
}
//------------------------------------------------------------------------------



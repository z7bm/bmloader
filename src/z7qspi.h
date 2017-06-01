//******************************************************************************
//*
//*      Xilinx zynq7000 QSPI Support Header File
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


#ifndef Z7INT_H
#define Z7INT_H

#include "z7common.h"
#include <ps7mmrs.h>

//------------------------------------------------------------------------------
//
//    Quad-SPI 
//
//    Notes:
//    ~~~~~
//    Default initialization brings modude to I/O Flash Mode with manual start 
//    transfer. That is, 
//     
//        CONFIG_REG[MAN_START_EN] = 1
//        CONFIG_REG[MANUAL_CS]    = 0
//        CONFIG_REG[PCS]          = 1
// 
//    In this mode the user's software should write data into TxFIFO buffer and
//    then issue write to CONFIG_REG[MAN_START_COM] = 1 (write-only bit). This
//    begins transfer: hardware asserts nCS line, send out/receive to data
//    through data I/O lines and deasserts nCS. In this case transfer length
//    is limited with 252 bytes (63 words).
// 
//    For full manual control the user's software must turn on nCS line direct 
//    control: CONFIG_REG[MANUAL_CS] = 1. This case, transaction consists of 
//     
//        * load data to TxFIFO buffer;
//        * assert nCS by CONFIG_REG[PCS] = 0;
//        * issue manual start command by CONFIG_REG[MAN_START_COM] = 1;
//        * supply output buffer with data (output data or dummy data 
//          in case of flash reading) if need;
//        * deassert nCS by CONFIG_REG[PCS] = 1;
// 
//    This method is little bit intrusive but is not limited with transfer 
//    length. By the way, even when CONFIG_REG[MANUAL_CS] = 1 if manual start
//    command is issued without manual asserting of nCS line, hardware controls
//    nCS - asserts before data send/receive and deasserts after.
//
class TQspi
{
public:
    TQspi() { }
    
    void init(bool manmode = true);
    
    void cs_on  () { clr_bits_pa(QSPI_CONFIG_REG, QSPI_PCS_MASK); }
    void cs_off () { set_bits_pa(QSPI_CONFIG_REG, QSPI_PCS_MASK); }

    void cs_enable  () { set_bits_pa(QSPI_CONFIG_REG, QSPI_MANUAL_CS_MASK); }
    void cs_disable () { clr_bits_pa(QSPI_CONFIG_REG, QSPI_MANUAL_CS_MASK); }
    
    void manual_mode_on  () { set_bits_pa(QSPI_CONFIG_REG, QSPI_MAN_START_EN_MASK); }
    void manual_mode_off () { clr_bits_pa(QSPI_CONFIG_REG, QSPI_MAN_START_EN_MASK); }
    
    void start_transfer  () { set_bits_pa(QSPI_CONFIG_REG, QSPI_MAN_START_COM_MASK); }
    
    uint32_t read_id() { write_pa(QSPI_TXD0_REG, 0x00000090); return read_pa(QSPI_RX_DATA_REG); }
    
private:
    
};
//------------------------------------------------------------------------------

#endif  // Z7INT_H


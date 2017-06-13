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
class TQSpi
{
public:
    TQSpi(uint32_t *buf) : CfgReg(0)
                         , CmdIndex(3)
                         , Address(0)
                         , Dest(buf)
                         , Count(0)
                         , Response(0)
                         , Launch(false) 
    { 
    }
    
    void init(bool manmode = true);
    
    void cs_on()  { CfgReg &= ~QSPI_PCS_MASK; write_pa(QSPI_CONFIG_REG, CfgReg); }
    void cs_off() { CfgReg |=  QSPI_PCS_MASK; write_pa(QSPI_CONFIG_REG, CfgReg); }

    void man_cs_enable()  { CfgReg &= ~QSPI_MANUAL_CS_MASK; write_pa(QSPI_CONFIG_REG, CfgReg); }
    void man_cs_disable() { CfgReg |=  QSPI_MANUAL_CS_MASK; write_pa(QSPI_CONFIG_REG, CfgReg); }
    
    void manual_mode_on  () { CfgReg &= ~QSPI_MAN_START_EN_MASK; write_pa(QSPI_CONFIG_REG, CfgReg); }
    void manual_mode_off () { CfgReg |=  QSPI_MAN_START_EN_MASK; write_pa(QSPI_CONFIG_REG, CfgReg); }
    
    void start_transfer()   { CfgReg |= QSPI_MAN_START_COM_MASK; write_pa(QSPI_CONFIG_REG, CfgReg); }
    
  //  uint32_t read_id() { write_pa(QSPI_TXD0_REG, 0x00000090); return read_pa(QSPI_RX_DATA_REG); }
    
    void run();
    
    enum TCommandCode : uint32_t
    {
        // status
        cmdREAD_ID   = 0x90,
        cmdRDID      = 0x9f,
        cmdRES       = 0xab,
        
        // register access
        cmdRDSR1     = 0x05,
        cmdRDSR2     = 0x07,
        cmdRDCR      = 0x35,
        cmdWRR       = 0x01,
        cmdWRDI      = 0x04,
        cmdWREN      = 0x06,
        cmdCLSR      = 0x30,
        cmdABRD      = 0x14,
        cmdABWR      = 0x15,
        cmdBRRD      = 0x16,
        cmdBRWR      = 0x17,
        cmdBRAC      = 0xb9,
        cmdDLPRD     = 0x41,
        cmdPNVDLR    = 0x43,
        cmdWVDLR     = 0x4a,
        
        // read flash array
        cmdREAD      = 0x03,
        cmdFAST_READ = 0x0b,
        cmdDOR       = 0x3b,
        cmdQOR       = 0x6b,
        cmdDIOR      = 0xbb,
        cmdQIOR      = 0xeb,

        // program flash array
        cmdPP        = 0x02,
        cmdQPP       = 0x32,

        // erase flash array
        cmdP4E       = 0x20,
        cmdSE        = 0xd8,
        cmdBE        = 0x60
    };
    
    enum TBufSize
    {
        TXBUF_SIZE = 64,
        RXBUF_SIZE = 1024
    };
    
public:
    void read_id();
    void read_sr();
    void read_cr();
    void wren();
    void wrr();
    void read();
    
private:
    void fill_tx_fifo  (const uint32_t count, const uint32_t pattern = 0);
    void write_tx_fifo (const uint32_t *data, const uint32_t count);
    void read_rx_fifo  (uint32_t       *dst,  const uint32_t count);
    
private:
    volatile  uint32_t CfgReg;     // "cache" access to QSPI_CONFIG_REG
    uint32_t  CmdIndex;
    uint32_t  Address;
    uint32_t *Dest;
    uint32_t  Count;
    uint32_t  Response;            // service variable, contains result of the transaction
    bool      Launch;
};
//------------------------------------------------------------------------------

#endif  // Z7INT_H


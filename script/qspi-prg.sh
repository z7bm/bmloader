#!/bin/sh

$CAD/xilinx/SDx/2016.3/SDK/bin/program_flash -f $1 -offset $2 -flash_type qspi_single -blank_check -verify -cable type xilinx_tcf url TCP:127.0.0.1:3121
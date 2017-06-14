proc rcr {} {
    set res [mrd 0xe000d000]
    return [lindex $res 1]
}

proc wcr {x} {
    mwr 0xe000d000 $x
}

#proc start {} {
#    set cr 0x[rcr]
#    #puts $cr
#    set cr 0x[format %x [expr $cr | 0x00010000]]
#    #puts $cr
#    mwr 0xe000d000 $cr
#}
#
#proc read_id { } {
#    # TXD0: 4 bytes
#    # TXD2: 2 bytes
#    mwr 0xe000d01c 0x00000090   
#    mwr 0xe000d084 0x00000000   
#    
#    start
#    
#}

#-------------------------------------------------------------------------------
proc rdvar {name {n 1}} {
    set res [lindex [print &$name] 2]
    mrd $res $n
}
#-------------------------------------------------------------------------------
proc wrvar {name data} {
    set res [lindex [print &$name] 2]
    mwr $res $data
}
#-------------------------------------------------------------------------------
proc rdres { {N 32} } {
    puts [ rdvar QSpi.Response ]
    set qspi_buf [scan [lindex [rdvar QSpi.Buf] 1] %x]
    mrd $qspi_buf $N
}
#-------------------------------------------------------------------------------
# wait end-of-transaction
proc wait_eot {} {     
    while {1} {
        set res [scan [lindex [rdvar QSpi.Launch] 1] %x]
        if {$res == 0} {
            break
        }
    }
}
#-------------------------------------------------------------------------------
proc lnch {} {
    wrvar QSpi.Launch 1
    wait_eot
}
#-------------------------------------------------------------------------------
proc read {addr n} {
    wrvar QSpi.CmdIndex 3
    wrvar QSpi.Address  $addr
    wrvar QSpi.Count    $n
    lnch
}
#-------------------------------------------------------------------------------
proc read_id {} {
    wrvar QSpi.CmdIndex 0
    lnch
    set res [scan [lindex [rdvar QSpi.Response] 1] %x]
    puts "Manufacturer ID:  [format %02x [expr ($res & 0xff)] ]"
    puts "Device ID      :  [format %02x [expr ($res & 0xff00) >> 8 ] ]"
}
#-------------------------------------------------------------------------------
proc read_sr {} {
    wrvar QSpi.CmdIndex 1
    lnch
    set response [scan [lindex [rdvar QSpi.Response] 1] %x]
    set res [format %02x $response]
    return [puts "SR: $res"]
}
#-------------------------------------------------------------------------------
proc read_cr {} {
    wrvar QSpi.CmdIndex 2
    lnch
    set response [scan [lindex [rdvar QSpi.Response] 1] %x]
    set res [format %02x $response]
    return [puts "CR: $res"]
}
#-------------------------------------------------------------------------------
proc wren {} {
    wrvar QSpi.CmdIndex 4
    lnch
    read_sr
}
#-------------------------------------------------------------------------------
proc wrr {sr cr} {
    wrvar QSpi.CmdIndex 5
    set qspi_buf [scan [lindex [rdvar QSpi.Buf] 1] %x]
    mwr $qspi_buf [expr $sr + ($cr << 8)]
    lnch
    read_sr
    read_cr
}
#-------------------------------------------------------------------------------
proc serase {addr} {
    wrvar QSpi.CmdIndex 6
    wrvar QSpi.Address  $addr
    lnch
}
#-------------------------------------------------------------------------------


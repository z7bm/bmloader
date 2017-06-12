proc rcr {} {
    set res [mrd 0xe000d000]
    return [lindex $res 1]
}

proc wcr {x} {
    mwr 0xe000d000 $x
}

proc start {} {
    set cr 0x[rcr]
    #puts $cr
    set cr 0x[format %x [expr $cr | 0x00010000]]
    #puts $cr
    mwr 0xe000d000 $cr
}

proc read_id { } {
    # TXD0: 4 bytes
    # TXD2: 2 bytes
    mwr 0xe000d01c 0x00000090   
    mwr 0xe000d084 0x00000000   
    
    start
    
}

proc rdvar {name {n 1}} {
    set res [lindex [print &$name] 2]
    mrd $res $n
}

proc wrvar {name data} {
    set res [lindex [print &$name] 2]
    mwr $res $data
}

proc rdres { {N 32} } {
    puts [ rdvar QSpi.RxIndex ]
    rdvar QSpi.RxBuf $N
}

proc lnch {} {
    wrvar QSpi.Launch 1
}


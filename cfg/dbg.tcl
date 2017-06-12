proc cn { } {
    connect -host localhost -port 3121
    targets -set -filter {name =~ "ARM* #0"}
}

proc rl { } {
    rst
    dow bin/fsbl.elf
    con 
}

proc rlc { } {
    rl
    con 
}

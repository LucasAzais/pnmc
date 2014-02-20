#!/bin/sh
# \
exec tclsh "$0" -- "$@"

# usage mkscc n

proc main {} {
    global argv
    if {"" == [set n [lindex $argv 1]]} {set n 1}
    puts "net \{scc $n\}"
    for {set i 0} {$i < $n} {incr i} {
	puts "tr t1_$i a -> d_$i"
	puts "tr t2_$i c d_$i e_$i -> d_$i b"
	puts "tr t3_$i d_$i b -> c a e_$i"
	puts "pl e_$i (1)"
    }
    puts "pl b (1)"
    puts "pl a (1)"
}

main

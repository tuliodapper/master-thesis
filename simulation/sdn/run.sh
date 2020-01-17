#!/bin/shx
M3WSN='$rootM3WSN/M3WSN'
M3WSNSrc='$rootM3WSN/src'
M3WSNSsim='$sim'
castalia='$rootcastalia-m3wsn/castalia'
castaliaSrc='$rootcastalia-m3wsn/src'
cd $M3WSNSsim
ini=$(pwd)'/sim-config.ini'
cmd=$M3WSN' -r 0 -u Cmdenv -c General -n '$M3WSNSrc':'$castaliaSrc' -l '$castalia' '$ini
$cmd > output.txt

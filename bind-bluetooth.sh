#!/bin/bash

if [ $# -eq 0 ]
then
    echo Usage: sudo $0 \<terminal-name\>
    echo This script must be execute with elevated privileges
    echo
    echo Example: sudo $0 PRO-12345678
    exit
fi

if [ $(id -u) != 0 ]
then
    echo Need elevated permission
    exit
fi

rfcommDevice="rfcomm0"

terminalName=$1
echo Searching for terminal $terminalName

macAddress="$(hcitool scan | grep $terminalName | grep -o "\(\([a-fA-F0-9]\{2\}:\?\)\{6\}\)")"

if [[ -z "$macAddress" ]]
then
    echo Device not found
    exit
fi

echo MAC address found: $macAddress

echo Releasing previous $rfcommDevice
rfcomm release $rfcommDevice

echo Binding $macAddress to $rfcommDevice
rfcommOutput="$(rfcomm bind $rfcommDevice $macAddress 1 2>&1)"

if [[ -z "$rfcommOutput" ]]
then
    echo Bind complete
else
    echo $rfcommOutput
fi


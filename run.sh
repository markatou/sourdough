#!/bin/bash

./autogen.sh && ./configure && make

sudo sysctl -w net.ipv4.ip_forward=1

cd datagrump

./run-contest blueberry


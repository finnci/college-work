#!/bin/sh

port=$1

if [ -z "$port" ]
then 
port=8000 
fi

ruby serverrun.rb $port

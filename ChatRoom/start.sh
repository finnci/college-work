#!/bin/sh

port=$1

if [ -z "$port" ]
then 
port=8000 
fi

ruby server.rb $port

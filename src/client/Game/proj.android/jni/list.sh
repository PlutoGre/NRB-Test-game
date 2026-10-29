#!/bin/bash

append_str=' \'

list_alldir()
{
    for file in $1/*
    do
        if [ -f $file ]; then
            echo $file$append_str | grep .cpp
            echo $file$append_str | grep .cc
        fi
        
        if [ -d $file ]; then
            list_alldir $file            
        fi
    done     
}

if [ $# -gt 0 ]; then
    list_alldir "$1"
else
	list_alldir "."
	list_alldir "../../Platform"
    list_alldir "../../Classes"
    list_alldir "../../Protobuf"
fi

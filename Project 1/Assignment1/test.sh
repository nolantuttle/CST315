#!/bin/bash

awk 'BEGIN {print "Files in directory: " } { print FILENAME} END {print "Current directory: "}' ./*

pwd

echo "Info about directory: "
pwd | xargs GetFileInfo 

echo "Current user browsing this directory: "
whoami

echo "Current shell being used: " 
echo $SHELL

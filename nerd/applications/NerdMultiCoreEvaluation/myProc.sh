#!/bin/bash


echo Start $1
seconds=$(( (RANDOM%10+1)*100000 ))
#sleep $seconds

x=1
while [ $x -le $seconds ]
do
x=$(( $x + 1 ))
done

echo Done $1

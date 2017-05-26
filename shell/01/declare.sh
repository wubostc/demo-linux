#!/bin/bash

movice[0]=a
movice[1]=b
declare -a movice[2]=c

echo ${movice[*]}

# [declare -x] == [export]

declare -i s=2+2

echo $s


s2=$(expr $s + 10)

echo "$s2"

s3=$(($s + $s2))
echo $s3


s10=$((3+10/(10%3)))

echo ${s10}

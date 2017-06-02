#!/bin/bash - 
#===============================================================================
#
#          FILE: bc2.sh
# 
#         USAGE: ./bc2.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2017年06月02日 15:14
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

var1=10
var2=23.54
var3=13.55
var4=20

var5=$(bc << EOF
scale=4
a1=($var1*$var2)
b1=($var3*$var4)
a1 + b1
EOF
)

echo var5 is $var5

#!/bin/bash - 
#===============================================================================
#
#          FILE: expr.sh
# 
#         USAGE: ./expr.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2017年06月02日 14:54
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

var1=$[1+4]
echo $var1

var2=$[$var1*2]
echo $var2

var3=40
var4=10
var5=$[$var3 * ($var4 - $var1)]
echo "result is $var5"


var10=10
var11=3
echo "only support int: $[$var10 / $var11]"

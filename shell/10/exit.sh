#!/bin/bash - 
#===============================================================================
#
#          FILE: exit.sh
# 
#         USAGE: ./exit.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2017年06月02日 15:24
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

var1=10
var2=30
var3=$[$var1+ $var2]
echo var3 is $var3

exit 5
# echo $?


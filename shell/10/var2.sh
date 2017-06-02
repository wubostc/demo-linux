#!/bin/bash - 
#===============================================================================
#
#          FILE: var2.sh
# 
#         USAGE: ./var2.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2017年06月02日 11:56
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

value1=10
value2=$value1
echo The resulting value is $value2

value2=value1
echo The resulting value is $value2

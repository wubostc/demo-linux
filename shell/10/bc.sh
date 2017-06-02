#!/bin/bash - 
#===============================================================================
#
#          FILE: bc.sh
# 
#         USAGE: ./bc.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2017年06月02日 15:09
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

var1=$(echo "scale=4; 25/4" | bc)
echo The answer is $var1


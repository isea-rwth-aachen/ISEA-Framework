#!/bin/bash
array=( $@ )
len=${#array[@]}
lastElement=${array[$len-1]}
allArgumentsButTheLast=${array[@]:0:$len-1}
sloccount --duplicates --wide --details $allArgumentsButTheLast > "$lastElement/sloccount.sc"

#!/usr/bin/bash

# Get filename parts
fn=$(basename $1)
dn=$(dirname $1)
sn=$(basename ${dn})

sz1=$(stat -c "%s" $2/${sn}.laz)
sz2=$(stat -c "%s" $2/${sn}.spoc.z)
ratio=$(bc <<< "scale=3; 100.0*$sz1/$sz2")
printf "%s\tlaz/spoc\t%d\t%d\t%.01f\n" $sn $sz1 $sz2 $ratio

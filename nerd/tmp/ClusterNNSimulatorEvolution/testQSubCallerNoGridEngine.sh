#!/bin/sh

#script to sequentially evaluate all individuals/groups in cluster-mode. Starts the jobscript $3 with each of the indizes between $1-$2.
count=1


i=$1
while [ $i -le $2 ]
do
 /bin/sh $3 $i
  i=`expr $i + 1`
done
exit 0

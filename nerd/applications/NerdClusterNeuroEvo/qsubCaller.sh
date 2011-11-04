#!/bin/sh

START_INDEX=$1;
END_INDEX=$2;
JOBFILE=$3;
JOBNAME=$4;
#DIRECTORY=$5;
DIRECTORY=/dev/null

#QSub-options: 
# -q nkg: chose nkg-queue as queue, where the jobs will be submitted.
# -r y: resubmit jobs that terminated without returning an exit code.
# -sync y: submit the job "synchronized" -> execution blocks until the job is terminated.
# -l arch=\"lx24-amd64\": ensure, that jobs or tasks of an array job are only sent to 64bit machines.
# -cwd: use current directory as working directory.
QSUB_OPTIONS="-q nkg -r y -sync y -l hostname=gene*&!(gene50) -l arch=lx26-amd64 "
GROUP_NAME=nkg;


if  [ $START_INDEX -eq $END_INDEX ]
then
#Start a normal job, if there is only one evaluation to be done.
qsub $QSUB_OPTIONS -e $DIRECTORY -o $DIRECTORY -N $JOBNAME $JOBFILE $START_INDEX

else
# Start an array job.
qsub $QSUB_OPTIONS -e $DIRECTORY -o $DIRECTORY -N $JOBNAME -t $START_INDEX-$END_INDEX:1 $JOBFILE

fi
exit 0

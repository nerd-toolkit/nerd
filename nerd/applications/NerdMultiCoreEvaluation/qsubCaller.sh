#!/bin/sh

MULTICORE_EVALUATION_APPLICATION=./NerdMultiCoreEvaluation
START_INDEX=$1;
END_INDEX=$2;
JOBFILE=$3;
JOBNAME=$4;
DIRECTORY=$(pwd)/multiCoreConfig

if  [ $START_INDEX -eq $END_INDEX ]
then
#Start a normal job, if there is only one evaluation to be done.
#qsub $QSUB_OPTIONS -e $DIRECTORY -o $DIRECTORY -N $JOBNAME $JOBFILE $START_INDEX
${MULTICORE_EVALUATION_APPLICATION} --configDir $DIRECTORY --name $JOBNAME --scriptFile $JOBFILE --index $START_INDEX

else
# Start an array job.
#qsub $QSUB_OPTIONS -e $DIRECTORY -o $DIRECTORY -N $JOBNAME -t $START_INDEX-$END_INDEX:1 $JOBFILE
${MULTICORE_EVALUATION_APPLICATION} --configDir $DIRECTORY --name $JOBNAME --scriptFile $JOBFILE --index $START_INDEX $END_INDEX

fi
exit 0

#!/bin/bash

BIN_DIR=$1
CURRENT_DIR=$(pwd)


if [ $# -eq 0 ]
then
BIN_DIR=$CURRENT_DIR
fi

if [ $# -eq 1 ]
then
cd ${BIN_DIR}
BIN_DIR=$(pwd)
cd ${CURRENT_DIR}
fi

echo
echo "NERD TEST SUITE"
echo "---------------"
echo

./testRunner.sh ${BIN_DIR} > testLog.txt





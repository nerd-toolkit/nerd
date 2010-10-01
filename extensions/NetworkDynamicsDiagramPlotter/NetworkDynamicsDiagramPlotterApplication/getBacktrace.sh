#!/bin/bash

    gdb $1 \
        --core $2 \
        --batch \
        --quiet \
        -ex "thread apply all bt full" \
        -ex "quit" \
    > $2_backtrace.txt

	


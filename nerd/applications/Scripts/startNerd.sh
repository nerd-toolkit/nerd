

#Execute NERD application
ulimit -c unlimited

$*

#This segment creates a backtrace of a core dump when the application crashes.
#If GDB is not installed, it tries to install it using apt-get.
if [ -f core ]
then
	echo 
	echo "There was an error during execution."
	echo "Creating backtrace file from dumped core."
    echo "Please send the backtrace file(s) to practice%neurokyb@gmx.de"
	echo 
    echo "Please wait..."
    echo
    gdb -ex "quit" &> /dev/null
	if [ $? -ne 0 ]
    then
       echo "GDB could not be found on your computer. Trying to install with apt-get..."
       echo "sudo apt-get install gdb"
       sudo apt-get install gdb
    fi

    TIME_NOW=$(date +"%y-%m-%d_%H-%M-%S")
    BACKTRACE_FILENAME="backtrace_${TIME_NOW}.txt"
	echo "Executed Program: ${CMD}" > ${BACKTRACE_FILENAME}
	echo >> ${BACKTRACE_FILENAME}
    gdb ${NERD} --core core --batch --quiet \
        -ex "thread apply all bt full" -ex "quit" \
        >> ${BACKTRACE_FILENAME}
	rm core;
	echo
	echo "Created backtrace file ${BACKTRACE_FILENAME}"
	echo
fi


echo Done

exit

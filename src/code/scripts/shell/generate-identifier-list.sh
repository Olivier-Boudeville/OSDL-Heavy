#!/bin/sh

USAGE=`basename $0`" <identifier text file>: generates from specified text file X.txt containing an identifier per line (ex: 'HelloWorld' in line 27) a X.sh file ready to be sourced by shell scripts containing numerical identifiers corresponding to the line of the identifier (ex: 'HelloWorld=27')"


SOURCE_FILE="$1"

if [ -z "${SOURCE_FILE}" ]; then
	echo "
	Error, no source identifier file specified. 
	Usage: $USAGE." 1>&2
	exit 1
fi


if [ ! -f "${SOURCE_FILE}" ]; then
	echo "
	Error, specified source identifier file (${SOURCE_FILE}) is not an existing file. 
	Usage: $USAGE." 1>&2
	exit 2
fi


if [ -z "$2" ] ; then
	# Choose the name of produced file:
	TARGET_FILE=`echo ${SOURCE_FILE}|sed 's|.txt$|.id|1'`
else
	# Use specified one:
	TARGET_FILE=$2
fi

#echo "TARGET_FILE = ${TARGET_FILE}"

# This appends to each line '=' and the number of that line:
cat ${SOURCE_FILE} | sort | uniq | sed = | sed 'N;s|\n| = |' | awk '{print $3$2$1}' > ${TARGET_FILE} && echo "    File ${TARGET_FILE} successfully generated."


# The formatting was very readable but not shell-friendly:
#cat ${SOURCE_FILE} | sort | uniq | sed = | sed 'N;s|\n| = |' | awk '{printf "%-30s =  %s\n", $3, $1}' > ${TARGET_FILE} && echo "
#	File ${TARGET_FILE} successfully generated."
	
	

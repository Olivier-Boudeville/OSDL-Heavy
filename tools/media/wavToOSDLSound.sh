#!/bin/sh

THIS_SCRIPT=`basename $0`

USAGE="${THIS_SCRIPT} [-h|--help] SOURCE_WAVE_FILE TARGET_OSDL_SOUND
Converts specified wave file into an OSDL sound counterpart.
Example: ${THIS_SCRIPT} hello.wav hello.osdl.sound uses hello.wav to generate its hello.osdl.sound counterpart."


if [ -x "sox" ] ; then

	echo "Error, 'sox' tool not found." 1>&2
	exit 1

fi

CONVERTER=rawToOSDLSound.exe

if [ ! -x "${CONVERTER}" ] ; then

	echo "Error, no raw to OSDL sound converter (${CONVERTER}) found." 1>&2
	exit 2 

fi

CONVERTER_ARGS=""

SOURCE_WAVE_FILE=$1
TARGET_OSDL_SOUND=$2

if [ $# -le 1 ] ; then

	if [ "$1" = "-h" ] || [ "$1" = "--help" ] ; then
		echo "$USAGE"
		exit
	else
		echo "Error, not enough parameters.\n$USAGE" 1>&2
		exit 3
	fi
	
fi


if [ ! -e "${SOURCE_WAVE_FILE}" ] ; then

	echo "Error, source wave file (${SOURCE_WAVE_FILE}) not found.\n$USAGE" 1>&2
	exit 4
	
fi


SOURCE_FILE_PREFIX=`echo ${SOURCE_WAVE_FILE} | sed 's|\..*||1'`
#echo "SOURCE_FILE_PREFIX = ${SOURCE_FILE_PREFIX}"

TMP_FILE="${SOURCE_FILE_PREFIX}.raw"

SOX_MSG=`sox -V ${SOURCE_WAVE_FILE} ${TMP_FILE} 2>&1`


#echo "SOX_RES=${SOX_RES}"

SOX_RES=$?

if [ ! ${SOX_RES} -eq 0 ] ; then

	echo "Error, sox failed with error code ${SOX_RES} and output: ${SOX_MSG}." 1>&2
	exit 5

fi


SOX_MSG_INPUT=`echo "${SOX_MSG}"|head -n 8`
#echo "${SOX_MSG_INPUT}"


SAMPLE_RATE=`echo "${SOX_MSG_INPUT}"|grep 'Sample Rate'|awk '{print $4}'`
#echo "SAMPLE_RATE = ${SAMPLE_RATE}"

CONVERTER_ARGS="${CONVERTER_ARGS} -f ${SAMPLE_RATE}" 

CHANNELS=`echo "${SOX_MSG_INPUT}"|grep 'Channels'|awk '{print $3}'`
#echo "CHANNELS = ${CHANNELS}"

if [ "${CHANNELS}" = "1" ]; then 
	CONVERTER_ARGS="${CONVERTER_ARGS} -m mono" 
else
	echo "Error, unsupported channel number (${CHANNELS})."	1>&2
	exit 6
fi


SAMPLE_SIZE=`echo "${SOX_MSG_INPUT}"|grep 'Sample Size'|awk '{print $4}'`
#echo "SAMPLE_SIZE = ${SAMPLE_SIZE}"

if [ "${SAMPLE_SIZE}" = "16-bit" ]; then
	CONVERTER_ARGS="${CONVERTER_ARGS} -b 16" 
else
	echo "Error, unsupported sample size (${SAMPLE_SIZE})."	1>&2
	exit 7
fi


COMMAND="${CONVERTER} ${CONVERTER_ARGS} ${TMP_FILE}"
#echo "COMMAND = ${COMMAND}"

CONVERTER_MSG=`${COMMAND} 2>&1`

CONVERTER_RES=$?

if [ ! ${CONVERTER_RES} -eq 0 ] ; then

	echo "Error, converter (${CONVERTER}) failed with error code ${CONVERTER_RES} and output: ${CONVERTER_MSG}." 1>&2
	exit 8

fi

rm -f ${TMP_FILE} rawToOSDLSound.exe.log

TARGET_FILE="${SOURCE_FILE_PREFIX}.osdl.sound"
echo "    ${TARGET_FILE} produced, ready to be used !"
ls -l ${SOURCE_WAVE_FILE} ${TARGET_FILE}


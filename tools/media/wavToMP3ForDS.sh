#!/bin/sh

THIS_SCRIPT=`basename $0`

USAGE="Usage: ${THIS_SCRIPT} [-h|--help] SOURCE_WAVE_FILE
Converts specified wave file into a mp3 file appropriate for playback on the DS.
  Example: ${THIS_SCRIPT} hello.wav uses hello.wav to generate its hello.mp3 counterpart (needs the lame encoder).
    -h/--help: displays this help
  One may use the audacity tool to preprocess the wave sound beforehand (cleaning, volume adjustment, correct export in format, etc.)
  Running this script is often the first step of a process: once having a mp3, one usually plays it on the Nintendo DS thanks to the getMP3Settings tools (copy the generated mp3 file to the root of your removable DS card, under the name 'test.mp3'), which will return an upper bound to the size of encoded frames for this mp3.
  Then this value can be used with the mp3ToOSDLMusic.exe tool (using the -u parameter) to finally produce an OSDL music file ready to be played back by the OSDL-Helix engine on the DS, with reduced resource needs.
"


LOG_FILE=${THIS_SCRIPT}.log

ENCODER=`which lame`

if [ ! -x "${ENCODER}" ] ; then

	echo "Error, no mp3 encoder ('lame') not found." 1>&2
	exit 1

fi


if [ "$1" = "-h" ] || [ "$1" = "--help" ] ; then
	echo "$USAGE"
	exit
fi

SOURCE_WAVE_FILE=$1


if [ ! $# -eq 1 ] ; then

	echo "Error, wrong number of parameters.\n$USAGE" 1>&2
	exit 3
	
fi


if [ ! -e "${SOURCE_WAVE_FILE}" ] ; then

	echo "Error, source wave file (${SOURCE_WAVE_FILE}) not found.\n$USAGE" 1>&2
	exit 4
	
fi


SOURCE_FILE_PREFIX=`echo ${SOURCE_WAVE_FILE} | sed 's|\..*||1'`
#echo "SOURCE_FILE_PREFIX = ${SOURCE_FILE_PREFIX}"


TARGET_FILE=${SOURCE_FILE_PREFIX}.mp3
	
echo "    Encoding ${SOURCE_WAVE_FILE} into ${TARGET_FILE} for DS playback."


CHANNEL_MODE=m
TARGET_QUALITY=5
QUALITY_OF_USED_ALGORITHM=0	
TARGET_SAMPLE_FREQUENCY=22.05

# Implies reduced mp3 frame upper-bound:
MAX_BITRATE=96

COMMAND="${ENCODER} ${SOURCE_WAVE_FILE} --verbose -m ${CHANNEL_MODE} --vbr-new -V ${TARGET_QUALITY} -q ${QUALITY_OF_USED_ALGORITHM} -B ${MAX_BITRATE} -t --resample ${TARGET_SAMPLE_FREQUENCY} ${TARGET_FILE}"

#echo "Command: ${COMMAND}"

	
#ENCODER_MSG=`${COMMAND} 2>&1`
#echo "Encoder output: ${ENCODER_MSG}"
${COMMAND}
	
ENCODER_RES=$?
	
if [ ! $ENCODER_RES -eq 0 ] ; then

	echo "Error, encoder (${ENCODER}) failed with error code ${ENCODER_RES} and output: ${ENCODER_MSG}." 1>&2
	exit 10

fi

echo "    ${TARGET_FILE} produced, ready to be used !"
ls -l ${SOURCE_WAVE_FILE} ${TARGET_FILE}


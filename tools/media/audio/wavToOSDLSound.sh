#!/bin/sh

THIS_SCRIPT=`basename $0`

USAGE="Usage: ${THIS_SCRIPT} [-h|--help] [-i|--ima-adpcm] SOURCE_WAVE_FILE
Converts specified wave file into an OSDL sound counterpart.
  Example: ${THIS_SCRIPT} hello.wav uses hello.wav to generate its hello.osdl.sound counterpart (needs sox and wavToOSDLSound.exe).
    -h/--help: displays this help
    -i/--ima-adpcm: encode the wave samples into IMA-ADPCM (about four times smaller but with poorer quality) [uses ffmpeg]
  Note that the wave file can contain usual PCM samples or IMA ADPCM samples: both will be managed automatically by this tool and by the OSDL player on the Nintendo DS.
  To generate an IMA ADPCM-encoded wave file, one should better use ffmpeg or audacity than sox, as the data produced by the latter is incorrectly decoded by the DS.
"


LOG_FILE=${THIS_SCRIPT}.log


if [ -x "sox" ] ; then

	echo "Error, 'sox' tool not found." 1>&2
	exit 1

fi

CONVERTER="wavToOSDLSound.exe"

if [ ! -x "${CONVERTER}" ] ; then

	echo "Error, no wave to OSDL sound converter (${CONVERTER}) found." 1>&2
	exit 2 

fi

CONVERTER_ARGS=""

ENCODER=`which ffmpeg`


if [ "$1" = "-h" ] || [ "$1" = "--help" ] ; then
	echo "$USAGE"
	exit
fi

do_encode=1
if [ "$1" = "-i" ] || [ "$1" = "--ima-adpcm" ] ; then
	do_encode=0
	shift
fi

SOURCE_WAVE_FILE=$1

INITIAL_WAVE_FILE=${SOURCE_WAVE_FILE}

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


if [ $do_encode -eq 0 ] ; then

	if [ ! -x "${ENCODER}" ] ; then
		echo "Error, IMA-ADPCM encoder (${ENCODER}) not found." 1>&2
		exit 5
	fi
	
	NEW_SOURCE_FILE=${SOURCE_FILE_PREFIX}-tmp.wav
	
	echo "    Encoding ${SOURCE_WAVE_FILE} to IMA-ADPCM"
	
	ENCODER_MSG=`${ENCODER} -y -i ${SOURCE_WAVE_FILE} -acodec adpcm_ima_wav ${NEW_SOURCE_FILE} 2>&1`
	
	ENCODER_RES=$?
	
	if [ ! $ENCODER_RES -eq 0 ] ; then

		echo "Error, encoder (${ENCODER}) failed with error code ${ENCODER_RES} and output: ${ENCODER_MSG}." 1>&2
		exit 10

	fi
			 
	SOURCE_WAVE_FILE=${NEW_SOURCE_FILE}
	
fi


# Volume could be increased here:
# (just requesting the stats here)
SOX_MSG=`sox -V ${SOURCE_WAVE_FILE} -n 2>&1`

# Sometimes sox fails but returns ok (0)...
SOX_RES=$?
#echo "SOX_RES=${SOX_RES}"

if [ ! ${SOX_RES} -eq 0 ] ; then

	echo "Error, sox failed with error code ${SOX_RES} and output: ${SOX_MSG}." 1>&2
	exit 6

fi

echo "${SOX_MSG}" > ${LOG_FILE}

SOX_MSG_INPUT=`echo "${SOX_MSG}"|head -n 8`
#echo "SOX_MSG_INPUT = ${SOX_MSG_INPUT}"


SAMPLE_RATE=`echo "${SOX_MSG_INPUT}"|grep 'Sample Rate'|awk '{print $4}'`
#echo "SAMPLE_RATE = ${SAMPLE_RATE}"

CONVERTER_ARGS="${CONVERTER_ARGS} -f ${SAMPLE_RATE}" 

CHANNELS=`echo "${SOX_MSG_INPUT}"|grep 'Channels'|awk '{print $3}'`
#echo "CHANNELS = ${CHANNELS}"

if [ "${CHANNELS}" = "1" ]; then 
	CONVERTER_ARGS="${CONVERTER_ARGS} -m mono" 
else
	echo "Error, unsupported channel number (${CHANNELS})."	1>&2
	exit 7
fi


SAMPLE_SIZE=`echo "${SOX_MSG_INPUT}"|grep 'Sample Size'|awk '{print $4}'`
#echo "SAMPLE_SIZE = ${SAMPLE_SIZE}"

if [ "${SAMPLE_SIZE}" = "16-bit" ]; then
	OSDL_SAMPLE_SIZE=16
else
	echo "Error, unsupported sample size (${SAMPLE_SIZE})."	1>&2
	exit 8
fi


SAMPLE_ENCODING=`echo "${SOX_MSG_INPUT}"|grep 'Sample Encoding'|awk '{print $3}'`
#echo "SAMPLE_ENCODING = ${SAMPLE_ENCODING}"

if [ "${SAMPLE_ENCODING}" = "IMA-ADPCM" ]; then
	# By convention:
	OSDL_SAMPLE_SIZE=4
fi


CONVERTER_ARGS="${CONVERTER_ARGS} -b ${OSDL_SAMPLE_SIZE}" 


# Here we have retrieved the sound settings, now removing the wav header from
# the sox-generated IMA_ADPCM file:


COMMAND="${CONVERTER} ${CONVERTER_ARGS} ${SOURCE_WAVE_FILE}"
#echo "COMMAND = ${COMMAND}"

CONVERTER_MSG=`${COMMAND} 2>&1`
CONVERTER_RES=$?

#echo "CONVERTER_MSG = ${CONVERTER_MSG}"

if [ ! ${CONVERTER_RES} -eq 0 ] ; then

	echo "Error, converter (${CONVERTER}) failed with error code ${CONVERTER_RES} and output: ${CONVERTER_MSG}." 1>&2
	exit 9

fi

rm -f wavToOSDLSound.exe.log

if [ $do_encode -eq 0 ]; then

	mv ${SOURCE_FILE_PREFIX}-tmp.osdl.sound ${SOURCE_FILE_PREFIX}.osdl.sound 
	rm ${SOURCE_FILE_PREFIX}-tmp.wav
fi



TARGET_FILE="${SOURCE_FILE_PREFIX}.osdl.sound"
echo "    ${TARGET_FILE} produced, ready to be used !"
ls -l ${INITIAL_WAVE_FILE} ${TARGET_FILE}


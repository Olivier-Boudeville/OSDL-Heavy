#!/bin/sh

USAGE=`basename $0`' <tileset archive> : processes a bitmap archive (ex : T_swordstan_shield.zip) from the splendid animated tilesets coming from Reiner "Tiles" Prokein (http://reinerstileset.4players.de) to convert them to an OAM archive'

do_preserve_content=1
do_uncompress=0
do_correct_name=0
do_convert_to_source_png=0
# Conversion to OSDL Animation Format :
do_convert_to_oaf=1
do_zip_result=0

# Anticipated checkings :

# Later we will scan the full web content mirror for tileset archive and
# convert them on the fly :
#TILESET_ROOT="$1"

#if [ -z "${TILESET_ROOT}" ] ; then
#	echo "
#	Error, no tileset root specified. 
#	Usage : $USAGE." 1>&2
#	exit 1
#fi

#if [ ! -d "${TILESET_ROOT}" ] ; then
#	echo "
#	Error, specified tileset root (${TILESET_ROOT}) is not an existing directory. 
#	Usage : $USAGE." 1>&2
#	exit 2
#fi

TILESET_ARCHIVE="$1"

if [ -z "${TILESET_ARCHIVE}" ] ; then
	echo "
	Error, no tileset archive specified. 
	Usage : $USAGE." 1>&2
	exit 1
fi

if [ ! -f "${TILESET_ARCHIVE}" ] ; then
	echo "
	Error, specified tileset archive (${TILESET_ARCHIVE}) is not an existing file. 
	Usage : $USAGE." 1>&2
	exit 2
fi

STARTING_DIR=`pwd`

# Will guess it this script is run from its location in OSDL build tree
# (trunk/src/code/scripts/shell) :
GUESSED_CEYLAN_ROOT=`echo ${STARTING_DIR} | sed 's|LOANI-repository.*$|LOANI-repository/ceylan/Ceylan/trunk/src/code/scripts/shell|1'`

#echo "GUESSED_CEYLAN_ROOT = ${GUESSED_CEYLAN_ROOT}"

if [ ! -d "${GUESSED_CEYLAN_ROOT}" ] ; then
	echo "
	Error, specified tileset root (${TILESET_ROOT}) is not an existing directory. 
	Usage : $USAGE." 1>&2
	exit 20
fi

CORRECT_SCRIPT="${GUESSED_CEYLAN_ROOT}/correctFilename.sh"
#echo "CORRECT_SCRIPT = ${CORRECT_SCRIPT}"

if [ ! -x "${CORRECT_SCRIPT}" ] ; then
	echo "
	Error, unable to find prerequesite executable script (${CORRECT_SCRIPT})." 1>&2
	exit 21
fi


echo "Processing Reiner sprite tileset in ${TILESET_ARCHIVE}"

# If given .../OSDL-data/T_swordstan_shield.zip :

# ARCHIVE_NAME is T_swordstan_shield.zip :
ARCHIVE_NAME=`basename ${TILESET_ARCHIVE}`

# TILESET_DIR is T_swordstan_shield :
TILESET_DIR=`echo ${ARCHIVE_NAME} | sed 's|.zip$||1'`

if [ $do_preserve_content -eq 1 ] ; then
	if [ -d "${TILESET_DIR}" ] ; then
		rm -rf ${TILESET_DIR}
	fi
fi

CONVERT_TOOL=`which convert`
echo "CONVERT_TOOL = ${CONVERT_TOOL}"

if [ ! -x ${CONVERT_TOOL} ] ; then
	echo "
	Error, unable to find 'convert' tool from imagemagick package.
	Install it for example with 'apt-get install imagemagick'.
	" 1>&2
	exit 21
fi


mkdir -p ${TILESET_DIR}
cd ${TILESET_DIR}


# Perform the work :


if [ $do_uncompress -eq 0 ] ; then
	echo " + unzipping ${TILESET_ARCHIVE}"
	unzip ${TILESET_ARCHIVE}
fi


if [ $do_correct_name -eq 0 ] ; then
	echo " + correcting entry names in ${TILESET_DIR}"
	find . -type d -exec ${CORRECT_SCRIPT} '{}' ';' 2>/dev/null
	find . -type f -exec ${CORRECT_SCRIPT} '{}' ';'
fi


if [ $do_convert_to_source_png -eq 0 ] ; then
	echo " + converting BMP to PNG files in ${TILESET_DIR}"

	CONVERT_OPT="-quality 100 -strip"
	#CONVERT_OPT="-sharpen 0x.5 -antialias"
	
	for f in `find . -name '*.bmp'`; do
		target=`echo $f | sed 's|bmp$|png|`
		echo "+ converting $f and replacing it by $target"
		${CONVERT_TOOL} ${CONVERT_OPT} $f $target
		rm -f $f
	done
fi


if [ $do_convert_to_oam -eq 0 ] ; then

	echo " + converting source PNG to OSDL Animation Format"

fi


if [ $do_zip_result -eq 0 ] ; then
	TARGET_ARCHIVE_NAME=`echo $ARCHIVE_NAME | sed 's|^T_|OAF_|1'`
	echo " + zipping ${TILESET_ARCHIVE} to ${TARGET_ARCHIVE_NAME}"
	
	cd ..
	NEW_TILESET_DIR=`echo ${TILESET_DIR} | sed 's|^T_|OAF_|1'`
	
	if [ -d "${NEW_TILESET_DIR}" ] ; then
		rm -rf ${NEW_TILESET_DIR}
		mkdir ${NEW_TILESET_DIR}
	fi
	
	mv -f ${TILESET_DIR} ${NEW_TILESET_DIR}
	zip -9 -r ${TARGET_ARCHIVE_NAME} ${TILESET_DIR} ${NEW_TILESET_DIR}
	rm -rf ${NEW_TILESET_DIR}
fi


echo "End of processing."


#!/bin/sh

USAGE=`basename $0`"' <root of sorted Reiner repository>: processes (set in canonical form) a repository of Reiner "Tiles" Prokein tilesets (http://reinerstileset.4players.de) that has preferably already been sorted thanks to the 'sort-reiner-mirror.sh' script. Uses for each archive the 'process-reiner-archive.sh' script."

do_extract_archives=0
do_remove_original_archives=1

TILESET_ROOT="$1"

if [ -z "${TILESET_ROOT}" ] ; then
   echo "
   Error, no tileset root specified. 
   Usage: $USAGE." 1>&2
   exit 1
fi

if [ ! -d "${TILESET_ROOT}" ] ; then
   echo "
   Error, specified tileset root (${TILESET_ROOT}) is not an existing directory.    Usage: $USAGE." 1>&2
   exit 2
fi


STARTING_DIR=`pwd`

PER_ARCHIVE_SCRIPT_NAME="process-reiner-archive.sh"
PER_ARCHIVE_SCRIPT=`which ${PER_ARCHIVE_SCRIPT_NAME} 2>/dev/null`
#echo "PER_ARCHIVE_SCRIPT = ${PER_ARCHIVE_SCRIPT}"

if [ ! -x ${PER_ARCHIVE_SCRIPT} ] ; then
	echo "
	Error, unable to find the '${PER_ARCHIVE_SCRIPT_NAME}' script.
	" 1>&2
	exit 1
fi




echo "    Processing all Reiner sprite tileset archives in ${TILESET_ROOT}"


RM=/bin/rm


# Perform the work:

TARGET_ARCHIVES=`find ${TILESET_ROOT} -name 'T*.zip' -a -type f`

echo "TARGET_ARCHIVES = ${TARGET_ARCHIVES}"

for f in ${TARGET_ARCHIVES}; do

	if [ $do_extract_archives -eq 0 ] ; then
		echo "   + unzipping $f"
		#unzip $f
	fi

	if [ $do_remove_original_archives -eq 0 ] ; then
		echo "   + removing $f"
		#${RM} -f $f
	fi

done

echo "End of mirror processing."


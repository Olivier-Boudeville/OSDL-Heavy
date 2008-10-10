#!/bin/sh

USAGE="Usage: "`basename $0`" <archive name> <directory to archive>: this script will create an OSDL archive (*.oar) from specified directory.
Ex: `basename $0` myArchive.oar content-directory"

# See also:
#  - osdl/OSDL/trunk/src/code/scripts/shell/extract-OSDL-archive.sh
#  - osdl/OSDL/trunk/test/basic/create-testOSDLEmbeddedFileSystem-archive.sh

# Thus produced archive can be decompresssed with: 
# 7zr x myArchive.oar - or - extract-OSDL-archive.sh myArchive.oar

if [ ! $# -eq 2 ] ; then
	echo "Error, exactly two parameters required.
    " $USAGE 1>&2
    exit 10
fi

archive_target="$1"

archive_directory="$2"

if [ ! -d "${archive_directory}" ] ; then
	echo "Error, ${archive_directory} is not a directory.
    " $USAGE 1>&2
    exit 11
fi

#echo "Will generate ${archive_target} from directory ${archive_directory}."
	

# OAR archives are based on the Lempel-Ziv-Markov chain-Algorithm (LZMA).
# See: http://en.wikipedia.org/wiki/Lempel-Ziv-Markov_chain_algorithm
# On Debian-based distributions, use: 'apt-get install p7zip' to have the 
# archiver.
# The lzma package is not enough, as it compresses only files, not filesystem
# full trees.

ARCHIVER_NAME="7zr"
ARCHIVER=`which ${ARCHIVER_NAME}`

if [ ! -x "${ARCHIVER}" ] ; then

	echo "Error, no archiver available (${ARCHIVER_NAME})." 1>&2
    exit 1
    
fi

RM="/bin/rm -f"


if [ -e "${archive_target}" ] ; then
	${RM} "${archive_target}"
fi


LANG= ${ARCHIVER} a ${archive_target} ${archive_directory} 1>/dev/null

#LANG= ${ARCHIVER} a -t7z -mx=9 ${archive_target} ${archive_directory} 1>/dev/null

if [ $? -eq 0 ] ; then
	echo "OSDL Archive ${archive_target} successfully produced from directory ${archive_directory}:"
    /bin/ls -l ${archive_target}
fi


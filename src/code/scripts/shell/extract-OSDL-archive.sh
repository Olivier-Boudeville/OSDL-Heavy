#!/bin/sh

USAGE="Usage: "`basename $0`" <archive name>: this script will extract an OSDL archive (*.oar) to current directory.
Ex: `basename $0` myArchive.oar"

# See also osdl/OSDL/trunk/src/code/scripts/shell/create-OSDL-archive.sh.


if [ ! $# -eq 1 ] ; then
	echo "Error, exactly one parameter required.
    " $USAGE 1>&2
    exit 10
fi

archive_source="$1"

if [ ! -f "${archive_source}" ] ; then
	echo "Error, file ${archive_source} not found.
    " $USAGE 1>&2
    exit 11
fi

#echo "Will extract ${archive_source}."
	

# OAR archives are based on the Lempel-Ziv-Markov chain-Algorithm (LZMA).
# See: http://en.wikipedia.org/wiki/Lempel-Ziv-Markov_chain_algorithm
# On Debian-based distributions, use: 'apt-get install p7zip' to have the 
# archiver.
# The lzma package is not enough, as it compresses only files, not filesystem
# full trees.

DEARCHIVER_NAME="7zr"
ARCHIVER=`which ${DEARCHIVER_NAME}`

if [ ! -x "${ARCHIVER}" ] ; then

	echo "Error, no archiver available (${DEARCHIVER_NAME})." 1>&2
    exit 1
    
fi


LANG= ${ARCHIVER} x -y ${archive_source} 

if [ $? -eq 0 ] ; then
	echo "OSDL Archive ${archive_source} successfully extracted."
fi


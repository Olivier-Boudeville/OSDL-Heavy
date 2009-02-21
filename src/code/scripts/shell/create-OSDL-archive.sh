#!/bin/sh

USAGE="Usage: "`basename $0`" <archive name> <directory to archive>: this script will create an OSDL archive (*.oar) from specified directory. All files in that directory will be stored in a cyphered version in the archive, but nothing under the specified directory will be modified.
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

RM="/bin/rm -f"
FIND=`which find`
RSYNC=`which rsync`

if [ ! -x "${RSYNC}" ] ; then

	echo "Error, no rsync tool found." 1>&2
	exit 20
	
fi


# Usually in osdl/OSDL/trunk/tools/media:
cypher_tool="cypherOSDLFile.exe"
cypher_dir=`dirname $0`"/../../../../tools/media"

# Needs to find an absolute path:
cypher_exec=`PATH=$PWD/$cypher_dir:$PATH which $cypher_tool 2>/dev/null`

if [ ! -x "${cypher_exec}" ] ; then

	echo "Error, no executable cypher tool ${cypher_tool} found." 1>&2
    exit 12

fi

# zip might be used instead, for the purpose of testing/fixing LZMA (with 7zr):
#archiver_name="7zr"
archiver_name="zip"
archiver=`which ${archiver_name}`

if [ ! -x "${archiver}" ] ; then

	echo "Error, no archiver available (${archiver_name})." 1>&2
    exit 1
    
fi


#echo "Will generate ${archive_target} from directory ${archive_directory}."

tmp_base="tmp-create-OSDL-archive"


# Cyphering will change files, so we will operate on a copy made beforehand:

if [ -d "${tmp_base}" ] ; then

	echo "Error, temporary directory '${tmp_base}' already exists, remove it first if appropriate." 1>&2
    exit 13

fi


/bin/mkdir ${tmp_base}

# Would copy also the .svn directories:
#/bin/cp -rf "${archive_directory}" ${tmp_base}

# Would not be relevant as the target tree would be flatten:
#${FIND} "${archive_directory}" \( -name .svn -prune \) -o  -exec cp... ';' 

# --cvs-exclude removes also .svn directories:
${RSYNC} -r --cvs-exclude "${archive_directory}" ${tmp_base}

if [ ! $? -eq 0	] ; then

	echo "Error, copy to temporary directory '${tmp_base}' failed, removing it." 1>&2
    ${RM} -r "${tmp_base}"
    exit 13

fi

if [ -e "${archive_target}" ] ; then
	${RM} "${archive_target}"
fi

archive_directory_name=`basename ${archive_directory}`


# Now let's cypher files in the temporary directory:
cd "${tmp_base}/"
${FIND} ${archive_directory_name} -type f -exec ${cypher_exec} '{}' ';' | grep -v 'Logs can be inspected'

# OAR archives are based on the Lempel-Ziv-Markov chain-Algorithm (LZMA).
# See: http://en.wikipedia.org/wiki/Lempel-Ziv-Markov_chain_algorithm
# On Debian-based distributions, use: 'apt-get install p7zip' to have the 
# archiver.
# The lzma package is not enough, as it compresses only files, not filesystem
# full trees.



# For 7zr:
#LANG= ${archiver} a ../${archive_target} "${archive_directory_name}" 1>/dev/null

#LANG= ${archiver} a -t7z -mx=9 ../${archive_target} "${archive_directory_name}" 1>/dev/null

# For zip:
LANG= ${archiver} -r ../${archive_target} "${archive_directory_name}"

cd ..

if [ $? -eq 0 ] ; then
	echo "OSDL Archive ${archive_target} successfully produced from directory ${archive_directory}:"
	file ${archive_target}
	du -sh ${tmp_base}
    /bin/ls -l ${archive_target}
fi


${RM} -r ${tmp_base}


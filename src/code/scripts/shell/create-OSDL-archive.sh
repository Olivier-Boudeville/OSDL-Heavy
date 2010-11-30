#!/bin/sh

USAGE="Usage: "`basename $0`" <archive name> <directory to archive> [<resource map prefix>]: this script will create an OSDL archive (*.oar) from specified directory, i.e. with all its content but without this directory being included.

All files found from that directory will be stored in a cyphered version in the archive, but nothing under the specified directory will be modified: a copy of the source tree is made, on which the script operates.

The optional third parameter dictates what is the prefix that should be used to generate XML and header file for the resource map.

Ex: `basename $0` myArchive.oar content-directory [my-resource-map]"


# See also:
#  - osdl/OSDL/trunk/src/code/scripts/shell/extract-OSDL-archive.sh
#  - osdl/OSDL/trunk/test/basic/create-testOSDLEmbeddedFileSystem-archive.sh

# Thus produced archive can be decompresssed with:
# 7zr x myArchive.oar - or - extract-OSDL-archive.sh myArchive.oar

# The OSDL-environment.sh script is expected to have already been sourced.


# By default use LZMA, otherwise use ZIP:
use_lzma=0
#use_lzma=1


# Returns a cyphered version (currently rot13) of specified name.
# Source: http://www.miranda.org/~jkominek/rot13/sh/rot13-tr.sh
cypher_name()
{

	res=`echo $1 | tr A-Za-z N-ZA-Mn-za-m`

}



if [ $# -le 1 ] ; then

	echo "Error, at least two parameters required.
	" $USAGE 1>&2
	exit 10

fi

archive_target="$1"

archive_directory="$2"


# Default:
index_basename="OSDLResourceMap"

if [ -n "$3" ] ; then
	index_basename="$3"
fi


if [ ! -d "${archive_directory}" ] ; then

	echo "Error, ${archive_directory} is not a directory.
	" $USAGE 1>&2
	exit 11

fi


RM="/bin/rm -f"
FIND=`which find`
RSYNC=`which rsync`
CP="/bin/cp -f"


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

	echo "Error, no executable cypher tool ${cypher_tool} found (were the OSDL tools compiled?)." 1>&2
	exit 12

fi


# zip might be used instead, for the purpose of testing/fixing LZMA (with 7zr):

if [ $use_lzma -eq 0 ] ; then

	archiver_name="7zr"

else

	archiver_name="zip"

fi


archiver=`which ${archiver_name}`

if [ ! -x "${archiver}" ] ; then

	echo "Error, no archiver available (${archiver_name})." 1>&2
	exit 30

fi

indexer_name="resource_indexer.sh"

# Needed to locate the indexer:
if [ -z "$Ceylan_PREFIX" ] ; then

	osdl_env_file="${LOANI_INSTALLATIONS}/OSDL-environment.sh"

	if [ -f "${osdl_env_file}" ] ; then
		. "${osdl_env_file}"
	fi

fi


indexer_dir="$Ceylan_PREFIX/share/Ceylan/scripts/erlang/tools/bin"
indexer=`PATH=$indexer_dir:$PATH which ${indexer_name}`

#echo "indexer_dir = ${indexer_dir}"

if [ ! -x "${indexer}" ] ; then

	echo "Error, no resource indexer tool (${indexer_name}) available. Was Ceylan-Erlang installed? If not, run 'make install-erlang' from Ceylan/trunk/src/code/scripts." 1>&2
	exit 35

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
#${CP} -r "${archive_directory}" ${tmp_base}

# Would not be relevant as the target tree would be flatten:
#${FIND} "${archive_directory}" \( -name .svn -prune \) -o  -exec cp... ';'

# --cvs-exclude removes also .svn directories:
${RSYNC} -r --cvs-exclude ${archive_directory}/* ${tmp_base}

if [ ! $? -eq 0	] ; then

	echo "Error, copy to temporary directory '${tmp_base}' failed, removing it." 1>&2
	${RM} -r ${tmp_base}
	exit 13

fi


if [ -e "${archive_target}" ] ; then
	${RM} "${archive_target}"
fi


cd ${tmp_base}


# First, generate an index:
${indexer} -index_basename ${index_basename} -scan_dir `pwd` -output_dir `pwd`/..

if [ ! $? -eq 0	] ; then

	echo "Error, generation of resource index failed." 1>&2
	exit 15

fi

${CP} ../${index_basename}.xml .



# Then obfuscate filenames as well:
files=`${FIND} . -type f`

for f in $files; do

	# Now let's cypher the content of files in the temporary directory:
	${cypher_exec} --nullPlug $f
	if [ ! $? -eq 0 ] ; then

		echo "Error, cyphering of file '$f' with tool '${cypher_exec}' failed." 1>&2
		exit 20

	fi

	base_dir=`dirname $f`
	filename=`basename $f`
	cypher_name $filename
	new_path=$base_dir/$res
	echo "  File $f renamed into $new_path"
	/bin/mv $f $new_path

done


# Finally the directories
# (depth first, as otherwise the cached entries would not match):

# Do not want to scramble the archive base directory:
directories=`${FIND} . -depth -type d`

for d in $directories; do

	if [ ! $d = "." ] ; then

		base_dir=`dirname $d`
		dir_name=`basename $d`
		cypher_name $dir_name
		new_path=$base_dir/$res
		echo "  Directory $d renamed into $new_path"
		/bin/mv $d $new_path

	fi

done


echo

# OAR archives are based on the Lempel-Ziv-Markov chain-Algorithm (LZMA).
# See: http://en.wikipedia.org/wiki/Lempel-Ziv-Markov_chain_algorithm
# On Debian-based distributions, use: 'apt-get install p7zip' to have the
# archiver.
# The lzma package is not enough, as it compresses only files, not filesystem
# full trees.


# From ${archive_directory_name}:

if [ $use_lzma -eq 0 ] ; then

	# For 7zr:
	#LANG= ${archiver} a ../${archive_target} * 1>/dev/null

	# or (best compression):
	LANG= ${archiver} a -t7z -mx=9 ../${archive_target} * 1>/dev/null

else

	# For zip:
	LANG= ${archiver} -r ../${archive_target} *

fi




if [ $? -eq 0 ] ; then

	cd ..
	echo "OSDL Archive ${archive_target} successfully produced from directory ${archive_directory}:"
	file ${archive_target}
	du -sh ${tmp_base}
	/bin/ls -l ${archive_target}

fi


${RM} -r ${tmp_base}

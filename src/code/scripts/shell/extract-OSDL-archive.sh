#!/bin/sh

USAGE="Usage: "`basename $0`" <archive name> <target directory>: this script will extract an OSDL archive (*.oar), specified with a relative path, to specified target directory (as the archive might not be entirely self-contained in a base directory). Paths and file content will be uncyphered.
Ex: `basename $0` myArchive.oar tmp"

# See also osdl/OSDL/trunk/src/code/scripts/shell/create-OSDL-archive.sh.


RM="/bin/rm -f"
FIND=`which find`
RSYNC=`which rsync`
CP="/bin/cp -f"



# Returns a cyphered version (currently rot13) of specified name.
# Source: http://www.miranda.org/~jkominek/rot13/sh/rot13-tr.sh
cypher_name()
{

	res=`echo $1 | tr A-Za-z N-ZA-Mn-za-m`
 
}


if [ ! $# -eq 2 ] ; then
	echo "Error, exactly two parameters required.
    " $USAGE 1>&2
    exit 10
fi


# Usually in osdl/OSDL/trunk/tools/media:
cypher_tool="cypherOSDLFile.exe"
cypher_dir=`dirname $0`"/../../../../tools/media"

osdl_inst_dir="$OSDL_PREFIX/share/OSDL-tools/media"

# Needs to find an absolute path:
cypher_exec=`PATH=$PWD/$cypher_dir:$osdl_inst_dir:$PATH which $cypher_tool 2>/dev/null`

if [ ! -x "${cypher_exec}" ] ; then

	echo "Error, no executable cypher tool ${cypher_tool} found (were the OSDL tools compiled?)." 1>&2
    exit 12

fi



initial_dir=`pwd`

archive_source="${initial_dir}/$1"

if [ ! -f "${archive_source}" ] ; then
	echo "Error, file ${archive_source} not found.
    " $USAGE 1>&2
    exit 11
fi


target_dir="$2"

if [ ! -d "${target_dir}" ] ; then
	echo "Error, directory ${target_dir} not found.
    " $USAGE 1>&2
    exit 12
fi


#echo "Will extract ${archive_source} in ${target_dir}."
	

cd "${target_dir}"



# OAR archives are based on the Lempel-Ziv-Markov chain-Algorithm (LZMA).
# See: http://en.wikipedia.org/wiki/Lempel-Ziv-Markov_chain_algorithm
# On Debian-based distributions, use: 'apt-get install p7zip' to have the 
# archiver.
# The lzma package is not enough, as it compresses only files, not filesystem
# full trees.

dearchiver_name="7zr"
archiver=`which ${dearchiver_name}`

if [ ! -x "${archiver}" ] ; then

	echo "Error, no archiver available (${dearchiver_name})." 1>&2
    exit 1
    
fi


echo
echo "  Extracting now "`basename ${archive_source}`" in directory ${target_dir}:"


LANG= ${archiver} x -y ${archive_source} 1>/dev/null

res=$?
if [ ! $res -eq 0 ] ; then
	echo "Error, archive ${archive_source} could not be successfully extracted." 1>&2
	exit 25
fi


# Now deobfuscate filenames as well:
files=`${FIND} . -type f`

for f in $files; do

	#echo
	# Now let's decypher the content of files in the temporary directory:
	${cypher_exec} --decypher --nullPlug $f 1>/dev/null
	
	base_dir=`dirname $f`
	filename=`basename $f`
	cypher_name $filename
	new_path=$base_dir/$res
	#echo "  File $f renamed into $new_path"
	/bin/mv $f $new_path
	
done


# Finally the directories
# (depth first, as otherwise the cached entries would not match):

# Do not want to scramble the archive base directory:
directories=`${FIND} . -depth -type d`

for d in $directories; do

	if [ ! $d = "." ] ; then
	
		#echo
		base_dir=`dirname $d`
		dir_name=`basename $d`
		cypher_name $dir_name
		new_path=$base_dir/$res
		#echo "  Directory $d renamed into $new_path"
		/bin/mv $d $new_path
		
	fi
	
done

#echo

tree .

echo "OSDL Archive ${archive_source} successfully extracted."


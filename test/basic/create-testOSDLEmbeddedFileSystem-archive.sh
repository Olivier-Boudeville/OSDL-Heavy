#!/bin/sh

USAGE="This script will create a test archive to be used by the testOSDLEmbeddedFileSystem test. Expected to be run directly from the osdl/OSDL/trunk/test/basic directory."


# See also:
#  - osdl/OSDL/trunk/src/code/scripts/shell/create-OSDL-archive.sh
#  - osdl/OSDL/trunk/src/code/scripts/shell/extract-OSDL-archive.sh



archiver_name="create-OSDL-archive.sh"
archiver=`PATH=$PWD:../../src/code/scripts/shell:$PATH which ${archiver_name}`

if [ ! -x "${archiver}" ] ; then

	echo "Error, no archiver available (${archiver_name})." 1>&2
    exit 1
    
fi

mkdir="/bin/mkdir -p"
rm="/bin/rm -f"
cp="/bin/cp -f"

test_archive_dir="test-OSDLEmbeddedFileSystem-archive"
test_archive_name="${test_archive_dir}.oar"

if [ -e "${test_archive_name}" ] ; then
	${rm} "${test_archive_name}"
fi

${mkdir} ${test_archive_dir}


sound_file="../../src/doc/web/common/sounds/OSDL.wav"

if [ ! -e "${sound_file}" ] ; then

	echo "Error, no sound file available (${sound_file})." 1>&2
    exit 10
fi


music_file="../../src/doc/web/common/sounds/welcome-to-OSDL.ogg"

if [ ! -e "${music_file}" ] ; then

	echo "Error, no music file available (${music_file})." 1>&2
    exit 11
fi


echo "First test file." > ${test_archive_dir}/first-file-to-read.txt

${cp} ${sound_file} ${music_file} ${test_archive_dir}

${mkdir} ${test_archive_dir}/test-directory

echo "Second test file." > ${test_archive_dir}/test-directory/second-file-to-read.txt


${archiver} ${test_archive_name} ${test_archive_dir}

if [ $? -eq 0 ] ; then
	echo "Test OSDL Archive ${test_archive_name} successfully produced."
fi

# Produced archive can be decompresssed with: 
# 7zr x test-OSDLEmbeddedFileSystem-archive.oar
# or extract-OSDL-archive.sh

${rm} -r ${test_archive_dir}


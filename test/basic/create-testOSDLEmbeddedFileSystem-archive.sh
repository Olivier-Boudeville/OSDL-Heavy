#!/bin/sh

USAGE="This script will create a test archive to be used by the testOSDLEmbeddedFileSystem test. Expected to be run directly from the osdl/OSDL/trunk/test/basic directory."


# See also:
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


if [ -f "${test_archive_dir}" ] ; then
	${rm} -r "${test_archive_dir}"
fi

${mkdir} ${test_archive_dir}


sound_file="../../src/doc/web/common/sounds/OSDL.wav"

if [ ! -e "${sound_file}" ] ; then

	echo "Error, sound file not available (${sound_file})." 1>&2
	exit 10
fi


music_file="../../src/doc/web/common/sounds/welcome-to-OSDL.ogg"

if [ ! -e "${music_file}" ] ; then

	echo "Error, music file not available (${music_file})." 1>&2
	exit 11
fi


png_image_file="../../src/doc/web/images/Soldier-heavy-purple-small.png"

if [ ! -e "${png_image_file}" ] ; then

	echo "Error, image file not available (${png_image_file})." 1>&2
	exit 12
fi


jpeg_image_file="../../src/doc/web/images/Rune-stone-small.jpg"

if [ ! -e "${jpeg_image_file}" ] ; then

	echo "Error, image file not available (${jpeg_image_file})." 1>&2
	exit 13
fi


font_file="../../src/doc/web/common/fonts/neurochr.ttf"

if [ ! -e "${font_file}" ] ; then

	echo "Error, font file not available (${font_file})." 1>&2
	exit 14
fi


echo "First test file." > ${test_archive_dir}/first-file-to-read.txt

# From encoding-based extension to usage-based one:
${cp} ${sound_file} ${test_archive_dir}/OSDL.sound
${cp} ${music_file} ${test_archive_dir}/welcome-to-OSDL.music
${cp} ${png_image_file} ${test_archive_dir}/Soldier-heavy-purple-small.image
${cp} ${jpeg_image_file} ${test_archive_dir}/Rune-stone-small.image
${cp} ${jpeg_image_file} ${test_archive_dir}/Rune-stone-small.tex3D
${cp} ${font_file} ${test_archive_dir}


${mkdir} ${test_archive_dir}/test-directory

echo "Second test file." > ${test_archive_dir}/test-directory/second-file-to-read.txt


${archiver} ${test_archive_name} ${test_archive_dir}

if [ $? -eq 0 ] ; then
	echo "Test OSDL Archive ${test_archive_name} successfully produced."
else
	echo "Error, execution of ${archiver} failed." 1>&2
	exit 15
fi


# Produced archive can be decompresssed with:
# 7zr x test-OSDLEmbeddedFileSystem-archive.oar
# or extract-OSDL-archive.sh

${rm} -r ${test_archive_dir}

#!/bin/sh

USAGE="This script will create a test archive to be used by the testOSDLEmbeddedFileSystem test."

# See also:
#  - osdl/OSDL/trunk/src/code/scripts/shell/create-OSDL-archive.sh
#  - osdl/OSDL/trunk/src/code/scripts/shell/extract-OSDL-archive.sh



ARCHIVER_NAME="create-OSDL-archive.sh"
ARCHIVER=`PATH=$PWD:../../src/code/scripts/shell:$PATH which ${ARCHIVER_NAME}`

if [ ! -x "${ARCHIVER}" ] ; then

	echo "Error, no archiver available (${ARCHIVER_NAME})." 1>&2
    exit 1
    
fi

MKDIR="/bin/mkdir -p"
RM="/bin/rm -f"

TEST_ARCHIVE_DIR="test-OSDLEmbeddedFileSystem-archive"
TEST_ARCHIVE_NAME="${TEST_ARCHIVE_DIR}.oar"

if [ -e "${TEST_ARCHIVE_NAME}" ] ; then
	${RM} "${TEST_ARCHIVE_NAME}"
fi

${MKDIR} ${TEST_ARCHIVE_DIR}

echo "First test file." > ${TEST_ARCHIVE_DIR}/first-file-to-read.txt

${MKDIR} ${TEST_ARCHIVE_DIR}/test-directory

echo "Second test file." > ${TEST_ARCHIVE_DIR}/test-directory/second-file-to-read.txt


${ARCHIVER} ${TEST_ARCHIVE_NAME} ${TEST_ARCHIVE_DIR}

if [ $? -eq 0 ] ; then
	echo "Test OSDL Archive ${TEST_ARCHIVE_NAME} successfully produced."
fi

# Produced archive can be decompresssed with: 
# 7zr x test-OSDLEmbeddedFileSystem-archive.oar
# or extract-OSDL-archive.sh

${RM} -r ${TEST_ARCHIVE_DIR}


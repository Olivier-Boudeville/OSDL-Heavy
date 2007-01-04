#!/bin/sh

USAGE="`basename $0` : builds and installs the OSDL library from a just-extracted distributed package, and builds, installs and runs the test suite against this install."

SUFFIX=`hostname`-`date '+%Y%m%d-%Hh%M'`
OSDL_INSTALL_ROOT=$HOME/tmp-osdl-install-$SUFFIX
OSDL_INSTALL_TEST_ROOT=$HOME/tmp-osdl-test-install-$SUFFIX

mkdir $OSDL_INSTALL_ROOT $OSDL_INSTALL_TEST_ROOT

CONFIGURE_COMMON_OPT=""
#CONFIGURE_COMMON_OPT="CXX=g++-3.4"

./configure ${CONFIGURE_COMMON_OPT} --prefix=$OSDL_INSTALL_ROOT && make && make install && cd test && ./configure ${CONFIGURE_COMMON_OPT} --prefix=$OSDL_INSTALL_TEST_ROOT --with-osdl-prefix=$OSDL_INSTALL_ROOT && make && make install && ./playTests.sh


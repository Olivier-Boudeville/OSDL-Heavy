# This script is meant to be sourced, so that from a Cygwin shell
# the tests can be launched locally (where they are built) but
# dynamically linked with the appropriate LOANI libraries.

# This

if [ ! -f "playTests.sh" ] ; then
  echo "Error, this script must be sourced from OSDL/trunk/test, nothing done." 1>&2
else
  LIBS_LOCATION=`cygpath -a .`../../../../../LOANI-installations/OSDL-libraries/debug-mt/dll
  #echo $LIBS_LOCATION
  #ls "$LIBS_LOCATION"
  export PATH=$LIBS_LOCATION:$PATH
fi

#echo $PATH



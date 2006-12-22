# This file tells LOANI and the build system about the tools that they must know.
# It corresponds to the gathering of Ceylan and OSDL related tools.
# 
# This file is generated, hence should *not* be modified :
# modify instead ceylan-versions.sh and osdl-versions.sh.

# Centralized version and release informations about used tools for Ceylan. 

# This file is intended to be included both in GNUmakefiles and in bash scripts.




# ----------- For required tools :




# GNU Libtool : http://www.gnu.org/software/libtool/libtool.html

libtool_VERSION="1.5.20"
libtool_ARCHIVE="libtool-${libtool_VERSION}.tar.gz"
libtool_DOWNLOAD_LOCATION="ftp://ftp.cs.tu-berlin.de/pub/gnu/libtool"
libtool_ALTERNATE_DOWNLOAD_LOCATION="http://mirrors.usc.edu/pub/gnu/libtool"
libtool_MD5="fc3b564700aebcb19de841c1c2d66e99"




# win32 pthreads : http://sources.redhat.com/pthreads-win32/

# Use g++ with standard C++ exceptions :
win_pthread_DLL="pthreadGCE.dll"
win_pthread_DLL_MD5="c1a62e3a4d457e70258c6054a6cf5279"

win_pthread_VERSION="2003-09-18"
win_pthread_ARCHIVE="${win_pthread_DLL}"
win_pthread_wget_dir="sources.redhat.com"
win_pthread_ftp_dir="pthreads-dll-${win_pthread_VERSION}"
win_pthread_DOWNLOAD_LOCATION="ftp://${win_pthread_wget_dir}/pub/pthreads-win32/${win_pthread_ftp_dir}"
win_pthread_ALTERNATE_DOWNLOAD_LOCATION=""

win_pthread_HEADER_MAIN="pthread.h"
win_pthread_HEADER_MAIN_MD5="36f00accd29be66dd1784b8c084001cb"

win_pthread_HEADER_SCHED="sched.h"
win_pthread_HEADER_SCHED_MD5="f1969e2a9c55492097831db0f22e45b9"

win_pthread_HEADER_SEM="semaphore.h"
win_pthread_HEADER_SEM_MD5="aa0a28552311dde63fe62661d39d5a27"

# checking for whole directory will be always false, 
# actual test will be made in getwin_pthread. 
win_pthread_MD5="XX"




# Ceylan is retrieved for the moment directly thanks to CVS (export or checkout).
# Next section is therefore ignored.


# Ceylan (libCeylan.so) : http://osdl.sourceforge.net/Ceylan/Ceylan-0.2/src/doc/web

Ceylan_VERSION="0.2"
Ceylan_RELEASE=2
Ceylan_ARCHIVE="Ceylan-${Ceylan_VERSION}.${Ceylan_RELEASE}.tar.bz2"
Ceylan_DOWNLOAD_LOCATION="http://osdn.dl.sourceforge.net/sourceforge/osdl"
Ceylan_ALTERNATE_DOWNLOAD_LOCATION="http://easynews.dl.sourceforge.net/sourceforge/osdl"
Ceylan_MD5="1b679888c2be13ddeae3ea1479db0d7f"

Ceylan_ROOT="Ceylan/Ceylan-${Ceylan_VERSION}"
Ceylan_CVS_SERVER=cvs.sourceforge.net




# ----------- For common build tools :



# gcc : http://www.gnu.org/software/gcc/gcc.html

gcc_VERSION="3.4.4"

gcc_ARCHIVE="gcc-${gcc_VERSION}.tar.bz2"
gcc_DOWNLOAD_LOCATION="ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-${gcc_VERSION}"
gcc_ALTERNATE_DOWNLOAD_LOCATION="ftp://ftp.uvsq.fr/pub/gcc/releases/gcc-${gcc_VERSION}"
gcc_MD5="b594ff4ea4fbef4ba9220887de713dfe"
gcc_BUILD_TREE="gcc-${gcc_VERSION}-buildTree"
gcc_SOURCE_TREE="gcc-${gcc_VERSION}-sourceTree"

# To test newer 4.0.1 gcc : 
# SDL 1.2.8 will not compile with it :
# "src/hermes/HeadMMX.h:53: error: 'ConvertMMX' aliased to undefined symbol '_ConvertMMX'"

#gcc_VERSION="4.0.1"
#gcc_ARCHIVE="gcc-${gcc_VERSION}.tar.bz2"
#gcc_DOWNLOAD_LOCATION="ftp://ftp.lip6.fr/pub/gcc/releases/gcc-${gcc_VERSION}"
#gcc_MD5="947416e825a877a0d69489be1be43be1"
#gcc_BUILD_TREE="gcc-${gcc_VERSION}-buildTree"
#gcc_SOURCE_TREE="gcc-${gcc_VERSION}-sourceTree"



# binutils : http://www.gnu.org/software/binutils/

binutils_VERSION="2.16.1"
binutils_ARCHIVE="binutils-${binutils_VERSION}.tar.gz"
binutils_DOWNLOAD_LOCATION="ftp://ftp.cs.tu-berlin.de/pub/gnu/binutils"
binutils_ALTERNATE_DOWNLOAD_LOCATION="http://ftp.gnu.org/gnu/binutils"
binutils_MD5="818bd33cc45bfe3d5b4b2ddf288ecdea"



# gdb : http://www.gnu.org/software/gdb/gdb.html

gdb_VERSION="6.3"
gdb_ARCHIVE="gdb-${gdb_VERSION}.tar.bz2"
gdb_DOWNLOAD_LOCATION="ftp://ftp.cs.tu-berlin.de/pub/gnu/gdb"
gdb_ALTERNATE_DOWNLOAD_LOCATION="http://ftp.gnu.org/gnu/gdb"
gdb_MD5="05b928f41fa5b482e49ca2c24762a0ae"





# ----------- For optional tools ;



# dot : http://www.research.att.com/sw/tools/graphviz/

dot_VERSION="2.6"
dot_ARCHIVE="graphviz-${dot_VERSION}.tar.gz"
dot_DOWNLOAD_LOCATION="http://www.graphviz.org/pub/graphviz/ARCHIVE"
dot_ALTERNATE_DOWNLOAD_LOCATION=""
dot_MD5="0d61fc4f8660be31503d4f9ab6f26bf0"



# doxygen : http://www.stack.nl/~dimitri/doxygen/

doxygen_VERSION="1.4.5"
doxygen_ARCHIVE="doxygen-${doxygen_VERSION}.src.tar.gz"
doxygen_DOWNLOAD_LOCATION="ftp://ftp.stack.nl/pub/users/dimitri"
doxygen_ALTERNATE_DOWNLOAD_LOCATION=""
doxygen_MD5="6319afd44c47a4bcf49658c995322f0e"



# tidy : http://tidy.sourceforge.net/ (currently deactivated)

tidy_ARCHIVE="tidy_src.tgz"
tidy_DOWNLOAD_LOCATION="http://tidy.sourceforge.net/src"
tidy_ALTERNATE_DOWNLOAD_LOCATION=""
# Changes often, not serious if does not match :
tidy_MD5="XXX"


# End of Ceylan tools definitions.



# Centralized version and release informations about used tools for OSDL. 

# This file is intended to be included both in GNUmakefiles and in bash scripts.




# ----------- For required tools :


# SDL

SDL_VERSION="1.2.9"
SDL_ARCHIVE="SDL-$SDL_VERSION.tar.gz"
SDL_DOWNLOAD_LOCATION="http://www.libsdl.org/release"
SDL_ALTERNATE_DOWNLOAD_LOCATION=""
SDL_MD5="80919ef556425ff82a8555ff40a579a0"



# JPEG library (libjpeg.so) : http://www.ijg.org/

libjpeg_VERSION="6b"
libjpeg_ARCHIVE="jpegsrc.v${libjpeg_VERSION}.tar.gz"
libjpeg_DOWNLOAD_LOCATION="ftp://ftp.uu.net/graphics/jpeg"
libjpeg_ALTERNATE_DOWNLOAD_LOCATION="http://www.ijg.org/files"
libjpeg_MD5="dbd5f3b47ed13132f04c685d608a7547"



# PNG library (libpng.so) : http://www.libpng.org/pub/png/libpng.html

libpng_VERSION="1.2.8"
libpng_ARCHIVE="libpng-${libpng_VERSION}.tar.bz2"
libpng_DOWNLOAD_LOCATION="http://ovh.dl.sourceforge.net/sourceforge/libpng"
libpng_ALTERNATE_DOWNLOAD_LOCATION="ftp://ftp.simplesystems.org/pub/libpng/src/"
libpng_PREVIOUS_DOWNLOAD_LOCATION="ftp://swrinde.nde.swri.edu/pub/png/src"
libpng_MD5="00cea4539bea4bd34cbf8b82ff9589cd"



# zlib library (libzlib.a) : http://www.zlib.net/

zlib_VERSION="1.2.3"
zlib_ARCHIVE="zlib-${zlib_VERSION}.tar.gz"
zlib_DOWNLOAD_LOCATION="http://www.zlib.net"
zlib_ALTERNATE_DOWNLOAD_LOCATION="http://www.gzip.org/zlib"
zlib_MD5="debc62758716a169df9f62e6ab2bc634"


# TIFF library (libtiff.so) : http://www.remotesensing.org/libtiff/
# (TIFF support for OSDL not maintained anymore : use PNG/JPEG instead)
libtiff_VERSION="3.7.3"
libtiff_ARCHIVE="tiff-v${libtiff_VERSION}.tar.gz"
libtiff_DOWNLOAD_LOCATION="ftp://ftp.remotesensing.org/pub/libtiff/"
libtiff_ALTERNATE_DOWNLOAD_LOCATION=""
libtiff_MD5="xxx"



# SDL_image : http://www.libsdl.org/projects/SDL_image/release/

SDL_image_VERSION="1.2.4"
SDL_image_ARCHIVE="SDL_image-${SDL_image_VERSION}.tar.gz"
SDL_image_DOWNLOAD_LOCATION="http://www.libsdl.org/projects/SDL_image/release"
SDL_image_ALTERNATE_DOWNLOAD_LOCATION=""
SDL_image_MD5="70bf617f99e51a2c94550fc79d542f0b"


# SDL_image pre-built archives


# Precompiled binaries of SDL_image, zlib, jpeg, png for Windows : http://www.libsdl.org/projects/SDL_image/release/

SDL_image_win_precompiled_VERSION=1.2.4
SDL_image_win_precompiled_ARCHIVE="SDL_image-devel-${SDL_image_win_precompiled_VERSION}-VC6.zip"
SDL_image_win_precompiled_DOWNLOAD_LOCATION="http://www.libsdl.org/projects/SDL_image/release"
SDL_image_win_precompiled_ALTERNATE_DOWNLOAD_LOCATION=""
SDL_image_win_precompiled_MD5=""


# Precompiled binaries of SDL_image, zlib, jpeg, png for Mac OS X : http://www.libsdl.org/projects/SDL_image/release/

SDL_image_osx_precompiled_VERSION=1.2.4-1
SDL_image_osx_precompiled_ARCHIVE="SDL_image-devel-${SDL_image_osx_precompiled_VERSION}.ppc.rpm"
SDL_image_osx_precompiled_DOWNLOAD_LOCATION="http://www.libsdl.org/projects/SDL_image/release"
SDL_image_osx_precompiled_ALTERNATE_DOWNLOAD_LOCATION=""
SDL_image_osx_precompiled_MD5=""



# SDL_gfx (libSDL_gfx.so) : http://www.ferzkopp.net/Software/SDL_gfx-2.0/
# No binaries available !

SDL_gfx_MAJOR_VERSION="2.0"
SDL_gfx_VERSION="${SDL_gfx_MAJOR_VERSION}.13"
SDL_gfx_ARCHIVE="SDL_gfx-${SDL_gfx_VERSION}.tar.gz"
SDL_gfx_DOWNLOAD_LOCATION="http://www.ferzkopp.net/Software/SDL_gfx-${SDL_gfx_MAJOR_VERSION}"
SDL_gfx_ALTERNATE_DOWNLOAD_LOCATION=""
SDL_gfx_MD5="b1ce778232db0b1979695f0a5a945a13"



# freetype (libfreetype.so) : http://freetype.sourceforge.net

# Sources.

freetype_VERSION="2.1.10"
freetype_ARCHIVE="freetype-${freetype_VERSION}.tar.bz2"
freetype_DOWNLOAD_LOCATION="http://savannah.nongnu.org/download/freetype"
freetype_ALTERNATE_DOWNLOAD_LOCATION="http://heanet.dl.sourceforge.net/sourceforge/freetype"
freetype_PREVIOUS_DOWNLOAD_LOCATION="http://belnet.dl.sourceforge.net/sourceforge/freetype"
freetype_OTHER_DOWNLOAD_LOCATION="http://ovh.dl.sourceforge.net/sourceforge/freetype"
freetype_MD5="a4012e7d1f6400df44a16743b11b8423"

# Precompiled binaries of freetype for Windows.
# See : http://gnuwin32.sourceforge.net/packages/freetype.htm
freetype_win_precompiled_VERSION="2.1.9"
freetype_win_precompiled_ARCHIVE="freetype-${freetype_VERSION}-1-bin.zip"
freetype_win_precompiled_ARCHIVE_SECOND="freetype-${freetype_VERSION}-1-lib.zip"
freetype_win_precompiled_DOWNLOAD_LOCATION="http://belnet.dl.sourceforge.net/sourceforge/gnuwin32"
freetype_win_precompiled_ALTERNATE_DOWNLOAD_LOCATION=""
freetype_win_precompiled_MD5=""


# Precompiled binaries of freetype for Mac OSX.

freetype_osx_precompiled_VERSION="2.1.7"
freetype_osx_precompiled_ARCHIVE=""
freetype_osx_precompiled_DOWNLOAD_LOCATION=""
freetype_osx_precompiled_ALTERNATE_DOWNLOAD_LOCATION=""
freetype_osx_precompiled_MD5=""



# SDL_ttf (libSDL_ttf.so) : http://www.libsdl.org/projects/SDL_ttf/

# Sources.
 
SDL_ttf_VERSION="2.0.7"
SDL_ttf_ARCHIVE="SDL_ttf-${SDL_ttf_VERSION}.tar.gz"
SDL_ttf_DOWNLOAD_LOCATION="http://www.libsdl.org/projects/SDL_ttf/release"
SDL_ttf_ALTERNATE_DOWNLOAD_LOCATION=""
SDL_ttf_MD5="0f6ee1a502e6913a412aac353dc75bbc"


# Precompiled binaries of SDL_ttf for Windows.

SDL_ttf_win_precompiled_VERSION="2.0.7"
SDL_ttf_win_precompiled_ARCHIVE="SDL_ttf-${SDL_ttf_VERSION}-win32.zip"
SDL_ttf_win_precompiled_DOWNLOAD_LOCATION="http://www.libsdl.org/projects/SDL_ttf/release"
SDL_ttf_win_precompiled_ALTERNATE_DOWNLOAD_LOCATION=""
SDL_ttf_win_precompiled_MD5=""

# Precompiled binaries of SDL_ttf for Mac OSX.

SDL_ttf_osx_precompiled_VERSION="2.0.7"
SDL_ttf_osx_precompiled_ARCHIVE=""
SDL_ttf_osx_precompiled_DOWNLOAD_LOCATION="http://www.libsdl.org/projects/SDL_ttf/release"
SDL_ttf_osx_precompiled_ALTERNATE_DOWNLOAD_LOCATION=""
SDL_ttf_osx_precompiled_MD5=""


# libvorbis (libvorbis.so) : http://www.xiph.org/vorbis/
libvorbis_VERSION="1.1.1"
libvorbis_ARCHIVE="libvorbis-${libvorbis_VERSION}.tar.gz"
libvorbis_DOWNLOAD_LOCATION="http://downloads.xiph.org/releases/vorbis"
libvorbis_ALTERNATE_DOWNLOAD_LOCATION="http://www.vorbis.com/files/${libvorbis_VERSION}/unix"
libvorbis_MD5="b77270c24840af4de54bea5ad1c0b252"



# libogg (libogg.so) : http://www.xiph.org/ogg/

libogg_VERSION="1.1.2"
libogg_ARCHIVE="libogg-${libogg_VERSION}.tar.gz"
libogg_DOWNLOAD_LOCATION="http://downloads.xiph.org/releases/ogg"
libogg_ALTERNATE_DOWNLOAD_LOCATION="http://www.vorbis.com/files/${libvorbis_VERSION}/unix"
libogg_MD5="4d82996517bf33bb912c97e9d0b635c4"



# SDL_mixer (libSDL_mixer.so) : http://www.libsdl.org/projects/SDL_mixer/

SDL_mixer_VERSION="1.2.6"
SDL_mixer_ARCHIVE="SDL_mixer-${SDL_mixer_VERSION}.tar.gz"
SDL_mixer_DOWNLOAD_LOCATION="http://www.libsdl.org/projects/SDL_mixer/release"
SDL_mixer_MD5="2b8beffad9179d80e598c22c80efb135"



# OSDL is retrieved for the moment directly thanks to CVS (export or checkout).
# Following sections are therefore ignored.



# OSDL (libOSDL.so) : http://osdl.sourceforge.net

# OSDL can be retrieved from CVS or from a source code archive.

OSDL_VERSION="0.3"
OSDL_RELEASE=2
OSDL_ARCHIVE="OSDL-${OSDL_VERSION}.${OSDL_RELEASE}.tar.bz2"
OSDL_DOWNLOAD_LOCATION="http://osdn.dl.sourceforge.net/sourceforge/osdl"
OSDL_ALTERNATE_DOWNLOAD_LOCATION="http://easynews.dl.sourceforge.net/sourceforge/osdl"
OSDL_MD5="f91aa3818180094af48df03e2a199be2"

OSDL_ROOT="OSDL/OSDL-${OSDL_VERSION}"
OSDL_CVS_SERVER=cvs.sourceforge.net


# Next to come may be : SDL_sound, PhysFS



# End of OSDL tools definitions.




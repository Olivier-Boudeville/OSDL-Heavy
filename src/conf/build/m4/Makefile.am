# osdl.m4 is to be found in ${top_srcdir}/test to help OSDL users
# detecting the OSDL library appropriately from their programs.

m4dir=${datadir}/@PROJECT_NAME@/aclocal
dist_m4_DATA=acinclude-osdl.m4

M4_FILES = \
	axinclude.m4 \
	osdl.m4      \
	pkg.m4       \
	ceylan.m4    \
	vorbis.m4    \
	ogg.m4       \
	sdl.m4       \
	opengl.m4

			
EXTRA_DIST = $(M4_FILES)

	
install-data-local:
	@for f in $(M4_FILES); do cp -f $(srcdir)/$$f @OSDL_INSTALL_SHARED_DIR@; done
	
	

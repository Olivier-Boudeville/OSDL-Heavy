
# Root Makefile.am for the whole OSDL package, including tests.

# Requires following version of automake or newer:
AUTOMAKE_OPTIONS = 1.9

SUBDIRS = src 


# These are the subdirectories which will be distributed with 'make dist':
# (the 'test' directory is not included since it is managed by dist-hook, as
# it may or may not be configured).
DIST_SUBDIRS = $(SUBDIRS) 


.PHONY: source-release update-basic-doc-files bz2 tgz style doc api stat    \
	help hints info-files info-project-files info-library-sources           \
	rm-svn clean-local real-clean clean-generated distclean-local


# Source archive settings.

SRC_RELEASE_ARCHIVE_DIR  = @OSDL_ARCHIVE_DIR@/source-releases
SRC_RELEASE_ARCHIVE_NAME = @PROJECT_NAME@-@OSDL_FULL_VERSION@.tar.bz2

SRC_SNAPSHOT_ARCHIVE_DIR  = @OSDL_ARCHIVE_DIR@/source-snapshots
SRC_SNAPSHOT_ARCHIVE_NAME = \
`date '+%Y%m%d'`-@PROJECT_NAME@-@OSDL_FULL_VERSION@




# A file is assumed to be a project file if it is a file, not located in
# a directory named .svn or .libs or LOANI-*, not a generated binary
# (object file, executable, library ), not a dependency. 
# Using implicit definition of a project file (defining what it is not
# rather than what it is) reduces the risk of missing a project file:
# in case of mistake, too many files will be archived, not too few.
# Useful notably for bz2 and tgz make targets.
# The following line should not be broken otherwise it will not work.
OSDL_PROJECT_FILES = `current_dir=\`basename $$PWD\`; cd $(top_srcdir)/..; find $$current_dir \( -path '*.svn*' -prune -o -path '*.libs' -prune -o -path '.deps' -prune -o -path '*autom4te.cache*' -prune -o -path 'testOSDL*-logs' -prune -o -path '*LOANI-*' -prune \) -a \( -type d -a ! -name '*.svn*' -a ! -name '.libs' -a ! -name '*LOANI*' \) -o \( -type f -a ! -name '*.o' -a ! -name '*.exe' ! -name '*.a' ! -name '*.so*' ! -name '.*list' ! -name '*.lo' ! -name '*.Plo' ! -name '*.la' ! -name '*.lai' ! -name 'Makefile.in' ! -name 'stamp-h1' ! -name 'OSDL-?.?.tar.gz' ! -name 'Makefile' ! -name '.*.d' ! -name '*.log' ! -name '*.pyc' \)| grep -v '.svn' | grep -v '.deps' | grep -v autom4te.cache`
	
		
# A file is assumed to be a source file if it is a file, not located in
# a directory named .svn, and if its name finishes by .h or .cc. 
# Useful notably for style make targets. 
# Only takes into account the library (ex: test sources are ignored).
# The following line should not be broken otherwise it will not work.
OSDL_LIBRARY_SOURCE_FILES = `cd $(top_srcdir); find src/code -path '*.svn*' -prune -a \( -type d -a ! -name '*.svn*' \) -o -name '*.cc' -o -name '*.h' -a -type f| grep -v '.svn'`



# Builds RPM distribution package from archive:
rpm: tgz
	@echo; echo "@OSDL_COMMAND_STYLE@    Making rpm package \
	@OSDL_DEFAULT_STYLE@"; echo
	@rpm -ta $(PACKAGE)-$(VERSION).tar.gz


# Here rules to rebuild the export lists for BeOS, MacOS and Win32 could exist.


# Installs the libraries only, prevents from rebuilding applications:
install-lib:
	cd src/code && $(MAKE) install-libLTLIBRARIES


# Run ldconfig after installing the library:
install-exec-hook:
	-ldconfig

update-basic-doc-files:
	cd ${top_builddir}/src/doc/basic; $(MAKE) update-basic-doc-files 


# Note: usually deprecated in favor of generate-releases.
source-release: update-basic-doc-files
	@echo; echo "@OSDL_COMMAND_STYLE@    Making source release \
	$(SRC_RELEASE_ARCHIVE_NAME):@OSDL_DEFAULT_STYLE@"; echo
	-@cd test; if [ -f Makefile ] ; then $(MAKE) clean; fi
	@mkdir -p $(SRC_RELEASE_ARCHIVE_DIR); \
	files=$(OSDL_PROJECT_FILES); cd $(top_srcdir)/.. ; \
	tar cvf - $$files | \
	bzip2 > "$(SRC_RELEASE_ARCHIVE_DIR)/$(SRC_RELEASE_ARCHIVE_NAME)" \
	&& echo \
	&& echo "@OSDL_COMMAND_STYLE@    + Source release for @PROJECT_NAME@ \
	version @OSDL_FULL_VERSION@ available at:" \
	&& echo "      $(SRC_RELEASE_ARCHIVE_DIR)/$(SRC_RELEASE_ARCHIVE_NAME) \
	@OSDL_DEFAULT_STYLE@" \
	&& echo


# Creates a SVN snapshot one can run 'svn update' on:
# (note: seems to delete as well generated file in the current source base)
snapshot:
	@echo; echo "@OSDL_COMMAND_STYLE@    Creating a SVN OSDL snapshot \
	@OSDL_DEFAULT_STYLE@"; echo
	@svn co https://svn.sourceforge.net/svnroot/OSDL OSDL
	@(cd OSDL/OSDL/trunk/src/conf/build && ./autogen.sh --no-build \
	&& cd ../../.. && /bin/rm -rf autom4te.cache )
	@cd OSDL && tar cvhf - OSDL | \
	bzip2 > $(HOME)/OSDL-@OSDL_VERSION@-svn-snapshot.tar.bz2 \
	&& echo \
	&& echo "@OSDL_COMMAND_STYLE@    + SVN snapshot for @PROJECT_NAME@ \
	available at:" \
	&& echo "      $(HOME)/OSDL-@OSDL_VERSION@-svn-snapshot.tar.bz2 \
	@OSDL_DEFAULT_STYLE@" && echo
	-@/bin/rm -rf OSDL
	
	
# Brute-force archiving with bzip2, see also the 'source-release' target.
# clean required not to archive test* executables:	
# To avoid uselessly fat archives, some cleaning has
# to be done, but using cleanGeneratedConfigFiles.sh removes the 
# Makefile being used itself.	
# Note: usually deprecated in favor of generate-releases.
bz2:
	@echo; echo "@OSDL_COMMAND_STYLE@    Saving all \
	@PROJECT_NAME@-@OSDL_VERSION@ project \
	in a bz2 archive@OSDL_DEFAULT_STYLE@"; echo
	@mkdir -p $(SRC_SNAPSHOT_ARCHIVE_DIR); files=$(OSDL_PROJECT_FILES);\
	cd $(top_srcdir)/.. ; tar cvf - $$files | bzip2 > \
	"$(SRC_SNAPSHOT_ARCHIVE_DIR)/$(SRC_SNAPSHOT_ARCHIVE_NAME).tar.bz2" \
	&& echo && echo "@OSDL_COMMAND_STYLE@    + whole @PROJECT_NAME@ \
	project has been saved in file"	&& echo \
	"      $(SRC_SNAPSHOT_ARCHIVE_DIR)/$(SRC_SNAPSHOT_ARCHIVE_NAME).tar.bz2 \
	@OSDL_DEFAULT_STYLE@" \
	&& echo
	
	
# Brute-force archiving with gzip, see also the 'source-release' target.
# clean required not to archive test* executables:			
# Note: usually deprecated in favor of generate-releases.
tgz:
	@echo; echo "@OSDL_COMMAND_STYLE@    Saving all \
	@PROJECT_NAME@-@OSDL_VERSION@ project \
	in a tgz archive@OSDL_DEFAULT_STYLE@"; echo
	@mkdir -p $(SRC_SNAPSHOT_ARCHIVE_DIR); files=$(OSDL_PROJECT_FILES);\
	cd $(top_srcdir)/.. ; tar cvf - $$files | gzip > \
	"$(SRC_SNAPSHOT_ARCHIVE_DIR)/$(SRC_SNAPSHOT_ARCHIVE_NAME).tar.gz" \
	&& echo	&& echo "@OSDL_COMMAND_STYLE@    + whole @PROJECT_NAME@ \
	project has been saved in file"	&& echo \
	"      $(SRC_SNAPSHOT_ARCHIVE_DIR)/$(SRC_SNAPSHOT_ARCHIVE_NAME).tar.gz \
	@OSDL_DEFAULT_STYLE@" \
	&& echo



ASTYLE      = `which astyle 2>/dev/null`
ASTYLE_CONF = ${top_srcdir}/src/conf/doc/astyle.conf


style: 
	@[ -x "$(ASTYLE)" ] \
	|| ( echo "@OSDL_ERROR_STYLE@    Unable to format source code \
	according to @PROJECT_NAME@'s conventions: \
	astyle tool not found.@OSDL_DEFAULT_STYLE@"; false ) 
	@echo
	@echo "    Do you really want to use astyle on each of \
	the project files ? (y/n) [n]"; \
	read value; if [ "$$value" == "y" ]; then \
	echo; echo "@OSDL_COMMAND_STYLE@    Enforcing standard project style \
	defined in $(ASTYLE_CONF) in source files:@OSDL_DEFAULT_STYLE@ ";\
	stylefile=$(ASTYLE_CONF); cd $(top_srcdir)/.. ;\
	for f in $(OSDL_LIBRARY_SOURCE_FILES); do ( echo "    Processing $$f";\
	$(ASTYLE) `cat $$stylefile` < $$f > $$f.tmp;\
	mv $$f.tmp $$f ); done; else echo \
	"@OSDL_ERROR_STYLE@astyle aborted.@OSDL_DEFAULT_STYLE@"; fi


doc:
	cd $(top_srcdir)/src/doc && $(MAKE) site  
	
	
api:
	cd $(top_srcdir)/src/doc && $(MAKE) api  


stat:
	@echo 		
	@for f in $(OSDL_LIBRARY_SOURCE_FILES);\
	do ( cat $$f >> .wholeSrcContent.txt; \
	echo $$f >> .srcFileList.txt ); done
	@s=`cat .wholeSrcContent.txt | wc`; \
	echo "@OSDL_COMMAND_STYLE@    In\
	@PROJECT_NAME@-@OSDL_FULL_VERSION@'s source code, there are: \
	"; echo ; \
	echo "    "`cat .srcFileList.txt | wc -w`  files; \
	echo "    "`echo $$s | awk '{print $$1}'` lines; \
	echo "    "`echo $$s | awk '{print $$2}'` words; \
	echo "    "`echo $$s | awk '{print $$3}'` characters\
	"@OSDL_DEFAULT_STYLE@"; 
	-@/bin/rm -f .wholeSrcContent.txt .srcFileList.txt


help:
	@echo; echo "@OSDL_DEFAULT_STYLE@  Main make targets are:"; echo 
	@echo "    @OSDL_ITEM_STYLE@all @OSDL_DEFAULT_STYLE@           \
	- builds and installs a full ready-to-use\
	@PROJECT_NAME@-$(VERSION) release"
	@echo "    @OSDL_ITEM_STYLE@install @OSDL_DEFAULT_STYLE@       \
	- installs an already built @PROJECT_NAME@ release in PREFIX"
	@echo "                 [currently PREFIX=$(prefix)]"	
	@echo "    @OSDL_ITEM_STYLE@tests @OSDL_DEFAULT_STYLE@         \
	- generates all tests for built libraries"		
	@echo "    @OSDL_ITEM_STYLE@source-release @OSDL_DEFAULT_STYLE@\
	- creates a new source release archive"	
	@echo "    @OSDL_ITEM_STYLE@bz2 @OSDL_DEFAULT_STYLE@           \
	- creates @PROJECT_NAME@ bz2 archive in @OSDL_ARCHIVE_DIR@"
	@echo "    @OSDL_ITEM_STYLE@tgz @OSDL_DEFAULT_STYLE@           \
	- creates @PROJECT_NAME@ tgz archive in @OSDL_ARCHIVE_DIR@"
	@echo "    @OSDL_ITEM_STYLE@style @OSDL_DEFAULT_STYLE@         \
	- format source code according to @PROJECT_NAME@'s conventions"
	@echo "    @OSDL_ITEM_STYLE@doc @OSDL_DEFAULT_STYLE@           \
	- builds @PROJECT_NAME@'s documentation, API excluded"
	@echo "    @OSDL_ITEM_STYLE@api @OSDL_DEFAULT_STYLE@           \
	- builds @PROJECT_NAME@'s API pages"	
	@echo "    @OSDL_ITEM_STYLE@stat @OSDL_DEFAULT_STYLE@          \
	- prints statistics about @PROJECT_NAME@'s source code"
	@echo "    @OSDL_ITEM_STYLE@info @OSDL_DEFAULT_STYLE@          \
	- displays some project-related variables"
	@echo "    @OSDL_ITEM_STYLE@rm-svn @OSDL_DEFAULT_STYLE@        \
	- removes SVN-related files and directories"
	@echo "    @OSDL_ITEM_STYLE@clean @OSDL_DEFAULT_STYLE@         \
	- removes all generated files"
	@echo "    @OSDL_ITEM_STYLE@help @OSDL_DEFAULT_STYLE@          \
	- displays this message"	
	@echo "    @OSDL_ITEM_STYLE@hints @OSDL_DEFAULT_STYLE@         \
	- shows how to overcome build errors and redefine output\
	directories"	
	@echo
	@echo "Other @OSDL_ITEM_STYLE@GNU standard targets \
	@OSDL_DEFAULT_STYLE@ also available with their usual meaning:"
	@echo "  install-strip, uninstall, distclean, check, installcheck \
	and dist"
	@echo
	@echo "You can find detailed informations and support at @OSDL_SF_URL@"


hints: 
	@echo
	@echo "Stable versions of @PROJECT_NAME@ should compile fine \
	out-of-the-box on all supported platforms."
	@echo
	@echo "The most common source of build failure is when the \
	build system detects a tool's availability (for instance, libtool) \
	whereas the found tool is in a too old version to be used by \
	@PROJECT_NAME@." 
	@echo
	@echo 
	@echo "Following settings are set:"
	@echo
	@echo " + where most of @PROJECT_NAME@ is to be installed: \
	currently $(prefix)"
	@echo
	@echo " + where all the @PROJECT_NAME@'s executables are to be \
	installed: currently $(exec_prefix)"
	@echo
	@echo " + where @PROJECT_NAME@'s html documentation is to be \
	installed, useful to feed a web server: @OSDL_LOCAL_WEB_DIR@"
	@echo
	@echo " + base directory where @PROJECT_NAME@ archives \
	(source release, bz2 or tgz) are to be put: @OSDL_ARCHIVE_DIR@"
	@echo 
	
	
info-files: info-project-files info-library-sources


info-project-files:
	@echo "OSDL_PROJECT_FILES = $(OSDL_PROJECT_FILES)"
	
	
info-library-sources:
	@echo "OSDL_LIBRARY_SOURCE_FILES = $(OSDL_LIBRARY_SOURCE_FILES)"


rm-svn:
	@echo
	@echo "@OSDL_CLEAN_STYLE@    Removing all SVN-related files from \
	top source directory@OSDL_DEFAULT_STYLE@"
	-@echo "    Do you really want to remove all SVN-related files, \
	hence possibly loosing modifications not already checked-in ? (y/n) [n]";\
	read value; if [ "$$value" == "y" ]; then \
	/bin/rm -rf `find ${top_srcdir} -name .svn -print 2>/dev/null`


clean-local:
	@echo "@OSDL_CLEAN_STYLE@    Cleaning all from top build \
	directory@OSDL_DEFAULT_STYLE@" 
	-@cd ${top_builddir}; /bin/rm -f AUTHORS COPYING.LIB ChangeLog FAQ \
	INSTALL MAINTENERS NEWS	README THANKS TODO $(RELEASE_FILES)


real-clean: clean-local
	-@/bin/rm -f Makefile.in aclocal.m4 config.log configure configure.ac \
	libtool config.status config.cache 
	-@/bin/rm -rf autom4te.cache


# Only useful if the source and build directories are the same.
clean-generated: 
	@cd ${top_srcdir}/conf/build; cleanGeneratedConfigFiles.sh 


distclean-local: real-clean


# Grab the test programs for the distribution:
# (the touch for both m4 is due to a libltdl bug with dist automake target)
dist-hook:
	@echo "@OSDL_COMMAND_STYLE@    Running distribution hook@OSDL_DEFAULT_STYLE@"
	@if ! test -f $(srcdir)/test/configure; then \
	echo "Test configure script not available, generating it." ; \
	cd $(srcdir)/test ; ./autogen.sh --only-prepare-dist -g; fi
	@if test -f $(srcdir)/test/Makefile; then \
	(cd $(srcdir)/test; $(MAKE) distclean); fi
	-@/bin/rm -rf $(srcdir)/test/autom4te* $(srcdir)/test/testsOutcome.txt
	@cp -rp $(srcdir)/test $(distdir)
	@if ! test -f $(srcdir)/tools/configure; then \
	echo "Tool configure script not available, generating it." ; \
	cd $(srcdir)/tools ; ./autogen.sh --only-prepare-dist -g; fi
	@if test -f $(srcdir)/tools/Makefile; then \
	(cd $(srcdir)/tools; $(MAKE) distclean); fi	
	-@/bin/rm -rf $(srcdir)/tools/autom4te*
	@cp -rp $(srcdir)/tools $(distdir)
	-@/bin/rm -rf `find $(distdir) -type d -name '.svn' -print`
	@cd $(distdir) && ../$(srcdir)/test/cleanTestGeneratedFiles.sh
	-@cd $(distdir); /bin/rm -f config.log config.status libtool \
	code/OSDLConfig.h
	-@touch $(srcdir)/libtool.m4 $(srcdir)/ltdl.m4


uninstall-hook:
	@echo; echo "@OSDL_INSTALL_STYLE@    Uninstalling all OSDL shared \
	files from $(DESTDIR)@OSDL_INSTALL_SHARED_DIR@ @OSDL_DEFAULT_STYLE@"
	-@/bin/rm -rf $(DESTDIR)@OSDL_INSTALL_SHARED_DIR@


RELEASE_FILES = osdl-@OSDL_VERSION@.tar.gz osdl-@OSDL_VERSION@.tar.bz2 osdl-@OSDL_VERSION@.zip


upload-releases: $(RELEASE_FILES)
	   @echo; echo "@OSDL_COMMAND_STYLE@    Uploading release files $(RELEASE_FILES) to Sourceforge @OSDL_DEFAULT_STYLE@"
	   @rsync -avP -e ssh $(RELEASE_FILES) @OSDL_DEFAULT_SF_USER@@frs.sourceforge.net:uploads/


# Apparently does not work if listed only as:
# 'generate-releases: dist-bzip2 dist-zip dist'
# (the release directory is removed and then next archive is empty)
generate-releases: 
	@$(MAKE) dist-bzip2 && $(MAKE) dist-zip && $(MAKE) dist


info-releases:
	@echo "RELEASE_FILES = $(RELEASE_FILES)"
	
	

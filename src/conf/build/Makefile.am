
SUBDIRS=m4 loani

.PHONY: all check-configure download-config-scripts    \
	release blank-MD5 links mktmpdir updateREADME test \
	info-local info-clean clean-download real-clean 


BUILD_SCRIPTS_SET = \
	autogen.sh                     \
	checkConfigure.sh              \
	cleanGeneratedConfigFiles.sh   \
	build-and-test-package.sh



EXTRA_DIST = \
	$(BUILD_SCRIPTS_SET)       \
	MakeConfigure              \
	OSDLToolsSettings.inc      \
	configure-template.ac
	
	
noinst_SCRIPTS = $(BUILD_SCRIPTS_SET)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @OSDL_PKGCONFIG_FILE@

DIFF_TOOL    = `which tkdiff 2>/dev/null`

CONFIGURE_FILE = configure.ac

CONFIGURE_SCRIPTS_BASE_URL = http://savannah.gnu.org/cgi-bin/viewcvs/*checkout*/config/config
# Was/will be : ftp://ftp.gnu.org/gnu/config

DOWNLOADED_HELPERS = config.guess config.sub

CONFIG_FILES = $(DOWNLOADED_HELPERS)


all:
	
		
check-configure: 
	@echo "@OSDL_MAKE_STYLE@    Comparing configure \
	files@OSDL_DEFAULT_STYLE@"
	@./checkConfigure.sh $(DIFF_TOOL)
	@echo "@OSDL_COMMAND_STYLE@    Comparison finished, \
	$(CONFIGURE_FILE) may be updated now."


download-config-scripts: clean-download
	@echo "@OSDL_MAKE_STYLE@    Downloading latest \
	configure helper scripts@OSDL_DEFAULT_STYLE@"
	@for f in $(DOWNLOADED_HELPERS); do \
	wget --quiet $(CONFIGURE_SCRIPTS_BASE_URL)/$$f ; done
	@chmod +x $(DOWNLOADED_HELPERS)


dist-hook:
	@cp $(srcdir)/build-and-test-package.sh $(top_distdir)
	
	
clean-local:
	@echo "@OSDL_CLEAN_STYLE@    Cleaning generated config \
	files@OSDL_DEFAULT_STYLE@"
	-@rm -f autoscan*.log autoscan.err configure.scan
	
	
clean-download:
	@echo "@OSDL_CLEAN_STYLE@    Cleaning downloaded config \
	files@OSDL_DEFAULT_STYLE@"
	-@rm -f $(DOWNLOADED_HELPERS)
	
	
real-clean: clean-local clean-download
	
		
		

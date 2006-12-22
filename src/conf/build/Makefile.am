
SUBDIRS=m4 loani

.PHONY: all check-configure download-config-scripts    \
	release blank-MD5 links mktmpdir updateREADME test \
	infoclean clean-download real-clean 


BUILD_SCRIPTS_SET = \
	autogen.sh                     \
	checkConfigure.sh              \
	cleanGeneratedConfigFiles.sh   \
	build-and-test-package.sh



EXTRA_DIST     = \
	$(BUILD_SCRIPTS_SET)   \
	MakeConfigure          \
	ToolsSettings.inc      \
	configure-template.ac
	
noinst_SCRIPTS = $(BUILD_SCRIPTS_SET)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @CEYLAN_PKGCONFIG_FILE@

DIFF_TOOL    = `which tkdiff 2>/dev/null`

CONFIGURE_FILE = configure.ac

CONFIGURE_SCRIPTS_BASE_URL = http://savannah.gnu.org/cgi-bin/viewcvs/*checkout*/config/config
# Was/will be : ftp://ftp.gnu.org/gnu/config

DOWNLOADED_HELPERS = config.guess config.sub

CONFIG_FILES = $(DOWNLOADED_HELPERS)


all:
	
		
check-configure: 
	@echo "@CEYLAN_MAKE_STYLE@    Comparing configure\
	files@CEYLAN_DEFAULT_STYLE@"
	@./checkConfigure.sh $(DIFF_TOOL)
	@echo "@CEYLAN_COMMAND_STYLE@    Comparison finished,\
	$(CONFIGURE_FILE) may be updated now."


download-config-scripts: clean-download
	@echo "@CEYLAN_MAKE_STYLE@    Downloading latest\
	configure helper scripts@CEYLAN_DEFAULT_STYLE@"
	@for f in $(DOWNLOADED_HELPERS); do \
	wget --quiet $(CONFIGURE_SCRIPTS_BASE_URL)/$$f ; done
	@chmod +x $(DOWNLOADED_HELPERS)


dist-hook:
	@cp $(srcdir)/build-and-test-package.sh $(top_distdir)
	
	
clean-local:
	@echo "@CEYLAN_CLEAN_STYLE@    Cleaning generated config\
	files@CEYLAN_DEFAULT_STYLE@"
	-@rm -f autoscan*.log autoscan.err configure.scan
	
	
clean-download:
	@echo "@CEYLAN_CLEAN_STYLE@    Cleaning downloaded config\
	files@CEYLAN_DEFAULT_STYLE@"
	-@rm -f $(DOWNLOADED_HELPERS)
	
	
real-clean: clean clean-download


# LOANI section.

# Where the LOANI download cache will be put :
LOANI_REPOSITORY_DIR = @LOANI_REPOSITORY_DIR@

# Where LOANI will install packages :
LOANI_INSTALL_DIR    = @LOANI-LOANI_INSTALL_DIR@

# The files that will be in the LOANI archive :
LOANI_FELLOWS_FILES = \
	loani.sh                  \
	loani-requiredTools.sh    \
	loani-commonBuildTools.sh \
	loani-optionalTools.sh    \
	loani-versions.sh         \
	defaultLocations.sh       \
	termUtils.sh              \
	platformDetection.sh      \
	generateBugReport.sh      \
	makeRepositoryLinks.sh    \
	$(LOANI_README_TARGET)


# The version of LOANI :
LOANI_VERSION = @LOANI_VERSION@

# The base name of LOANI archives :
LOANI_ARCHIVE_NAME = LOANI-$(LOANI_VERSION)
 
LOANI_ZIP = $(LOANI_ARCHIVE_NAME).zip
LOANI_TGZ = $(LOANI_ARCHIVE_NAME).tar.gz

# The directory used to prepare the LOANI archive :
LOANI_TMP_ARCHIVE_DIR = $(LOANI_ARCHIVE_NAME)

# Where the Ceylan source archive is, so that its MD5 sum can be computed :
CEYLAN_RELEASE =  $(LOANI_REPOSITORY_DIR)/Ceylan/Ceylan-@CEYLAN_OLDEST_SUPPORTED_VERSION@/@CEYLAN_CURRENT_SOURCE_ARCHIVE@

# Where the OSDL source archive is, so that its MD5 sum can be computed :
OSDL_RELEASE = $(LOANI_REPOSITORY_DIR)/OSDL/OSDL-@OSDL_VERSION@/@OSDL_CURRENT_SOURCE_ARCHIVE@


# The template for the LOANI README :
LOANI_README_TEMPLATE = LOANI-README-template.txt

# The actual README to produce :
LOANI_README_TARGET   = README




release: links $(LOANI_TGZ) $(LOANI_ZIP) 
	@echo "$(make_style)    Release archives for LOANI are available.$(default_style)"
	
blank-MD5:
	@echo "$(install_style)    Blanking MD5 sum of OSDL release to avoid MD5 recursive dependency on archive$(default_style)"
	@../../../../Ceylan/Ceylan-${CEYLAN_VERSION}/src/code/scripts/shell/replaceLineStartingBy.sh OSDL_MD5 'OSDL_MD5="(blanked)"' osdl-versions.sh 

	
links: 
	@ln -sf ../../../../Ceylan/Ceylan-${CEYLAN_VERSION}/src/code/scripts/shell/termUtils.sh
	@ln -sf ../../../../Ceylan/Ceylan-${CEYLAN_VERSION}/src/code/scripts/shell/defaultLocations.sh
	@ln -sf ../../../../Ceylan/Ceylan-${CEYLAN_VERSION}/src/code/scripts/shell/makeRepositoryLinks.sh
	
	
$(LOANI_ZIP): $(LOANI_FELLOWS_FILES) $(LOANI_README_TEMPLATE)
	@$(MAKE) mktmpdir
	@echo "$(make_style)    Building LOANI release file $@$(default_style)"
	@zip $@ $(LOANI_TMP_ARCHIVE_DIR)/* 1>/dev/null 2>&1
	
			
$(LOANI_TGZ): $(LOANI_FELLOWS_FILES) $(LOANI_README_TEMPLATE)
	@$(MAKE) mktmpdir
	@echo "$(make_style)    Building LOANI release file $@$(default_style)"
	@tar chf - $(LOANI_TMP_ARCHIVE_DIR) | gzip > $@


mktmpdir: $(LOANI_README_TARGET)
	-@rm -rf $(LOANI_TMP_ARCHIVE_DIR)
	@mkdir $(LOANI_TMP_ARCHIVE_DIR)
	@cp -f $(LOANI_FELLOWS_FILES) $(LOANI_TMP_ARCHIVE_DIR)
	@chmod +x $(LOANI_TMP_ARCHIVE_DIR)/loani.sh
		

$(LOANI_README_TARGET): $(TOP)/GNUmakevars.inc
	@cat $(LOANI_README_TEMPLATE) | sed 's|PG_VERSION|$(LOANI_VERSION)|g' | sed "s|PG_DATE|`date '+%A, %B %e, %Y (%H:%M)'`|g"> $@


loani-versions.sh: $(CEYLAN_VERSIONS) osdl-versions.sh $(CEYLAN_RELEASE) $(OSDL_RELEASE)
	@echo "$(make_style)    Merging $@$(default_style)"
	@echo "# This file tells LOANI and the build system about the tools that they must know." > $@
	@echo "# It corresponds to the gathering of Ceylan and OSDL related tools." >> $@
	@echo "# " >> $@
	@echo "# This file is generated, hence should *not* be modified :" >> $@
	@echo "# modify instead" `basename $(CEYLAN_VERSIONS)` "and osdl-versions.sh." >> $@
	@echo >> $@
	@cat $(CEYLAN_VERSIONS) osdl-versions.sh >> $@
	@./updateLOANI-MD5.sh $(CEYLAN_RELEASE) $(OSDL_RELEASE)\
	../../../../Ceylan/Ceylan-${CEYLAN_VERSION}/src/code/scripts/shell/replaceLineStartingBy.sh

	
test: release
	@mkdir -p $(HOME)/tmp/LOANI-testing
	@cp -f $(LOANI_ZIP) $(LOANI_TGZ) $(HOME)/tmp/LOANI-testing
	@cd $(HOME)/tmp/LOANI-testing; unzip -qq $(LOANI_ZIP)
	@echo "You can type now : cd $(HOME)/tmp/LOANI-testing; loani.sh -d --sourceforge <your user>"


info:
	@echo "LOANI_VERSION       = $(LOANI_VERSION)"
	@echo "LOANI_ZIP           = $(LOANI_ZIP)" 
	@echo "LOANI_TGZ           = $(LOANI_TGZ)"
	@echo "LOANI_FELLOWS_FILES = $(LOANI_FELLOWS_FILES)"
	@echo "Ceylan release      = $(CEYLAN_RELEASE)"
	@echo "OSDL release        = $(OSDL_RELEASE)"
	

stat:
	@echo "newline, word, and byte counts for LOANI :"
	@cat $(LOANI_FELLOWS_FILES) | wc
		
		
clean:
	@echo "$(clean_style)    Cleaning all in `pwd`$(default_style)"
	-@rm -f wget-log* LOANI.log $(LOANI_ZIP) $(LOANI_TGZ)\
	$(LOANI_README_TARGET) loani-versions.sh
	-@rm -rf LOANI-$(LOANI_VERSION)
	
	
realclean : clean
	@echo "$(clean_style)    Real cleaning all in `pwd`$(default_style)"
	-@rm -rf $(LOANI_REPOSITORY_DIR) $(LOANI_INSTALL_DIR)	
	-@rm -f  $(LOANI_TGZ) $(LOANI_ZIP) 
	


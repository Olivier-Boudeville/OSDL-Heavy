
.PHONY: blank-MD5 prepare-release release test info-local stat\
	clean-local realclean 


LOANI_README_TEMPLATE = LOANI-README-template.txt
LOANI_README_TARGET   = LOANI-README


CEYLAN_TOOLS_SETTINGS = \
	@CEYLAN_INSTALL_ROOT@/share/Ceylan/CeylanToolsSettings.inc
	
OSDL_TOOLS_SETTINGS   = $(srcdir)/../OSDLToolsSettings.inc

LOANI_TOOLS_SETTINGS  = $(srcdir)/LOANIToolsSettings.inc

CEYLAN_RELEASE = $(srcdir)/../../../../../../ceylan/Ceylan/trunk/ceylan-$(CEYLAN_OLDEST_SUPPORTED_VERSION).tar.bz2

OSDL_RELEASE   = $(srcdir)/../../../osdl-$(OSDL_VERSION).tar.bz2


LOANI_FELLOWS_FILES = \
	loani.sh                    \
	loani-requiredTools.sh      \
	loani-commonBuildTools.sh   \
	loani-optionalTools.sh      \
	$(LOANI_TOOLS_SETTINGS)     \
	defaultLocations.sh         \
	termUtils.sh                \
	platformDetection.sh        \
	generateBugReport.sh        \
	makeRepositoryLinks.sh      \
	$(LOANI_README_TARGET)


LOANI_VERSION = @OSDL_VERSION@

LOANI_ARCHIVE_NAME = LOANI-$(LOANI_VERSION)
 
LOANI_ZIP = $(LOANI_ARCHIVE_NAME).zip
LOANI_TGZ = $(LOANI_ARCHIVE_NAME).tar.gz

LOANI_TMP_ARCHIVE_DIR = $(LOANI_ARCHIVE_NAME)


EXTRA_DIST = \
	$(LOANI_FELLOWS_FILES)
	
noinst_SCRIPTS = \
	$(LOANI_FELLOWS_FILES)



REPLACE_SCRIPT = \
	@CEYLAN_INSTALL_ROOT@/share/Ceylan/scripts/shell/replaceLineStartingBy.sh


$(LOANI_README_TARGET): 
	@cp $(top_srcdir)/$(LOANI_README_TARGET) $(srcdir) 
	@cat $(LOANI_README_TARGET) | sed 's|ST_LOANI_VERSION|$(LOANI_VERSION)|g' | sed "s|ST_LOANI_DATE|`LANG= && date '+%A, %B %-e, %Y (%H:%M)'`|g" > $@


$(LOANI_TOOLS_SETTINGS): $(CEYLAN_TOOLS_SETTINGS) $(OSDL_TOOLS_SETTINGS) $(CEYLAN_RELEASE) $(OSDL_RELEASE)
	@echo "@OSDL_MAKE_STYLE@    Merging $@@OSDL_DEFAULT_STYLE@"
	@echo "# This file tells LOANI and the build system about \
	the tools that they must know." > $@
	@echo "# It corresponds to the gathering of \
	Ceylan and OSDL related tools." >> $@
	@echo "# " >> $@
	@echo "# This file is generated, hence should *not* be modified :" >> $@
	@echo "# modify instead" `basename $(CEYLAN_TOOLS_SETTINGS)`\
	"and" `basename $(OSDL_TOOLS_SETTINGS)`"." >> $@
	@echo >> $@
	@echo "# From $(CEYLAN_TOOLS_SETTINGS) :"       >> $@
	@cat $(CEYLAN_TOOLS_SETTINGS)                   >> $@
	@echo "# From $(OSDL_TOOLS_SETTINGS) :"         >> $@
	@cat $(OSDL_TOOLS_SETTINGS)                     >> $@
	@$(srcdir)/updateLOANI-MD5.sh $(CEYLAN_RELEASE) $(OSDL_RELEASE)\
	$(REPLACE_SCRIPT)


blank-MD5:
	@echo "@OSDL_INSTALL_STYLE@    Blanking MD5 sum of OSDL release \
	to avoid MD5 recursive dependency on archive@OSDL_DEFAULT_STYLE@"
	@$(REPLACE_SCRIPT) OSDL_MD5 'OSDL_MD5="(blanked)"' $(OSDL_TOOLS_SETTINGS)


$(LOANI_TGZ): $(LOANI_FELLOWS_FILES) $(LOANI_README_TEMPLATE)
	@echo "@OSDL_MAKE_STYLE@    Building LOANI release file\
	$@@OSDL_DEFAULT_STYLE@"
	@tar chf - $(LOANI_TMP_ARCHIVE_DIR) | gzip > $@


$(LOANI_ZIP): $(LOANI_FELLOWS_FILES) $(LOANI_README_TEMPLATE)
	@echo "@OSDL_MAKE_STYLE@    Building LOANI release file\
	$@@OSDL_DEFAULT_STYLE@"
	@zip $@ $(LOANI_TMP_ARCHIVE_DIR)/* 1>/dev/null 2>&1
	
			
prepare-release:
	-@rm -rf $(srcdir)/$(LOANI_TMP_ARCHIVE_DIR)
	@mkdir $(srcdir)/$(LOANI_TMP_ARCHIVE_DIR)
	@cd $(srcdir) && \
	cp -f $(LOANI_FELLOWS_FILES) $(srcdir)/$(LOANI_TMP_ARCHIVE_DIR)
	@chmod +x $(srcdir)/$(LOANI_TMP_ARCHIVE_DIR)/loani.sh


release: prepare-release $(LOANI_TGZ) $(LOANI_ZIP) 
	@echo "@OSDL_MAKE_STYLE@    Release archives for LOANI\
	are available.@OSDL_DEFAULT_STYLE@"

	
test: release
	@mkdir -p $(HOME)/tmp/LOANI-testing
	@cp -f $(LOANI_ZIP) $(LOANI_TGZ) $(HOME)/tmp/LOANI-testing
	@cd $(HOME)/tmp/LOANI-testing; unzip -qq $(LOANI_ZIP)
	@echo "You can type now : cd $(HOME)/tmp/LOANI-testing;\
	loani.sh -d --sourceforge <your user>"


info-local:
	@echo "LOANI_VERSION       = $(LOANI_VERSION)"
	@echo "LOANI_ZIP           = $(LOANI_ZIP)" 
	@echo "LOANI_TGZ           = $(LOANI_TGZ)"
	@echo "LOANI_FELLOWS_FILES = $(LOANI_FELLOWS_FILES)"
	@echo "Ceylan release      = $(CEYLAN_RELEASE)"
	@echo "OSDL release        = $(OSDL_RELEASE)"
	

stat:
	@echo "Newline, word, and byte counts for LOANI :"
	@cat $(LOANI_FELLOWS_FILES) | wc
		
		
clean-local:
	@echo "@OSDL_CLEAN_STYLE@    Cleaning all in `pwd`@OSDL_DEFAULT_STYLE@"
	-@rm -f wget-log* LOANI.log $(LOANI_ZIP) $(LOANI_TGZ)\
	$(LOANI_README_TARGET) $(LOANI_TOOLS_SETTINGS)
	-@rm -rf LOANI-$(LOANI_VERSION)
	
	
realclean : clean-local
	@echo "@OSDL_CLEAN_STYLE@    Real cleaning all in `pwd`@OSDL_DEFAULT_STYLE@"
	-@rm -rf $(LOANI_REPOSITORY_DIR) $(LOANI_INSTALL_DIR)	
	-@rm -f  $(LOANI_TGZ) $(LOANI_ZIP) 
	

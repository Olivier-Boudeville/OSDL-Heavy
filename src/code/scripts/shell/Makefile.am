ARCHIVE_DIR  = @OSDL_ARCHIVE_DIR@/bin
ARCHIVE_FILE = "$(ARCHIVE_DIR)/`date '+%Y%m%d'`-Full-archive-OSDL-bin.tar.bz2" 

SUBDIRS =

# These are the subdirectories which will be distributed with 'make dist' :
DIST_SUBDIRS = $(SUBDIRS)


SHELL_SCRIPTS_SET = \
	create-OSDL-archive.sh                 \
	extract-OSDL-archive.sh                \
	generate-identifier-list.sh            \
	process-reiner-content.sh              \
	process-reiner-individual-archive.sh   \
	retrieve-reiner-content.sh             \
	sort-reiner-mirror.sh

        
EXTRA_DIST = $(SHELL_SCRIPTS_SET)		 
                  
all: 

install-data-local:
	@echo; echo "@OSDL_INSTALL_STYLE@    Installing shell scripts in \
	$(srcdir) to $(DESTDIR)@OSDL_INSTALL_SHELL_DIR@ @OSDL_DEFAULT_STYLE@"
	@mkdir -p $(DESTDIR)@OSDL_INSTALL_SHELL_DIR@
	@for f in $(SHELL_SCRIPTS_SET); do \
	cp -f $(srcdir)/$$f $(DESTDIR)@OSDL_INSTALL_SHELL_DIR@/$$f; done


tar:
	@mkdir -p $(ARCHIVE_DIR)
	@tar -cvf - * 2>/dev/null | bzip2 > $(ARCHIVE_FILE)
	@echo "@OSDL_INSTALL_STYLE@    All bin scripts have been saved in \
	$(ARCHIVE_FILE)."


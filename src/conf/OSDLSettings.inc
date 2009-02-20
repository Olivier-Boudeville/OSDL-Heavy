
# Basic settings for the OSDL project.

# This is a source file that can be modified to customize settings prior to
# the whole 'autogen.sh' process, whose beginning is the generation of 
# 'configure.ac' from the settings defined here and the template
# 'configure-template.ac'.

# More precisely, due to Autoconf limitations (no variable substitution in
# AC_INIT, AC_CONFIG_SRCDIR, AC_CONFIG_HEADERS, etc.) we had to create
# 'configure-template.ac', a template for the expected 'configure.ac', made to
# be filled with the values from this 'OSDLSettings.inc' file.
# It is a means of preserving one-time assignments of key variables: we 
# believe that duplication of value definitions leads to nasty pitfalls.

# The allowed syntax in this file is the one of the Makefile, since this
# settings file will be only included in 'MakeConfigure' to guide the
# substitution process: the ST_* Substitution Targets tags written in
# 'configure-template.ac' will be replaced by their associated value defined
# here, and the result will be the expected 'configure.ac' filled appropriately.  
# This settings file is only to be sourced for the substitution process, it
# should not be used afterwards in the building process. Hence no Makefile
# should include it. The proper way for them to access these settings is
# to use Autoconf variables (ex: @HELLO@). 


# Filenames are relative to the 'trunk/src' directory.


# Naming section.

# (tag: ST_PROJECT_NAME)
PROJECT_NAME=OSDL

# (tag: ST_UNIX_PROJECT_NAME)
UNIX_PROJECT_NAME=osdl


# OSDL version numbers for current release.

# Actual release date.

# Full actual release date (not generated for better control).
# (quote protection is necessary)
# (tag: ST_RELEASE_DATE)
OSDL_RELEASE_DATE='"Saturday, February 14, 2009"'

# Usual versioning. 

# (tag: ST_MAJOR_VERSION)
OSDL_MAJOR_VERSION=0

# (tag: ST_MINOR_VERSION)
OSDL_MINOR_VERSION=5

# Release number is only for project fine grain milestone,
# for a given major.minor version (ex: release candidate).
# (tag: ST_RELEASE)
OSDL_RELEASE=0


# (tag: ST_VERSION)
OSDL_VERSION=$(OSDL_MAJOR_VERSION).$(OSDL_MINOR_VERSION)

# (tag: ST_FULL_VERSION)
OSDL_FULL_VERSION=$(OSDL_VERSION).$(OSDL_RELEASE)


# Libtool-style versioning.

# When making releases, modify these version numbers according to 
# Libtool-defined rules, as described in:
# http://www.gnu.org/software/libtool/manual.html#Updating-version-info

# Starting from latest public release, labelled 'major:minor:ancestor', version
# becomes:
# (all matching rules should be applied, in turn)
#   1. If the implementation (the source code) changed, then increment minor
#   2. If the API changed (any interface added, removed, or changed), then
# increment major, and set minor to zero (i.e. go to the next major)
#   3. If any interface has been removed or has its contract changed, 
# set ancestor to major


# Ancestor corresponds to the first major version whose API is a subset of the 
# current one, i.e. starting from this major version, any later one can be used
# instead. Libtool age corresponds to: major minus ancestor.
# (tag: ST_MAJOR_ANCESTOR)
OSDL_MAJOR_ANCESTOR=0



# Build section.

# (tag: ST_CONFIG_HEADER).
OSDL_CONFIG_HEADER="src/code/OSDLTemporaryConfig.h"


# Support section.

# (tag: ST_MAILING_LIST_SUPPORT)
OSDL_MAILING_LIST_SUPPORT="osdl-support@lists.sourceforge.net"

# (tag: ST_MAILING_LIST_BUGTRACKING)
OSDL_MAILING_LIST_BUGTRACKING="osdl-bugs@lists.sourceforge.net"

# (tag: ST_MAILING_LIST_LOANI)
OSDL_MAILING_LIST_LOANI="osdl-loani-bugs@lists.sourceforge.net"



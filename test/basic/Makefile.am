## Tests for the 'Basic' module of the OSDL library.


testsbasicdir=@OSDL_TEST_DIR@/basic

testsbasic_CXXFLAGS = @AM_CXXFLAGS@

testsbasic_PROGRAMS = \
	testBasic.exe                               \
	testOSDLBasic.exe                           \
	testOSDLCDROMDrive.exe                      \
	testOSDLEmbeddedFileSystem.exe              \
	testOSDLException.exe                       \
	testOSDLGUI.exe                             \
	testOSDLUtils.exe                           \
	testSDL.exe                                 \
	testSDL_gfx.exe                             \
	testSDL_image.exe                           \
	testSDL_mixer.exe                           \
	testSDLWithOpenGL.exe
	

testBasic_exe_SOURCES                        = testBasic.cc
testOSDLBasic_exe_SOURCES                    = testOSDLBasic.cc
testOSDLCDROMDrive_exe_SOURCES               = testOSDLCDROMDrive.cc
testOSDLEmbeddedFileSystem_exe_SOURCES       = testOSDLEmbeddedFileSystem.cc
testOSDLException_exe_SOURCES                = testOSDLException.cc
testOSDLGUI_exe_SOURCES                      = testOSDLGUI.cc
testOSDLUtils_exe_SOURCES                    = testOSDLUtils.cc
testSDL_exe_SOURCES                          = testSDL.cc
testSDL_gfx_exe_SOURCES                      = testSDL_gfx.cc
testSDL_image_exe_SOURCES                    = testSDL_image.cc
testSDL_mixer_exe_SOURCES                    = testSDL_mixer.cc
testSDLWithOpenGL_exe_SOURCES                = testSDLWithOpenGL.cc


clean: clean-local


clean-local:
	-@/bin/rm -f resource-map.h resource-map.xml cypherOSDLFile.exe.log
	

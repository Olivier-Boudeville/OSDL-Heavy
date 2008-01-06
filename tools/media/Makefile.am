## OSDL media-related tools.


mediatoolsdir=@OSDL_TOOL_DIR@/media

mediatools_CXXFLAGS = @AM_CXXFLAGS@


mediatools_PROGRAMS = \
	wavToOSDLSound.exe   \
	mp3ToOSDLMusic.exe   \
	identifyOSDLFile.exe
	

wavToOSDLSound_exe_SOURCES   = wavToOSDLSound.cc
mp3ToOSDLMusic_exe_SOURCES   = mp3ToOSDLMusic.cc
identifyOSDLFile_exe_SOURCES = identifyOSDLFile.cc


clean:clean-local

clean-local: 
	@rm -f *.osdl.sound *.osdl.music *.log

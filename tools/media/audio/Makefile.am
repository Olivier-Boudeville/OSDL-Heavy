## OSDL audio-related media tools.


audiotoolsdir = @OSDL_TOOL_DIR@/media/audio

audiotools_CXXFLAGS = @AM_CXXFLAGS@


audiotools_PROGRAMS = \
	wavToOSDLSound.exe   \
	mp3ToOSDLMusic.exe


wavToOSDLSound_exe_SOURCES = wavToOSDLSound.cc
mp3ToOSDLMusic_exe_SOURCES = mp3ToOSDLMusic.cc


clean-local:
	@/bin/rm -f *.log

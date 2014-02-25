SET (IIMA_LINK )
SET (IIMA_LIBS )
SET (IIMA_INCLUDE )

SET (IIMA_SRC Utils.cpp AudioTypes.cpp AudioFilter.cpp AudioSink.cpp
				WaveFile.cpp WaveSource.cpp WaveSink.cpp
				filters/SineMultiply.cpp filters/NullFilter.cpp 
				filters/SimpleEchoFilter.cpp
				video_ops.cpp
				
				
				../include/iimavlib.h ../include/iimavlib/Utils.h ../include/iimavlib/AudioTypes.h 
				../include/iimavlib/AudioFilter.h ../include/iimavlib/AudioSink.h
				../include/iimavlib/WaveFile.h ../include/iimavlib/WaveSource.h ../include/iimavlib/WaveSink.h
				../include/iimavlib/filters/SineMultiply.h ../include/iimavlib/filters/NullFilter.h 
				../include/iimavlib/filters/SimpleEchoFilter.h
				../include/iimavlib/video_types.h ../include/iimavlib/video_ops.h
				)
IF (UNIX)
SET(IIMA_SRC ${IIMA_SRC} AlsaDevice.cpp AlsaSink.cpp AlsaSource.cpp
				../include/iimavlib/AlsaDevice.h ../include/iimavlib/AlsaSink.h ../include/iimavlib/AlsaSource.h)
SET(IIMA_LIBS ${IIMA_LIBS} asound)
ELSEIF(WIN32)
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp WinMMSink.cpp WinMMSource.cpp
		../include/iimavlib/WinMMDevice.h ../include/iimavlib/WinMMSink.h ../include/iimavlib/WinMMSource.h)
ELSEIF(CYGWIN)
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp WinMMSink.cpp WinMMSource.cpp
		../include/iimavlib/WinMMDevice.h ../include/iimavlib/WinMMSink.h ../include/iimavlib/WinMMSource.h)
ELSE()
MESSAGE("Unknown platform")
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp WinMMSink.cpp WinMMSource.cpp
		../include/iimavlib/WinMMDevice.h ../include/iimavlib/WinMMSink.h ../include/iimavlib/WinMMSource.h)
ENDIF()

IF (SDL_FOUND)
	SET(IIMA_SRC ${IIMA_SRC} SDLDevice.cpp ../include/iimavlib/SDLDevice.h)
	SET(IIMA_LIBS ${IIMA_LIBS} ${SDL_LIBRARY})
	SET(IIMA_INCLUDE ${IIMA_INCLUDE} ${SDL_INCLUDE_DIR})
ENDIF()

add_library(iimav ${IIMA_SRC})
link_directories ( ${IIMA_LINK} )
include_directories ( ${IIMA_INCLUDE} )
target_link_libraries ( iimav ${IIMA_LIBS})

SET_TARGET_PROPERTIES( iimav PROPERTIES SOVERSION 1 VERSION 1.1.0 )
install(TARGETS iimav 		RUNTIME DESTINATION bin
							LIBRARY DESTINATION lib 
							ARCHIVE DESTINATION lib)

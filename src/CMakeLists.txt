SET (IIMA_LINK )
SET (IIMA_LIBS )
SET (IIMA_INCLUDE )

SET (IIMA_SRC Utils.cpp AudioTypes.cpp AudioFilter.cpp AudioSink.cpp
				WaveFile.cpp WaveSource.cpp WaveSink.cpp
				filters/SineMultiply.cpp filters/NullFilter.cpp filters/SimpleEchoFilter.cpp
				
				)


IF (UNIX)
SET(IIMA_SRC ${IIMA_SRC} AlsaDevice.cpp AlsaSink.cpp AlsaSource.cpp)
SET(IIMA_LIBS ${IIMA_LIBS} asound)
ELSEIF(WIN32)
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp WinMMSink.cpp WinMMSource.cpp)
ELSEIF(CYGWIN)
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp WinMMSink.cpp WinMMSource.cpp)
ELSE()
MESSAGE("Unknown platform")
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp WinMMSink.cpp WinMMSource.cpp)
ENDIF()

IF (SDL_FOUND)
	SET(IIMA_SRC ${IIMA_SRC} SDLDevice.cpp)
	SET(IIMA_LIBS ${IIMA_LIBS} ${SDL_LIBRARY})
	SET(IIMA_INCLUDE ${IIMA_INCLUDE} ${SDL_INCLUDE_DIR})
ENDIF()

add_library(iimav ${IIMA_SRC})
link_directories ( ${IIMA_LINK} )
include_directories ( ${IIMA_INCLUDE} )
target_link_libraries ( iimav ${IIMA_LIBS})

SET_TARGET_PROPERTIES( iimav PROPERTIES SOVERSION 1 VERSION 0.9.0 )
install(TARGETS iimav 		RUNTIME DESTINATION bin
							LIBRARY DESTINATION lib 
							ARCHIVE DESTINATION lib)

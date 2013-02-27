SET (IIMA_LINK )
SET (IIMA_LIBS )
SET (IIMA_INCLUDE )

SET (IIMA_SRC Utils.cpp WaveFile.cpp AudioTypes.cpp)


#CHECK_INCLUDE_FILES (stdint.h HAVE_STDINT_H)
#IF (NOT HAVE_STDINT_H)
#	MESSAGE(FATAL_ERROR "Missing stdint.h. Please update your compile chain")
#ENDIF()

IF (UNIX)
SET(IIMA_SRC ${IIMA_SRC} AlsaDevice.cpp)
SET(IIMA_LIBS ${IIMA_LIBS} asound)
ELSEIF(WIN32)
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp)
ELSEIF(CYGWIN)
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp)
ELSE()
MESSAGE("Unknown platform")
SET(IIMA_SRC ${IIMA_SRC} WinMMDevice.cpp)
ENDIF()

IF (SDL_FOUND)
	SET(IIMA_SRC ${IIMA_SRC} SDLDevice.cpp)
	SET(IIMA_LIBS ${IIMA_LIBS} ${SDL_LIBRARY})
	SET(IIMA_INCLUDE ${IIMA_INCLUDE} ${SDL_INCLUDE_DIR})
ENDIF()

add_library(iimavlib ${IIMA_SRC})
link_directories ( ${IIMA_LINK} )
include_directories ( ${IIMA_INCLUDE} )
target_link_libraries ( iimavlib ${IIMA_LIBS})

SET_TARGET_PROPERTIES( iimavlib PROPERTIES SOVERSION 1 VERSION 0.9.0 )
install(TARGETS iimavlib 	RUNTIME DESTINATION bin
							LIBRARY DESTINATION lib 
							ARCHIVE DESTINATION lib)

SET (IIMA_LINK )
SET (IIMA_LIBS )
SET (IIMA_INCLUDE )

SET (IIMA_SRC Utils.cpp)


#CHECK_INCLUDE_FILES (stdint.h HAVE_STDINT_H)
#IF (NOT HAVE_STDINT_H)
#	MESSAGE(FATAL_ERROR "Missing stdint.h. Please update your compile chain")
#ENDIF()

IF (UNIX)
SET(IIMA_SRC ${IIMA_SRC} AlsaDevice.cpp WaveFile.cpp AudioTypes.cpp)
SET(IIMA_LIBS ${IIMA_LIBS} asound)
ENDIF()


add_library(iimaudio ${IIMA_SRC})
link_directories ( ${IIMA_LINK} )
#include_directories ( ${IIMA_INCLUDE} )
target_link_libraries ( iimaudio ${IIMA_LIBS})

SET_TARGET_PROPERTIES( iimaudio PROPERTIES SOVERSION 1 VERSION 0.1.0 )
install(TARGETS iimaudio 	RUNTIME DESTINATION bin
							LIBRARY DESTINATION lib 
							ARCHIVE DESTINATION lib)

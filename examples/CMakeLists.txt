IF (BUILD_EXAMPLES)
		
	SET(EX_LIBS iimav)
    
	add_executable(enumerate_devices enumerate_devices.cpp)
	target_link_libraries ( enumerate_devices  ${EX_LIBS} )
	install(TARGETS enumerate_devices RUNTIME DESTINATION bin)
	
	add_executable(playback_wav playback_wav.cpp)
	target_link_libraries ( playback_wav  ${EX_LIBS} )
	install(TARGETS playback_wav RUNTIME DESTINATION bin)
	
	add_executable(playback_sine playback_sine.cpp)
	target_link_libraries (playback_sine  ${EX_LIBS} )
	install(TARGETS playback_sine RUNTIME DESTINATION bin)

	add_executable(copy_wav copy_wav.cpp)
	target_link_libraries ( copy_wav  ${EX_LIBS} )
	install(TARGETS copy_wav RUNTIME DESTINATION bin)
	
	add_executable(playthrough playthrough.cpp)
	target_link_libraries ( playthrough  ${EX_LIBS} )
	install(TARGETS playthrough RUNTIME DESTINATION bin)
	
	IF(SDL_FOUND)
		include_directories(${SDL_INCLUDE_DIR})
		add_executable(sdl_window sdl_window.cpp)
		target_link_libraries ( sdl_window  ${EX_LIBS} )
		install(TARGETS sdl_window RUNTIME DESTINATION bin)

		add_executable(visualization visualization.cpp)
		target_link_libraries ( visualization  ${EX_LIBS} )
		install(TARGETS visualization RUNTIME DESTINATION bin)

		add_executable(sdl_transform sdl_transform.cpp)
		target_link_libraries ( sdl_transform  ${EX_LIBS} )
		install(TARGETS sdl_transform RUNTIME DESTINATION bin)
		
		add_executable(sdl_cellular_automata sdl_cellular_automata.cpp)
		target_link_libraries ( sdl_cellular_automata  ${EX_LIBS} )
		install(TARGETS sdl_cellular_automata RUNTIME DESTINATION bin)
		
		add_executable(sdl_drums sdl_drums.cpp)
		target_link_libraries ( sdl_drums  ${EX_LIBS} )
		install(TARGETS sdl_drums RUNTIME DESTINATION bin)
	ENDIF()			
ENDIF(BUILD_EXAMPLES)

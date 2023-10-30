IF (BUILD_EXAMPLES)
		
	SET(EX_LIBS iimav)
    
	add_executable(enumerate_devices enumerate_devices.cpp)
	target_link_libraries ( enumerate_devices  ${EX_LIBS} )
	install(TARGETS enumerate_devices RUNTIME DESTINATION bin)

	add_executable(enumerate_midi_devices enumerate_midi_devices.cpp)
	target_link_libraries ( enumerate_midi_devices ${EX_LIBS} )
	install(TARGETS enumerate_midi_devices RUNTIME DESTINATION bin)
	
	add_executable(playback_wav playback_wav.cpp)
	target_link_libraries ( playback_wav  ${EX_LIBS} )
	install(TARGETS playback_wav RUNTIME DESTINATION bin)
	
	add_executable(record_wav record_wav.cpp)
	target_link_libraries (record_wav  ${EX_LIBS} )
	install(TARGETS record_wav RUNTIME DESTINATION bin)
	
	add_executable(playback_sine playback_sine.cpp)
	target_link_libraries (playback_sine  ${EX_LIBS} )
	install(TARGETS playback_sine RUNTIME DESTINATION bin)

	add_executable(copy_wav copy_wav.cpp)
	target_link_libraries ( copy_wav  ${EX_LIBS} )
	install(TARGETS copy_wav RUNTIME DESTINATION bin)
	
	add_executable(playthrough playthrough.cpp)
	target_link_libraries ( playthrough  ${EX_LIBS} )
	install(TARGETS playthrough RUNTIME DESTINATION bin)
	
	add_executable(empty_project empty_project.cpp)
	target_link_libraries (empty_project  ${EX_LIBS} )
	install(TARGETS empty_project RUNTIME DESTINATION bin)
	
	add_executable(sdl_spectrum sdl_spectrum.cpp)
	target_link_libraries (sdl_spectrum  ${EX_LIBS} )
	install(TARGETS sdl_spectrum RUNTIME DESTINATION bin)
	
	add_executable(midi_sine midi_sine.cpp)
	target_link_libraries (midi_sine  ${EX_LIBS} )
	install(TARGETS playback_sine RUNTIME DESTINATION bin)
	
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
		
		add_executable(cellular_automata cellular_automata.cpp)
		target_link_libraries ( cellular_automata  ${EX_LIBS} )
		install(TARGETS cellular_automata RUNTIME DESTINATION bin)
		
		add_executable(sdl_drums sdl_drums.cpp)
		target_link_libraries ( sdl_drums  ${EX_LIBS} )
		install(TARGETS sdl_drums RUNTIME DESTINATION bin)
		
		add_executable(sdl_frequency sdl_frequency.cpp)
		target_link_libraries (sdl_frequency  ${EX_LIBS} )
		install(TARGETS sdl_frequency RUNTIME DESTINATION bin)
		
		add_executable(sdl_drums_artnet sdl_drums_artnet.cpp)
		target_link_libraries ( sdl_drums_artnet  ${EX_LIBS} )
		install(TARGETS sdl_drums_artnet RUNTIME DESTINATION bin)
		
		add_executable(sdl_drums_midi sdl_drums_midi.cpp)
		target_link_libraries ( sdl_drums_midi  ${EX_LIBS} )
		install(TARGETS sdl_drums_midi RUNTIME DESTINATION bin)

		add_executable(sdl_sequencer sdl_sequencer.cpp)
		target_link_libraries ( sdl_sequencer ${EX_LIBS} )
		install(TARGETS sdl_sequencer RUNTIME DESTINATION bin)
	ENDIF()			
ENDIF(BUILD_EXAMPLES)

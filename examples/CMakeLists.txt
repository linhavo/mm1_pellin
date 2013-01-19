IF (BUILD_EXAMPLES)
#		find_package( SDL )

#		find_package( Boost 1.46 COMPONENTS program_options timer )
#		IF (Boost_TIMER_FOUND)
#			add_definitions(-DMVTP_HAVE_BOOST_TIMER)
#		ELSE()
#			find_package( Boost 1.46 COMPONENTS program_options )
#		ENDIF (Boost_TIMER_FOUND)
#		IF(Boost_PROGRAM_OPTIONS_FOUND)
#			add_definitions(-DMVTP_HAVE_BOOST_PROGRAM_OPTIONS)
#		ENDIF(Boost_PROGRAM_OPTIONS_FOUND)
#		IF (Boost_FOUND)
			
		SET(EX_LIBS iimaudio) 
		add_executable(capture_test capture_test.cpp)
		target_link_libraries ( capture_test  ${EX_LIBS} )
		install(TARGETS capture_test RUNTIME DESTINATION bin)
		
		add_executable(playback_test playback_test.cpp)
		target_link_libraries ( playback_test  ${EX_LIBS} )
		install(TARGETS playback_test RUNTIME DESTINATION bin)
			
ENDIF(BUILD_EXAMPLES)
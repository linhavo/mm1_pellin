SET(EX_LIBS iimav)

add_executable(test_iimavlib
		test_main.cpp
		test_matrix.cpp
		test_fft.cpp
		)
target_link_libraries ( test_iimavlib  ${EX_LIBS} )
#install(TARGETS enumerate_devices RUNTIME DESTINATION bin)


include_directories(
	${SOLUTION_INCLUDE_DIRS}
	/usr/local/include
)
link_directories(
	/usr/local/lib
)

add_executable(test_tcp
    test_tcp.cpp
)

target_link_libraries(test_tcp common)

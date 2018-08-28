include_directories(
	${SOLUTION_INCLUDE_DIRS}
	/usr/local/include
)
link_directories(
	/usr/local/lib
)

add_library(tcp_client SHARED
	tcp_client.h
	tcp_client.cpp
)

add_executable(test_tcp
    test_tcp.cpp
)

target_link_libraries(tcp_client scy_base scy_net libuv jsoncpp)

target_link_libraries(test_tcp tcp_client)

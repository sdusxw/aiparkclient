
add_library(common
	common.h
	common.cpp
	net_udp.h
	net_udp.cpp
	net_tcp.h
	net_tcp.cpp
)

target_link_libraries(common pthread jpeg)


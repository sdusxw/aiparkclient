include_directories(
	${SOLUTION_INCLUDE_DIRS}
	/usr/local/include
)
link_directories(
	/usr/local/lib
)

add_executable(aipayclient
	aipayclient.h
	aipayclient.cpp
)

target_link_libraries(aipayclient pthread common jsoncpp)


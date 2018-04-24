if (BUILD_CLIENT)
	set(client_testutil_SRCS
		${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
		PARENT_SCOPE
	)
endif()


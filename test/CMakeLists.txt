cmake_minimum_required(VERSION 3.20.0)
project(Test_${PROJECT_NAME} VERSION 0.1 DESCRIPTION "")


set(DEPENDENCIES)
#add_library(${PROJECT_NAME} OBJECT)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
#string(REPLACE "Test_" "" PARENT_NAME ${PROJECT_NAME})
#file(GLOB INC ${CMAKE_CURRENT_SOURCE_DIR}/inc/${PARENT_NAME}/Test/*.hpp)
#file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
#target_sources(${PROJECT_NAME} PUBLIC ${INC} PRIVATE ${SRC})


if (DEPENDENCIES)
	foreach (T IN LISTS DEPENDENCIES)
		if (NOT TARGET ${T})
			if (T MATCHES "^Test_")
				string(REPLACE "Test_" "" T T)
				add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../${T}/test ${CMAKE_CURRENT_BINARY_DIR}/../../${T}/Testing)
			else()
				add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../${T} ${CMAKE_CURRENT_BINARY_DIR}/../../${T})
			endif()
		endif()
	endforeach()
	target_link_libraries(${PROJECT_NAME} PUBLIC ${DEPENDENCIES})
endif (DEPENDENCIES)


file(GLOB Classes RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/[A-Z]*)
foreach (Class ${Classes})
	if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${Class})
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${Class} ${CMAKE_CURRENT_BINARY_DIR}/${Class})
	endif()
endforeach()

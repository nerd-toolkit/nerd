
cmake_minimum_required(VERSION 2.6)



if(${CMAKE_COMPILER_IS_GNUCXX})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wshadow")
#message(${CMAKE_CXX_FLAGS})
endif()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-E")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

add_subdirectory(nerd)
add_subdirectory(extensions)

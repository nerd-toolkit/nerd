
cmake_minimum_required(VERSION 2.6)



if(${CMAKE_COMPILER_IS_GNUCXX})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wshadow")
#message(${CMAKE_CXX_FLAGS})
endif()

set(CMAKE_BUILD_TYPE debug)

add_subdirectory(nerd)
add_subdirectory(extensions)

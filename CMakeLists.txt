cmake_minimum_required ( VERSION 2.8 )

project ( mysrv )

set (CMAKE_CXX_FLAGS "$ENV{CXXFLAG} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

add_subdirectory ( src )



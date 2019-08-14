
aux_source_directory(. SRC_LIST)

add_library(mysrv SHARED ${SRC_LIST})
add_executable(main ${SRC_LIST})

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAG} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

add_dependencies(main mysrv)
target_link_libraries(main mysrv)


#首先就是EXECUTEABLE_OUTPUT_PATH是目标二进制存放的位置
#其次就是PROJECT_SOURCE_DIR是工程的根目录

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

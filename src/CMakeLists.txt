add_definitions(-std=c++11 ${CMAKE_OPTION})
set(CMAKE_EXE_LINKER_FLAGS "-Wall -lpthread")

find_library(YMALCPP libyaml-cpp.a)

aux_source_directory(. DIR_SRC)
include_directories(
        ./
        /home/mjf/lib/include/
        ../conf/yaml-cpp/include/
        )

add_executable(mysrv  ${DIR_SRC})

target_link_libraries(mysrv  yaml-cpp) #添加libmxnet.so

#target_link_libraries(mysrv ${CMAKE_EXE_LINKER_FLAGS})

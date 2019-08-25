add_definitions(-std=c++11 ${CMAKE_OPTION})
set(CMAKE_EXE_LINKER_FLAGS "-Wall -lpthread")

aux_source_directory(. DIR_SRC)
include_directories(
        ./
        )

add_executable(mysrv  ${DIR_SRC})
#target_link_libraries(mysrv ${CMAKE_EXE_LINKER_FLAGS})

cmake_minimum_required ( VERSION 2.8 )

project ( mysrv )
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
    set(CMAKE_OPTION "$ENV{MJOY_OPTION}  -Werror  -rdynamic  -o0 ")
    set(LIB_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib  ${CMAKE_CURRENT_SOURCE_DIR}/conf/yaml-cpp/build )
    set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
    set(PRO_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(INCLUDE_DIR
          /home/mjf/lib/include/
        /home/mysrv/mysrv/conf/yaml-cpp/include/)

    link_directories(
    ${INCLUDE_DIR}
    #${PRO_DIR}
    ${LIB_DIR}
       )

     set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})
     set(LIBRARY_OUTPUT_PATH ${LIB_DIR})
     add_subdirectory ( src )


     message("")
     message("====================================")
     message("   compile path:")
     message("src        = Loading")
     message("   compile choice:")
     message(" BUILD_TYPE    =" ${CMAKE_BUILD_TYPE})
     message(" CMAKE_OPTION  =" ${CMAKE_OPTION})

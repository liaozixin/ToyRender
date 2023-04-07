cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

install(DIRECTORY ./Include
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT vulkancomponent)

install(DIRECTORY ./Lib
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT vulkancomponent)

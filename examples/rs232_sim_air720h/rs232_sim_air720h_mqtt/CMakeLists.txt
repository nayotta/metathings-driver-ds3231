cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../components ${CMAKE_CURRENT_SOURCE_DIR}/../../../mt_proto ${CMAKE_CURRENT_SOURCE_DIR}/../../../mt_apps ") 

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(mt_sim_air720h_mqtt)

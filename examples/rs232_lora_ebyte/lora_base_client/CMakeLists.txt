# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# (Not part of the boilerplate)
# This example uses an extra component for common functions such as Wi-Fi and Ethernet connection.
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../components ${CMAKE_CURRENT_SOURCE_DIR}/../../../mt_proto ${CMAKE_CURRENT_SOURCE_DIR}/../../../mt_apps") 

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(lora_push_frame_client)

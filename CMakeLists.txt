cmake_minimum_required(VERSION 3.15)
include(${CMAKE_CURRENT_SOURCE_DIR}/resources/pd.build/pd.cmake) # pd.build
project(saturator)

#╭──────────────────────────────────────╮
#│           PureData Object            │
#╰──────────────────────────────────────╯

set_pd_external_path(${CMAKE_CURRENT_SOURCE_DIR}/saturator~)
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_pd_external(pd-saturator saturator~ ${SOURCES})
add_definitions(-DPD)
set_target_properties(pd-saturator PROPERTIES CXX_STANDARD 11)






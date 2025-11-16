cmake_minimum_required(VERSION 3.5)

if(DEFINED ENV{RMAKER_PATH})
  set(RMAKER_PATH $ENV{RMAKER_PATH})
else()
  set(RMAKER_PATH ${CMAKE_CURRENT_LIST_DIR}/../..)
endif()

set(EXTRA_COMPONENT_DIRS ${RMAKER_PATH}/examples/common)

set(PROJECT_VER "1.0")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(PlantWatering)

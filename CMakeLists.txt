##**************************************************
## \file CMakeLists.txt
## \brief: CMakeLists of G4DREMTubes project
## \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
## \start date: 7 July 2021
##**************************************************

# Setup the project
#
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(G4DREMTubes)

# Find Geant4 package, activating all available UI and Vis drivers by default
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# Setup Geant4 include directories and prohect include directories
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Locate project source and headers
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Add the executable and link it to the Geant4 libraries
#
add_executable(DREMTubes DREMTubes.cc ${sources} ${headers})
target_link_libraries(DREMTubes ${Geant4_LIBRARIES})

# Copy all scripts to the build directory 
#
set(DREMTubes_SCRIPTS
  exampleB4a.out
  exampleB4.in
  icons.mac
  gui.mac
  run.png
  init_vis.mac
  run1.mac
  run2.mac
  vis.mac
  )

foreach(_script ${DREMTubes_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#For internal Geant4 use 
#
add_custom_target(G4DREMTubes DEPENDS G4EMTubes)

# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DREMTubes DESTINATION bin)

##**************************************************

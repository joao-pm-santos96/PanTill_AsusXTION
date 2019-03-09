cmake_minimum_required (VERSION 2.8)
project (Projeto_PARI_2018-19)
set(PROG ptu_xtion)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

set(Extra_Warns "-Wall")
#use the next one if you want to see more things, for exemple, unused variables
#set(Extra_Warns "-Wall -Wextra")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${Extra_Warns}")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${Extra_Warns}")

#OpenCV
find_package( OpenCV 3.1 REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

#GTK
find_package(PkgConfig REQUIRED)  #necessary for next command to work
pkg_check_modules(GTK REQUIRED gtk+-3.0)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

#OpenNI2
find_package(OpenNI2 REQUIRED)
link_directories(${OPENNI2_INCLUDE_DIRS})
include_directories(${OPENNI2_INCLUDE_DIRS})

#Biblioteca PARI
set(pari_bbl_src pari_bbl/comms.cpp pari_bbl/gtk_update.cpp pari_bbl/openni2.cpp pari_bbl/ptu.cpp pari_bbl/callbacks.c)
SET_SOURCE_FILES_PROPERTIES( ${pari_bbl_src} PROPERTIES LANGUAGE CXX )

add_library(pari_bbl SHARED ${pari_bbl_src})
target_link_libraries(pari_bbl m ${OpenCV_LIBS} ${GTK_LIBRARIES} ${OPENNI2_LIBRARIES})

#Programa
set(SOURCES src/main.cpp)
include_directories ("${PROJECT_SOURCE_DIR}/pari_bbl")
add_executable(${PROG} ${SOURCES})
SET_SOURCE_FILES_PROPERTIES( ${SOURCES} PROPERTIES LANGUAGE CXX )

#Linkage
target_link_libraries(${PROG} m ${OpenCV_LIBS} ${GTK_LIBRARIES} ${OPENNI2_LIBRARIES} pari_bbl)

#Targets and Dependencies - cpfiles
add_custom_target( cpfiles
                COMMAND cp "${PROJECT_SOURCE_DIR}/src/*.glade" "${PROJECT_BINARY_DIR}" )

add_dependencies(${PROG} cpfiles)

#Targets and Dependencies - proto
add_custom_target(proto
                  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/pari_bbl"
                  COMMAND sudo ./genproto2.sh
)


#Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install doc (requires Doxygen)" ${DOXYGEN_FOUND})
if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()
    set(doxyfile_in "${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in" )
    set(doxyfile "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    set(doxy_main_page "${CMAKE_CURRENT_SOURCE_DIR}/doc/mainpage.dox")
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()
#code inspired on: http://www.tty1.net/blog/2014/cmake-doxygen_en.html

#Instalation
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")

#install(DIRECTORY ${PROJECT_SOURCE_DIR}/OpenNI2/lib/
#        DESTINATION lib
#)

install(FILES "${PROJECT_SOURCE_DIR}/src/UI.glade"
        DESTINATION bin
)

install(TARGETS ${PROG} pari_bbl
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
)

add_custom_target(libpath
        COMMAND sudo ldconfig "${PROJECT_SOURCE_DIR}/lib"
)
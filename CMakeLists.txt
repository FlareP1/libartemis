# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.9)
#CMake instructions to make the static lib

#project(libartemis)
project(artemis VERSION 0.1.0 DESCRIPTION "Artemis CCD Driver")
set(CMAKE_BUILD_TYPE Debug)

message(${PROJECT_SOURCE_DIR})

#Add the compile defines required by the wxWidgets library
add_definitions(-DNOPCH -D__WXDEBUG__ -D_DEBUG -D__WX__ )
#Set Compiler Flags
# from wx-config --version=2.8 --toolkit=gtk2 --debug --cxxflags
SET(GCC_COVERAGE_COMPILE_FLAGS "-I${PROJECT_SOURCE_DIR}/ -I.${PROJECT_SOURCE_DIR}/wxIO -I${PROJECT_SOURCE_DIR}/include -I${PROJECT_SOURCE_DIR}/../wxWidgets-2.8.12/include/ -I${PROJECT_SOURCE_DIR}/../wxWidgets-2.8.12/build-debug/lib/wx/include/gtk2-ansi-debug-2.8/ -I/usr/local/lib/wx/include/gtk2-ansi-debug-2.8 -I/usr/local/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXDEBUG__ -D__WXGTK__ -pthread ")
# from wx-config  --version=2.8 --toolkit=gtk2 --debug --libs
SET(GCC_COVERAGE_LINK_FLAGS  "-L{PROJECT_SOURCE_DIR}/../wxWidgets-2.8.12/build-debug/lib -L/usr/local/lib -pthread   -lwx_gtk2d_richtext-2.8 -lwx_gtk2d_aui-2.8 -lwx_gtk2d_xrc-2.8 -lwx_gtk2d_qa-2.8 -lwx_gtk2d_html-2.8 -lwx_gtk2d_adv-2.8 -lwx_gtk2d_core-2.8 -lwx_based_xml-2.8 -lwx_based_net-2.8 -lwx_based-2.8 -lusb")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

#However, the file(GLOB...) allows for wildcard additions:

#Can manually add the sources using the set command as follows:
set(MAIN_SOURCES 
ArtBase.cpp
ArtDevice.cpp
ArtDrvSim.cpp
ArtemisCCDAPI.cpp
ArtTimeHelper.cpp
ARTUSB_LibUsb.cpp
Atik16Cmd.cpp
Atik16Drv.cpp
Atik16Thread.cpp
Ftd245IO.cpp
Ftd245IOUX.cpp
Ftd245Lib.cpp
GenericImageMem.cpp
wxArtSample.cpp
wxBMP2Fits.cpp
wxSimpleImageStore.cpp
wxYP162Fits.cpp
#yuv_to_rgb.cpp
include/GenericDebHelp.cpp
)
set(WXIO_SOURCES 
 wxIO/UxUsb_LibUsb.cpp 
 wxIO/IOBuffer.cpp )
set(FITS_SOURCES 
 fits/wxFitsOut.cpp
 fits/wxFitsKeyword.cpp 
 fits/wxFitsOutputStream.cpp )

message( STATUS ${WXIO_SOURCES} )
message( STATUS ${FITS_SOURCES} )
message( STATUS ${WX_MISC} )

#file(GLOB MAIN_SOURCE "*.cpp")

message( STATUS ${MAIN_SOURCES} )

#set(SOURCES ${WXIO_SOURCES} ${FITS_SOURCES} ${AVI_SOURCES} ${MAIN_SOURCE})
set(SOURCES ${WXIO_SOURCES} ${FITS_SOURCES} ${MAIN_SOURCES} )

#Generate the shared library from the sources
add_library(artemis SHARED ${SOURCES} )
 
#Set the location for library installation -- i.e., /usr/lib in this case
# not really necessary in this example. Use "sudo make install" to apply
install(TARGETS artemis DESTINATION /usr/lib)
      
# CMake instructions to test using the static lib
add_executable(ArtemisTest ./main.cpp)
target_link_libraries( ArtemisTest artemis)


	

 



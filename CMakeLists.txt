cmake_minimum_required(VERSION 2.8.9)
project (PulseWave)

include_directories(inc)
include_directories("/usr/include/")

set(STATIC_FLAGS "-O3 -Wall -Wno-deprecated -DNDEBUG -DUNORDERED")
set(SHARED_FLAGS "-O3 -Wall -Wno-deprecated -DNDEBUG -DUNORDERED")

FILE(GLOB SOURCES src/pulseheader.cpp src/pulsevlrs.cpp src/pulsedescriptor.cpp src/pulsepulse.cpp src/waveswaves.cpp src/pulsereader.cpp 
			src/pulsewriter.cpp src/pulsereadermerged.cpp src/pulsereader_pls.cpp src/pulsereader_gcw.cpp src/pulsereader_lgw.cpp 
			src/pulsewriter_pls.cpp src/pulsewriter_txt.cpp src/pulsereadwaves_raw.cpp src/pulsereadwaves_compressed.cpp 
			src/pulsewritewaves_raw.cpp src/pulsewritewaves_compressed.cpp src/pulseutility.cpp src/pulsefilter.cpp 
			src/pulsetransform.cpp src/pulsezip.cpp src/pulsereadpulse.cpp src/pulsereaditemcompressed_v1.cpp src/pulsewritepulse.cpp 
			src/pulsewriteitemcompressed_v1.cpp src/integercompressor.cpp src/arithmeticdecoder.cpp src/arithmeticencoder.cpp src/arithmeticmodel.cpp)




FILE(GLOB SINCLUDES ${CMAKE_SOURCE_DIR}/src/*.hpp)


add_library(pulsewavesSHARED SHARED ${SOURCES} ${INCLUDE} ${SINCLUDES})

add_library(pulsewavesSTATIC STATIC ${SOURCES} ${INCLUDE} ${SINCLUDES})

set_target_properties(pulsewavesSHARED PROPERTIES OUTPUT_NAME pulsewaves)
set_target_properties(pulsewavesSTATIC PROPERTIES OUTPUT_NAME pulsewaves)

#add_executable(hello helloworld.cpp)

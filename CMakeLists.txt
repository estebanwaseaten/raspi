cmake_minimum_required(VERSION 3.0.0)

project(raspi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)



#set(HEADER_LIST
#    "include/raspi.h"
#    "include/raspi_gpio.h"
#    "include/raspi_spi.h"
#
#    "src/raspi_gpio_bcm2835.h"
#    "src/raspi_gpio_bcm2835.h"
#)


set(SOURCE_LIST
	"src/raspi.cpp"
    "src/raspi_gpio_bcm2835.cpp"
    "src/raspi_gpio_dwapbssi.cpp"
    "src/raspi_spi_bcm2835.cpp"
    "src/raspi_spi_dwapbssi.cpp"
)

set(LIBRARIES_LIST
	${PROJECT_SOURCE_DIR}/libs/devicetree/       #THIS projects source dir... means we can only build this when threader is installed BESIDE/NEXT TO this library...
)

foreach(LIBRARY ${LIBRARIES_LIST} )            # all libs THIS project depends on must be built first
	add_subdirectory(${LIBRARY} ${LIBRARY}/build)      # first ${LIBRARY} for source dir, second for binary dir (i.e. the dir where all the output files are put, but not the library binary)
endforeach()


add_library(${PROJECT_NAME} SHARED ${SOURCE_LIST} )

target_include_directories(${PROJECT_NAME}
    PUBLIC
        include
        ${PROJECT_SOURCE_DIR}/libs/devicetree/include
    PRIVATE src
)

target_link_libraries(${PROJECT_NAME} PUBLIC devicetree)			# link against devicetree
#target_link_libraries(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

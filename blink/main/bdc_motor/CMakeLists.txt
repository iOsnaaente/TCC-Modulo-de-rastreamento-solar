include($ENV{IDF_PATH}/tools/cmake/version.cmake)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" "interface"
                       REQUIRES "driver")

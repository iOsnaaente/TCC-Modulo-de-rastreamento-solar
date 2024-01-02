set(PROJECT_NAME "modbus_tcp_slave")

idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

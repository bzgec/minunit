FILE(GLOB minunit *.c)

idf_component_register(SRCS ${minunit}
                       INCLUDE_DIRS .)

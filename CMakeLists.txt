add_library(husb238 STATIC husb238.c)

target_include_directories(husb238 PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries( husb238 PUBLIC
		pico_stdlib
		hardware_i2c
		)
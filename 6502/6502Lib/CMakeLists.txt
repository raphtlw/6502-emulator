cmake_minimum_required(VERSION 3.7)

project( M6502Lib )

# if(MSVC)
# 	add_compile_options(/MP) # Use multiple processors when building
# 	add_compile_options(/W4 /wd4201 /WX) # Warning level 4, all warnings are errors
# else()
# 	add_compile_options(-W -Wall -Werror) # All Warnings, all warnings are errors
# endif()

set(M6502_SOURCES
  "src/m6502.h"
	"src/m6502.cpp"
)

source_group("src" FILES ${M6502_SOURCES})

add_executable(M6502Lib "src/main.cpp")
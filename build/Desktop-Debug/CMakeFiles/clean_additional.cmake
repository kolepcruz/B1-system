# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/b1_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/b1_autogen.dir/ParseCache.txt"
  "b1_autogen"
  )
endif()

#cmake_minimum_required(VERSION 3.3)
cmake_minimum_required(VERSION 3.16)

# list of source files
set(dgpTest3_files dgpTest3.cpp dgpPrt.cpp)

# define the executable
if(WIN32)
  add_executable(dgpTest3 WIN32 ${dgpTest3_files})
else()
  add_executable(dgpTest3 ${dgpTest3_files})
endif()

# in Windows + Visual Studio we need this to make it a console application
if(WIN32)
  if(MSVC)
    set_target_properties(dgpTest3 PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
  endif(MSVC)
endif(WIN32)

# install application
set(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)

# target_link_libraries(dgpTest2 core io util wrl)
target_link_libraries(dgpTest3 ${LIB_LIST})

install(TARGETS dgpTest3 DESTINATION ${BIN_DIR})


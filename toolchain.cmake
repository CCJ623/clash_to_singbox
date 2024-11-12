# cross compile settings
set(CMAKE_SYSTEM_NAME Windows)

#####
##### change following env according to your installation.
#####
 
message(STATUS "cross compile for windows_32 ")
set(TOOLCHAIN_PATH  /usr)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/x86_64-w64-mingw32-g++)
 
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
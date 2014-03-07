mkdir build
copy libs/x86/SDL.dll build
cd build
cmake.exe .. -DSDL_INCLUDE_DIR=../include/SDL -DSDL_LIBRARY_TEMP=../lib/x86/SDL -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 11"

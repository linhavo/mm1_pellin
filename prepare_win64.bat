mkdir build
copy libs/x64/SDL.dll build
cd build
cmake.exe .. -DSDL_INCLUDE_DIR=../include/SDL -DSDL_LIBRARY_TEMP=../lib/x64/SDL -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 11 Win64"

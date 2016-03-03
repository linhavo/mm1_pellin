mkdir build
copy lib\x64\SDL.dll build
cd build
cmake.exe .. -DSDL_INCLUDE_DIR=../include/SDL -DSDL_LIBRARY_TEMP=../lib/x64/SDL.lib -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 14 2015 Win64"

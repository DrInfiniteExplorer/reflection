set CC=gcc
set CXX=g++
mkdir build-gcc
cd build-gcc
cmake .. -G "Unix Makefiles" && cmake --build . && ctest ..
cd ..

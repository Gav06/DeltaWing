rm -r build
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
# Remove build dir if it exists
rm -r build

mkdir build
cd build
cmake -G "Unix Makefiles" ..
cmake --build .

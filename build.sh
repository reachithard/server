
set -eux

rm -rf build/
mkdir build
cd build
cmake -DCMAKE_VERBOSE_MAKEFILE=on ..
make

set -e

rm -f *.gcda
rm -f *.gcno
mkdir -p out
rm -f out/*

rm -rf build

rm -f edge-impulse-sdk/utensor/CMakeCache.txt

echo "Building uTensor library"

# build uTensor library file
cd edge-impulse-sdk/utensor
cmake .
make

echo "Building uTensor library OK"

echo "Building standalone classifier"

cd ../..

# build the Edge Impulse classifier
make

echo "Building standalone classifier OK"

# clear up
rm -f *.gcda
rm -f *.gcno
rm -rf out

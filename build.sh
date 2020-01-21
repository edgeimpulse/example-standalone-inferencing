set -e

rm -f *.gcda
rm -f *.gcno
mkdir -p out
rm -f out/*

rm -rf build && rm -rf edge-impulse-sdk && rm -rf model-parameters && rm -rf utensor-model
unzip deploy.zip

rm -f edge-impulse-sdk/utensor/CMakeCache.txt

echo "Building uTensor library"

# build uTensor library file
cd edge-impulse-sdk/utensor
cmake .
make

echo "Building uTensor library OK"

echo "Building sanity checker"

cd ../..

# build the uTensor classifier
make

echo "Building sanity checker OK"

# clear up
rm -f *.gcda
rm -f *.gcno
rm -rf out

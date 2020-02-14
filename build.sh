set -e
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

rm -f $SCRIPTPATH/*.gcda
rm -f $SCRIPTPATH/*.gcno
mkdir -p $SCRIPTPATH/out
rm -f $SCRIPTPATH/out/*

rm -rf $SCRIPTPATH/build

rm -f $SCRIPTPATH/edge-impulse-sdk/utensor/CMakeCache.txt

if [ -d $SCRIPTPATH/utensor-model/ ]; then
    echo "Building uTensor library"

    # build uTensor library file
    cd $SCRIPTPATH/edge-impulse-sdk/utensor
    cmake .
    make

    echo "Building uTensor library OK"
fi

echo "Building standalone classifier"

cd $SCRIPTPATH

# build the Edge Impulse classifier
if [ -d $SCRIPTPATH/utensor-model/ ]; then
    make -f Makefile.utensor
else
    make -f Makefile.tflite
fi

echo "Building standalone classifier OK"

# clear up
rm -f $SCRIPTPATH/*.gcda
rm -f $SCRIPTPATH/*.gcno
rm -rf $SCRIPTPATH/out

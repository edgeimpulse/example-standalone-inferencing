set -e
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

rm -rf $SCRIPTPATH/build

echo "Building standalone classifier"

cd $SCRIPTPATH

make -f Makefile.tflite

echo "Building standalone classifier OK"

# clear up
rm -f $SCRIPTPATH/*.gcda
rm -f $SCRIPTPATH/*.gcno

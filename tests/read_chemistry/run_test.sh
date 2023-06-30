#!/bin/sh

# remove old directory:
rm -rf run.no_errors run.errors

# run without any errors to check it works:
cp -R ../../share/run ./run.no_errors
cd run.no_errors
cp ../aether.json.no_errors ./aether.json
./aether
cd ../

#now we get into the real testing
cp -R ../../share/run ./run.errors
cd run.errors
for i in 0 1 2 3 4 5; 
do
    echo
    echo Test $i: checking for successful crash 
    echo ......................
    sed -i "s/chemistry_earth_richards.csv/chemistry_check_$i.csv/g" aether.json
    cp ../aether.check_$i ./aether.json
    mpirun -np 4 ./aether
    sed -i "s/chemistry_check_$i.csv/chemistry_earth_richards.csv/g" aether.json
done;
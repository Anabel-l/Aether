#!/bin/sh
#run aacgm version
rm -rf run.test
cp -R ../../share/run ./run.test
cd run.test
cp -f ../aether.json.aacgm ./aether.json
./aether
cd UA/output
~/bin/postAether.py -rm
~/bin/aether_plot_simple.py 3DBFI_*.nc -var="Beast" -alt=120
~/bin/aether_plot_simple.py 3DBFI_*.nc -var="Bnorth" -alt=120
~/bin/aether_plot_simple.py 3DBFI_*.nc -var="Bvertical" -alt=120
~/bin/aether_plot_simple.py 3DBFI_*.nc -var="mlat" -alt=120

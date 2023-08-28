#! /usr/bin/bash
#run aacgm version
cp -R ../../share/run ../../run.test
cd ../../run.test/UA/inputs
cp -f ../../../tests/aacgm_plot/aether.json.aacgm ./aether.json
cd ../../
./aether
cd UA/output
../../../srcPython/postAether.py -rm
~/Aether-test/Aether/aetherpy/aether_plot_simple.py 3DBFI_*.nc -var="Beast" -alt=200
~/Aether-test/Aether/aetherpy/aether_plot_simple.py 3DBFI_*.nc -var="mlat" -alt=200
~/Aether-test/Aether/aetherpy/aether_plot_simple.py 3DBFI_*.nc -var="O" -alt=200
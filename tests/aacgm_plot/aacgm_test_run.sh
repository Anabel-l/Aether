#! /usr/bin/bash
cp -R ../../share/run ../../run.test
cd ../../run.test/UA/inputs
cp -f ../../../tests/aacgm_plot/defaults.json.aacgm ./defaults.json
cd ../../
./aether
cd UA/output
../../../srcPython/postAether.py -rm
~/Aether-test/Aether/aetherpy/aether_plot_simple.py 3DALL_*.nc -var="O" -alt=200
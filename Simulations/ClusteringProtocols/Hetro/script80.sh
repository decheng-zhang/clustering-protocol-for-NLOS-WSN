#!/bin/bash
numNodes=80

for ((POS=1; POS <= 8 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 12480 ] >> EnergyLevels80
done

for ((POS=9; POS <= 16 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 18720 ] >> EnergyLevels80
done

for ((POS=17; POS <= numNodes ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 6240 ] >> EnergyLevels80
done

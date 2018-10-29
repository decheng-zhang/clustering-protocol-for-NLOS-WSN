#!/bin/bash
numNodes=60

for ((POS=1; POS <= 6 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 12480 ] >> EnergyLevels60
done

for ((POS=7; POS <= 12 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 18720 ] >> EnergyLevels60
done

for ((POS=13; POS <= numNodes ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 6240 ] >> EnergyLevels60
done

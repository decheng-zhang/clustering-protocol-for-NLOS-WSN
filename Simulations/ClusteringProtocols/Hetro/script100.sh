#!/bin/bash
numNodes=100

for ((POS=1; POS <= 10 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 12480 ] >> EnergyLevels40
done

for ((POS=11; POS <= 20 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 18720 ] >> EnergyLevels40
done

for ((POS=21; POS <= numNodes ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 6240 ] >> EnergyLevels40
done

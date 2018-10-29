#!/bin/bash
numNodes=500

for ((POS=1; POS <= 50 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 12480 ] >> EnergyLevels40
done

for ((POS=51; POS <= 100 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 18720 ] >> EnergyLevels40
done

for ((POS=101; POS <= numNodes ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 6240 ] >> EnergyLevels40
done

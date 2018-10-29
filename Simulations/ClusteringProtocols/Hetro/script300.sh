#!/bin/bash
numNodes=300

for ((POS=1; POS <= 30 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 12480 ] >> EnergyLevels40
done

for ((POS=31; POS <= 60 ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 18720 ] >> EnergyLevels40
done

for ((POS=61; POS <= numNodes ; POS++)); do
        echo SN.node\[$POS\].ResourceManager.initialEnergy = $[ 6240 ] >> EnergyLevels40
done


The repository embodies a set of experiments for NLOS clustering protocol in WSN (Wireless Sensor Network)

#Experiment Details
All experiment is executed over 100..500 network size
## Experiment 1
PDR for NSGA-LOS-CH vs NSGA-NLOS-CH

## Experiment 2
Existence of obstacles
Packet Deliver Rate for three protocols:
NSGA-NLOS-CH RSSI values derived from the proposed path loss model
PSO-CH RSSI(not consider obstacle)
LEACH-3D totally random mechanism when electing the CHs

## Experiment 3
Average num of CHs per round for all protocols
NSGA-NLOS-CH results in a higher num of CHs

## Experiment 4
Average consumed energy per sensor
LEACH-3D >> NSGA-CH-NLOS > PSO-CH (due to higher num of CHs)

## Experiment 5
Average number of "unclustered sensors" per round
LEACH-3D >> NSGA-CH-NLOS == PSO-CH

## Proposing
Demonstrate Coverage performance between three algorithm.

# Software Specs

* Ubuntu 14.04 16.04
* Omnetpp 4.5, 4.6
* Castalia 3.3
* JmetalCpp
* Lib-Popot

# Prerequisites

* Omnetpp
* Lib popot 
* JmetalCpp (included)
* Adding new LDFLAGS **--allow-multiple-definition** in the Makefile.inc in Omnetpp.x.y.z/  

# Install
```
  ./makemake
   make
```
# Usage
```
$ Castalia -c [CONFIG]
```

# License

Credit to [Riham Elhabyan](riham.elhabyan@gmail.com)
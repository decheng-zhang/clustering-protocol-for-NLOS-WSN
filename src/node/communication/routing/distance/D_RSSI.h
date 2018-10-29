#ifndef _D_RSSI_H_
#define _D_RSSI_H_

#include <queue>
#include <vector>
#include <omnetpp.h>
#include <algorithm>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <D_RSSI_m.h>
#include "ResourceManager.h"
#include "VirtualRouting.h"
#include "VirtualApplication.h"	
#include "PSORoutingPacket_m.h"
#include "ApplicationPacket_m.h"
#include "NoMobilityManager.h"


enum D_RSSI_Timers 
{
	START_ROUND = 0,
	ADV_RSSI = 1,
	ADV_RSSI2 = 2,
       
};

class D_RSSI : public VirtualRouting 
{
private:
	string applicationID;

	bool isSink;
   	

	int advPacketSize;
	int tdmaPacketSize;
	int dataPacketSize;
	int joinPacketSize;
	
	
	double maxPower;
	double sensibility;
	double aggrConsumption;

        double slotLength;
	double percentage;
	double roundLength;

	

     	cModule *theSNModule;
	vector<RoutingPacket> bufferAggregate;
	vector<int> powers;
	queue <cPacket *> tempTXBuffer;

      	
public:

	void startup();
	void finishSpecific();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);
	void initProtocol();
        void runCDDP();
	void processBufferedPacket();
	void sendAggregate();
	void setPowerLevel(double);
	void setStateSleep();
	void setStateRx();
	void levelTxPower(int);	
	void generateGraph();
	void readXMLparams();
	void updateNeighborTable(int id, double rssi, double energy);
		
};

#endif	

		

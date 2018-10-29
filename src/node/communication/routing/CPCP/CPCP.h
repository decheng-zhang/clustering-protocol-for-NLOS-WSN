#ifndef _CPCP_H_
#define _CPCP_H_

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
#include "VirtualRouting.h"
#include "VirtualApplication.h"	
#include "ApplicationPacket_m.h"
#include "CPCPPacket_m.h"
#include "NoMobilityManager.h"
#include <boost/tokenizer.hpp>
//#include <ExperimentExecution.h>
#include <string>

using namespace std;
using namespace boost;

enum CPCPTimers {
	START_ROUND = 1,	
	SEND_ADV = 2,	
	JOIN_CH = 3,		
	MAKE_TDMA = 4,			
	START_SLOT = 5,	
	END_SLOT = 6,
	CHECK_CH_STATUS = 7,
	CALCULATE_COVERAGE_COST = 8,
	FIND_COVERAGE_COST = 9,
	FIND_NEIGHBORS = 10,
	CHECK_ACTIVATION_STATUS = 11,
	TEST = 12,
	ENTER_STEADY_PHASE = 13,

};

struct CHInfo
{
	int src;
	double rssi;
};

struct NeighborRecord 
{
	int id;
	double rssi;
	double remainingEnergy;
};

struct nodeInfo
{
	int id;
	double x;
	double y;
	double energy;
	vector<nodeInfo> members;
};


class CPCP : public VirtualRouting {

private:
	
	string applicationID;
	int advPacketSize;
	int tdmaPacketSize;
	int dataPacketSize;
	int joinPacketSize;

	double maxPower;
	double sensibility;
	double aggrConsumption;

	double sensingRange;
        double cubeWidth;
	double distError;
        double numOfCubes;
	double width;
	double height;
	int myCH;

	
	double slotLength;
	int clusterLength;
	double percentage;
	double probability;
	double roundLength;
	double initialEnergy;
	int roundNumber;
	int myTDMATurn;
	int dataSN;
	int numberOfRounds;
	bool isCH;
	bool isCM;
	bool isSink;
	bool isActive;
	bool areaIsCovered;
	bool isCt;
	bool assignedTDMA;
	bool endFormClus;

	double totalPackets;
	cModule *theSNModule;
	int networkSize;

	double myWght;
	vector<vector<double> > coverageMatrix;	
	vector<vector<double> > adjacencyMatrix;

	vector<RoutingPacket> bufferAggregate;
	vector<int> powers;
	queue <cPacket *> tempTXBuffer;
	vector <int> clusterMembers;
	list <CHInfo> CHcandidates;

	vector <double> energyPerCell;
	vector <double> minWghtCost;
	vector <double> WghtSumCost;	

protected:

	void startup();
	void finishSpecific();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);
	void processBufferedPacket();

	void runCDDP();
	
	void sendAggregate();
	void setPowerLevel(double);
	void setStateSleep();
	void setStateRx();
	void levelTxPower(int);	
	void readXMLparams();

};
bool cmpRssi(CHInfo a, CHInfo b);

#endif			

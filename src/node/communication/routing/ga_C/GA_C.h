#ifndef _GA_C_H_
#define _GA_C_H_

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
#include "ResourceManager.h"
#include "VirtualRouting.h"
#include "VirtualApplication.h"	
#include "ApplicationPacket_m.h"
#include "NoMobilityManager.h"
#include "GARoutingPacket_m.h"
#include <gGA.h>
#include <SBXCrossover.h>
#include <PolynomialMutation.h>
#include <BinaryTournament2.h>
#include <Algorithm.h>
#include <Solution.h>
#include <time.h>
#include <ProblemFactory.h>
#include <GAProblem.h>
#include <PSOProblem.h>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

struct NeighborRecord 
{
	int id;
	double rssi;
	double remainingEnergy;
};

enum GA_CTimers 
{
	START_ROUND = 0,
	RUN_GA = 1,
        RUN_CDDP = 2,
	ENTER_STEADY_PHASE = 3,
	START_SLOT = 4,
	END_SLOT = 5,
	TEST = 6,
	FIND_NEIGHBORS = 7,
};

class GA_C : public VirtualRouting 
{
private:
	string applicationID;

	bool isSink;
   	bool isCH;
	bool isCM;
	bool endFormClus;

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

	double totalPackets;

	int roundNumber;
	int numberOfRounds;
	int networkSize;
	int numOfCHs;
	int populationSize;
	int epoch;

	int myCH;
	int myTDMATurn;
	int clusterLength;

	

	int noLiveNodes;

     	cModule *theSNModule;
	vector <NeighborRecord> neighborTable;
	vector <int> neighbors;
	const char* networkInformation;
	vector <nodeInfo> networkInfo ;
	vector <nodeInfo> eligibleNodes; 
	vector <int> actualMembers;
	vector<RoutingPacket> bufferAggregate;
	vector<int> powers;
	vector <int> clusterMembers;
	vector <nodeInfo> clusterHeads;
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

		

#ifndef _MOPSO_C_H_
#define _MOPSO_C_H_

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
#include "MOPSORoutingPacket_m.h"
#include "VirtualApplication.h"	
#include "ApplicationPacket_m.h"
#include "NoMobilityManager.h"
#include <Problem.h>
#include <Algorithm.h>
#include <Solution.h>
#include <Operator.h>
//#include <QualityIndicator.h>
#include <ProblemFactory.h>
#include <SMPSO.h>
#include <PolynomialMutation.h>
#include <iostream>
#include <Clustering.h>
#include <time.h>

#include <boost/tokenizer.hpp>

using namespace boost;

struct nodeInfo
{
	int id;
	double x;
	double y;
	double energy;
	vector<nodeInfo> members;
};

struct CH
{
	int id;
	double rssi;
};


enum MOPSO_C_H_Timers 
{
	START_ROUND = 0,
	FIND_NEIGHBORS = 1,
	RUN_PROTOCOL = 2,
      	RUN_PSO = 3,
        RUN_CDDP = 4,
	START_SLOT = 5,
	END_SLOT = 6,
	TEST = 7,
	CONFG_NWK = 8,
	RUN_PERIORITY_ENCODING = 9,
	FIND_ROUTES = 10,
	ENTER_STEADY_PHASE = 11,
	SEND_INTER = 12,
	SEND_INTRA = 13,
	RUN_PSO_TWO = 14,
	SET_STATE_RX = 15,
};


class MOPSO_C : public VirtualRouting 
{
	private:
	
	string applicationID;

	bool isSink;
   	bool isCH;
	bool isCM;
	bool isRelayNode;
	bool endFormClus;

	int mode;
	int routingSwarmSize;

	int advPacketSize;
	int tdmaPacketSize;
	int dataPacketSize;
	int joinPacketSize;

	int packetsPerNode;
	int packetSize;
	int packetsSent;

	double packetSpacing;
	double txInterval_perNode;
	double txInterval_total;
	
	
	double maxPower;
	double sensibility;
	double aggrConsumption;

        double slotLength;
	double percentage;
	double roundLength;

	int roundNumber;
	int numberOfRounds;
	int networkSize;
	int numOfCHs;
	int swarmSize;
	int CH_Frequencey;
	int numOfSel;
	int epoch;

	//ofstream myfile1;	
	//ofstream myfile2;

	int numOfNbrs;

	int myCH;
	int nextHop;
	int myTDMATurn;
	int clusterLength;

	int noLiveNodes;
	
     	cModule *theSNModule;
		
	const char* candidates;
	const char* routes;
	const char* neighbours;
	
	vector <int> heads;
	vector<vector<double> > adjacencyMatrix;

	const char* networkInformation;
	vector <NeighborRecord> neighborTable;
	vector <NodeControlInfo> networkInfo ;
	vector <NodeControlInfo> eligibleNodes;
	vector <NodeControlInfo> clusterHeads;
	vector<RoutingPacket> bufferAggregate;
	vector <int> neighbors;
	vector <int> powers;
	vector <int> clusterMembers;
	queue <cPacket *> tempTXBuffer;
	
      
			
public:

	void startup();
	void finishSpecific();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);
	void setPowerLevel(double);
	void processBufferedPacket();
	void sendAggregate();
	void setStateSleep();
	void setStateRx();
	void initProtocol();
     	void runCDDP();
	void readXMLparams();
	void levelTxPower(int);	
	void updateNeighborTable(int id, double rssi, double energy);
};
#endif	

		

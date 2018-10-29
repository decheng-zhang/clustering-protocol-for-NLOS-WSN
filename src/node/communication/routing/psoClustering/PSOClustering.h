#ifndef _PSOCLUSTERING_H_
#define _PSOCLUSTERING_H_

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
#include "PSOClusteringPacket_m.h"
#include "ApplicationPacket_m.h"
#include "NoMobilityManager.h"
#include "RoutingProblem.h"
#include "ClusteringProblem.h"
#include <boost/tokenizer.hpp>



#include "popot/rng_generators.h"
typedef popot::rng::CRNG RNG_GENERATOR;
#include "popot/popot.h"

using namespace std;
using namespace boost;



enum PSOClusteringTimers 
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
};

struct RelayNode
{
   int node;
   int nextNode;

};

struct nodeInfo
{
  int id;
  double energy;
  vector<nodeInfo> members;
};

struct CH
{
	int id;
	double rssi;
};

class PSOClustering : public VirtualRouting 
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
		
	double startTime;
	double endTime;

	double packetSpacing;
	double txInterval_perNode;
	double txInterval_total;
	clock_t t_ini, t_fin;
	
	double maxPower;
	double sensibility;
	double aggrConsumption;

        double slotLength;
	double percentage;
	double roundLength;
	double totalPackets;

	double initialEnergy;

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

	CH myCH;
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
	vector <int> powers;
	vector <int> clusterMembers;
	vector <Path> InterClusterRoutes; 
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
	void updateNeighborTable(int id, double rssi, double energy);
	void setNeighbours();
	void sendAggregate();
	void setPowerLevel(double);
	void setStateSleep();
	void setStateRx();
	void setStateTx();
	void levelTxPower(int);	
	void fillInNeighborData(char* nbr);
	void generateGraph();
	double getRSSI(int nbr);
	void readXMLparams();

};


bool cmpCHsRssi(CH a, CH b);
#endif	

		

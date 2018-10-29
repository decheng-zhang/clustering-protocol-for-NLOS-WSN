#ifndef _LEACHCENTRALIZED_H_
#define _LEACHCENTRALIZED_H_

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
#include "LeachCentralizedRoutingPacket_m.h"
#include "ApplicationPacket_m.h"
#include "NoMobilityManager.h"
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

enum LEACHCENTRALIZED_Timers 
{
	START_ROUND = 0,
	RUN_SA = 1,
        RUN_CDDP = 2,
	ENTER_STEADY_PHASE = 3,
        START_SLOT = 4,
	END_SLOT = 5,
	TEST = 6,
	FIND_NEIGHBORS = 7,
};

struct NeighborRecord 
{
	int id;
	double rssi;
	double remainingEnergy;
};

struct CHInfo
{
	int src;
	double rssi;
};

struct NodeInfo
{
	int id;
	double x;
	double y;
	double energy;
	vector<NodeInfo> members;
};

struct ID_Comparer
{
    bool operator() (const NodeInfo& data1, const NodeInfo& data2) const { return ( data1.id < data2.id );   }
};


class LeachCentralized : public VirtualRouting 
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

	int percentage;
        double slotLength;
	double roundLength;
	
	double initialTemp;
	double coolingRate;
	
	double totalPackets;

	int clusterLength;
	int roundNumber;
	int numberOfRounds;
	int networkSize;
	int numOfCHs;
	int swarmSize;
	int epoch;
	
	int myCH;
	int myTDMATurn;

	int noLiveNodes;

     	cModule *theSNModule;
	
	const char* networkInformation;
	vector <NeighborRecord> neighborTable;
	vector <int> neighbors;
	vector <NodeInfo> networkInfo ;
	vector <NodeInfo> eligibleNodes; 
	vector <RoutingPacket> bufferAggregate;
	vector <int> powers;
	vector <int> clusterMembers;
	vector <int> actualMembers;
	vector <int> clusterHeads;
	queue  <cPacket *> tempTXBuffer;
        list <CHInfo> CHcandidates;
        
	
		
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
	vector<NodeInfo> generateInitialSolution();
	vector<NodeInfo> clusterTheNetwork(vector<NodeInfo> candidates);
	void updateNeighborTable(int id, double rssi, double energy);
	double evaluateSolution(vector<NodeInfo> candidates);
	NodeInfo getRandomNode(vector<NodeInfo> currentSolution);
	bool contains(vector<NodeInfo> currentSolution,NodeInfo n);
	double acceptanceProbability(int engery, int newEngery, double temperature);
	void generateGraph();
	void readXMLparams();
		
};
bool cmpCHeadsRssi(CHInfo a, CHInfo b);
#endif	

		

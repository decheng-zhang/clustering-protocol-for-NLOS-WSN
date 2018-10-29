#ifndef _PSO_CH_H_
#define _PSO_CH_H_

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
#include "PSOPacket_m.h"
#include "NoMobilityManager.h"
#include <StandardPSO2011.h>
#include <Algorithm.h>
#include <Solution.h>
#include <time.h>
#include <SMPSO.h>
#include <ProblemFactory.h>
#include <StandardStudyReportSO.h>
#include <StandardStudyExecutionSO.h>
#include <PSOProblem1.h>
#include <boost/tokenizer.hpp>
//#include <ExperimentExecution.h>
#include <string>

using namespace std;
using namespace boost;

struct NeighborRecord 
{
	int id;
	double rssi;
	double remainingEnergy;
};

enum PSO_CH_Timers 
{
	START_ROUND = 0,
	RUN_PSO = 1,
        RUN_CDDP = 2,
	ENTER_STEADY_PHASE = 3,
	START_SLOT = 4,
	END_SLOT = 5,
	TEST = 6,
	FIND_NEIGHBORS = 7,
	RUN_TPC = 8,
};

struct nodeInfo
{
	int id;
	double x;
	double y;
	double energy;
	vector<nodeInfo> members;
};

struct ID_Comparer
{
    bool operator() (const nodeInfo& data1, const nodeInfo& data2) const { return ( data1.id < data2.id );   }
};


class PSO_CH : public VirtualRouting
{
private:
	string applicationID;

	bool isSink;
   	bool isCH;
	bool isCM;
	bool endFormClus;

	int protocol;
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

	double initialEnergy;
	double sensingRange;
        double cellWidth;
	double distError;
        double numOfCells;
	int width;
	int height;
	vector<vector<double> > coverageMatrix;	

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

	const char* neighbours;
	const char* candidates;
	const char* networkInformation;

	vector<vector<double> > adjacencyMatrix;


	int noLiveNodes;

     	cModule *theSNModule;
	vector <NeighborRecord> neighborTable;
	vector <int> neighbors;
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

		

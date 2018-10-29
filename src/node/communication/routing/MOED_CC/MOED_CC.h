#ifndef _MOED_CC_H_
#define _MOED_CC_H_

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
#include <StandardPSO2011.h>
#include <Algorithm.h>
#include <Solution.h>
#include <time.h>
#include <NSGAII.h>
#include <ProblemFactory.h>
#include <StandardStudyReportSO.h>
#include <StandardStudyExecutionSO.h>
#include <MOED_CCP.h>
#include <MOEAD.h>
#include <SMPSO.h>
#include <MOED_CC_m.h>
#include <BinaryTournament2.h>
#include <BitFlipMutation.h>
#include <SinglePointCrossover.h>
#include <CoverageProblem.h>
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

enum MOED_CC_Timers 
{
	START_ROUND = 0,
	RUN_CCP = 1,
        RUN_CDDP = 2,
	ENTER_STEADY_PHASE = 3,
	START_SLOT = 4,
	END_SLOT = 5,
	TEST = 6,
	FIND_NEIGHBORS = 7,
	RUN_TPC = 8,
	OPT_COVERAGE =9,
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


class MOED_CC : public VirtualRouting
{
private:
	string applicationID;

	bool isSink;
   	bool isCH;
	bool isActive;
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
	double cubeWidth;
	double distError;
        double numOfCubes;
	double sensingRange;

	int roundNumber;
	int numberOfRounds;
	int networkSize;
	double width;
	double length;
	double height;
	int numOfCHs;
	int populationSize;
	int epoch;

	int myCH;
	int myTDMATurn;
	int clusterLength;

	ofstream myfile, myfile2;

	const char* neighbours;
	const char* candidates;
	const char* networkInformation;
	const char* networkStatus;

       	
	vector<SensorInfo> Sensors;
	vector<vector<double> > adjacencyMatrix;
	vector<vector<double> > coverageMatrix;
	vector<vector<double> > overlappingMatrix;
		
     	cModule *theSNModule;
	vector <NeighborRecord> neighborTable;
	vector <int> neighbors;
	vector <nodeInfo> networkInfo ;
	vector <nodeInfo> eligibleNodes; 
        vector<double> energyVector;
	vector <int> actualMembers;
        vector <int> CHs;
	vector<RoutingPacket> bufferAggregate;
	vector<int> powers;
	vector <int> clusterMembers;
	vector <int> clusterHeads;
	multimap<int,int> network;
	queue <cPacket *> tempTXBuffer;

      	
public:

	void startup();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);
	void updateSensors();
	void updateSensorInfo();
        void runCDDP();

	void initializeMatrices();		
	void updateMatrices();
	void updateCoverageMatrix();
	void processBufferedPacket();
	void sendAggregate();
	void setPowerLevel(double);
	void setStateSleep();
	void setStateRx();
	void levelTxPower(int);	
	void clusterTheNetwork();
	void readXMLparams();
	string returnConfiguration(vector<int> activeNodes);
	void testCoveragePerformance(vector<int> sleepNodes);
	Solution * findBestCompromiseSolution(SolutionSet *set);
	double findOverlappingArea(double dist);
	
};

#endif	

		

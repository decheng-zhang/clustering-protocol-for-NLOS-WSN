#ifndef _NSGA_II_CCP_3D_CO_
#define _NSGA_II_CCP_3D_CO_

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
#include "NSGA_II_CCP_3D_CO_m.h"
#include "NoMobilityManager.h"
#include <NSGAII.h>
#include <Algorithm.h>
#include <Solution.h>
#include <time.h>
#include <SMPSO.h>
#include <ProblemFactory.h>
#include <StandardStudyReportSO.h>
#include <StandardStudyExecutionSO.h>
#include <BinaryTournament2.h>
#include <BitFlipMutation.h>
#include <SinglePointCrossover.h>
#include <CH_3D.h>
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

struct Obstacle 
{
	int id;
	double x;
	double y;
	double z;
};

enum NSGA_II_CCP_3D_CO_Timers 
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


class NSGA_II_CCP_3D_CO : public VirtualRouting
{
private:
	string applicationID;

	bool isSink;
	bool isCH;
	bool isActive;
	bool isCM;
	bool endFormClus;
	bool isObstacleAware;

	int protocol;
	int advPacketSize;
	int tdmaPacketSize;
	int dataPacketSize;
	int joinPacketSize;
	
	
	double maxPower;
	double sensibility;
	double totalPackets;
	double aggrConsumption;

        double slotLength;
	double percentage;
	double roundLength;

	double initialEnergy;
	double cellWidth;
	double distError;
        double numOfCells;
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
	vector<vector<double> > DEM;	
	vector<vector<double> > PLD;	
	vector<vector<double> > coverageMatrix;	
	vector<vector<double> > ObstaclesPLDMatrix;
	vector<vector<double> > adjacencyMatrix;
	vector<vector<double> > visibilityMatrix;
	double tp;
	
	int noLiveNodes;

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
	queue <cPacket *> tempTXBuffer;

      	
public:

	void startup();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);
	void updateSensorInfo();
        void runCDDP();

	void initializeMatrices();		
	void loadDEMData();
	void updateSensorsElevation();
	void updateVisibilityMatrix();
	void generateLNSMPathLossMap();
	void updateAdjacencyMatrix();
	void updateCoverageMatrix();

	void processBufferedPacket();
	string returnConfiguration();
	void sendAggregate();
	void setPowerLevel(double);
	void setStateSleep();
	void setStateRx();
	void levelTxPower(int);	
	void generateGraph();
	void finishSpecific();
	void readXMLparams();
	void getCell(int x, int y);
	bool areSameCells(double xt,double yt,double xr,double yr);
	bool areNeighoubringCells(double xt,double yt,double xr,double yr);
	double LOS(double xt,double yt,double zt, double xr,double yr, double zr);
	double findNextObstacle(double xt,double yt,double zt, double xr,double yr, double zr);
	double calculatePLDObstacles(vector<Obstacle> Obstacles);
	void updateNeighborTable(int id, double rssi, double energy);
	Solution * findBestCompromiseSolution(SolutionSet *set);
	
};

#endif	

		

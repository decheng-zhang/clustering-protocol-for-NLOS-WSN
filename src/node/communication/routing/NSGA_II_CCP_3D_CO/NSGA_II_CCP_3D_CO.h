#ifndef _NSGA_II_CCP_3D_CO_
#define _NSGA_II_CCP_3D_CO_

#include <queue>
#include <vector>
#include <omnetpp.h>
#include <algorithm>
//#include <string>
#include <cmath>
//#include <stdlib.h>
#include <utility>
//#include <stdio.h>
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
#include <CH_3D_CO.h>
//#include <CH_3D.h>
#include <boost/tokenizer.hpp>
//#include <ExperimentExecution.h>
#include <string>
#define NO_TIN_D 20
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
	vector<int>  coverageMatrix;
	vector<vector<int>> coveringMappingMatrix;
	vector<vector<double> > ObstaclesPLDMatrix;
	vector<vector<double> > adjacencyMatrix;

	vector<vector<double> > visibilityMatrix;
	vector<Tin> Tin_Matrix;
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
	void bfs (vector<vector<int>> &board, int row, int col, double coorx, double coory, double sen_range) ;

public:
	
	void startup();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);
	void updateSensorInfo();
        void runCDDP();
	void testingFun(){
	  int i=0;
	  ofstream os;
	  os.open("nsga-cmap.txt"); 
	  ;
	  for(auto p : coverageMatrix)
	    {
	      if(i%40 ==0) os<< "\n";
	      os << p <<" ";
	      i++;
	    }
	  os.close();
	  // trace() << "UU " << os.str();
	};

	void initializeMatrices();		
	void loadDEMData();
	void updateSensorsElevation();
	void updateVisibilityMatrix();
	void generateLNSMPathLossMap();
	void updateAdjacencyMatrix();
	void updateCoverageMatrix();
	double evaluateCoverageRedundancy();
	void updateTinMatrix();
	void updateSingleCoverage(vector<vector<int>> &board, const SensorInfo sen) ;
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
	bool coveringTin(Tin tin_t , SensorInfo sen){
	  vector<coor3d> vertiSet = {tin_t.first, tin_t.second, tin_t.third};
	  bool isCover = true;
	  for(auto e : vertiSet){
	    
	    double ver_to_sen =  sqrt ( pow( (e.x- sen.x),2)+ 
					pow( (e.y- sen.y),2)+
					pow( (e.z- sen.z),2)) ;
	    double range = sen.sensorRadius;
	    //trace() << ver_to_sen << ": " << (e.x-sen.x)
	    //	    <<", "<<(e.y-sen.y)<<", "<<(e.z-sen.z);
	    if( ver_to_sen > range){
	    
	      isCover = false; 
	      break;
	    }


	  }
	  return isCover;
	}
	double LOS(double xt,double yt,double zt, double xr,double yr, double zr);
	coor3d givingElev (coor3d target) {
	  coor3d temp =  target;
	  int max_row = DEM.size();
	  int max_column = (DEM[0]).size();
	  int row = std::min((int ) (target.y/cellWidth), max_row-1);
	  int column =std::min( ( int) (target.x / cellWidth), max_column -1);
	 temp.z = DEM[row][column];
	 
	  return temp;
	}

	// i, j pure coor()
	vector<int> findTinIdFromSqtCoor(int i, int j) {
	  vector<int> rst(2,0);
	  int sqtId = std::max(j, 0) * NO_TIN_D + i;
	  rst[0] = sqtId * 2;
	  rst[1] = sqtId *2 +1;
	  return rst;
	}

	double findNextObstacle(double xt,double yt,double zt, double xr,double yr, double zr);
	double calculatePLDObstacles(vector<Obstacle> Obstacles);
	void updateNeighborTable(int id, double rssi, double energy);
	Solution * findBestCompromiseSolution(SolutionSet *set);
	
};

#endif	

		

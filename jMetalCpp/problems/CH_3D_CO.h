#ifndef _CH_3D_CO_H_
#define _CH_3D_CO_H_

#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <list>
#include <Problem.h>
#include <RealSolutionType.h>
#include <BinaryRealSolutionType.h>
#include <Solution.h>
#include <queue>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <list>
#include <fstream>
#include <Problem.h>
#include <Solution.h>
#include <random>
#include <BinarySolutionType.h>
#include <boost/tokenizer.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <string>
    struct coor3d {
    coor3d(double X, double Y, double Z):
      x(X), y(Y), z(Z){}
    double x;
      double y;  
     double z;
    };
    struct Tin //triangular irregular network
    {
    Tin(int ID, coor3d FIRST, coor3d SECOND, coor3d THIRD): id(ID), first(FIRST), second(SECOND), third(THIRD) {}
      int id;
      coor3d  first;
      coor3d second;
      coor3d third;
    };
    struct SensorInfo
    {
    SensorInfo(): id(0),x(0.0),y(0.0),z(0.0),energy(0.0),sensorRadius(0.0){}
	    int id;
	    double x;
	    double y;
	    double z;
	    double energy;
	    double sensorRadius;
    };

class CH_3D_CO : public Problem
{
	int networkSize;
	double numOfCubes;
		
	ofstream myfile;


	int uncover;
	vector<int> sleepNodes;
	vector<int> activeNodes;
	vector<int> clusterHeads;
	vector<int> configuration;

	multimap<int,int> network;

	vector <SensorInfo> Sensors;
	vector <vector<double> > adjacencyMatrix;
	vector <int> coverageMatrix;
	vector <vector<double> > overlappingMatrix;
	vector<vector<int> > coverageMappingMatrix ;

  public:
	

	
	CH_3D_CO(vector< vector<double>> adjacencyM, vector<SensorInfo> sensors);
	CH_3D_CO(vector< vector<double>> &adjacencyM, 
		     const vector<SensorInfo> &sensors, 
		     vector<int>& coverageM, 
		     vector<vector<int>> &coverageMapping
		 );
	~CH_3D_CO();
	static double _calOverlayArea(SensorInfo lsensor , SensorInfo rsensor);
	double  evaluateCoverageRedundancy();
	double clusterTheNetwork();

	//double  getCoverage();
	double getCompactness();
	
	void evaluate(Solution *solution);

};

#endif

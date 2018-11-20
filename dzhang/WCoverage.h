#ifndef _WCOVERAGE_H_
#define _WCOVERAGE_H_

//#include <stdio.h>
//#include <stdlib.h>
#include <utility>
#include <cmath>
#include <queue>
#include <vector>
#include <assert.h>
#include <fstream>

using namespace std;
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
    SensorInfo():id(0),x(0),y(0),z(0),energy(0),sensorRadius(20){}
      //          SensorInfo(int id_, double x_, double y_, double z_, double se_r_): id(id_), x(x_), y(y_), z(z_), energy(0.0),sensorRadius(se_r_){};
    	    int id;
    	    double x;
    	    double y;
    	    double z;
    	    double energy;
    	    double sensorRadius;
    };

class WCoverage {

 private: 
  int NO_TIN_D ;
  int width_of_terrain;
	vector<SensorInfo> Sensors;
	vector<vector<double> > DEM;	
	vector<int>  coverageMatrix;
	vector<vector<int>> coveringMappingMatrix;
	vector<Tin> Tin_Matrix;
	
	vector<int> CHs;
	coor3d givingElev (coor3d target) {
	  int cellWidth = 5;
	  coor3d temp =  target;
	  int max_row = DEM.size();
	  int max_column = (DEM[0]).size();
	  int row = std::min((int ) (target.y/cellWidth), max_row-1);
	  int column =std::min( ( int) (target.x / cellWidth), max_column -1);
	 temp.z = DEM[row][column];
	 
	  return temp;
	}
	
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
	void updateSingleCoverage(vector<vector<int>> &board, const SensorInfo sen );
	void bfs (vector<vector<int>> &board, int row, int col, double coorx, double coory, double sen_range);
	void updateTinMatrix();
	
	// i, j pure coor()
	vector<int> findTinIdFromSqtCoor(int i, int j) {
	  vector<int> rst(2,0);
	  int sqtId = std::max(j, 0) * NO_TIN_D + i;
	  rst[0] = sqtId * 2;
	  rst[1] = sqtId *2 +1;
	  return rst;
	}
	void testingFun(){
	  int i=0;
	  ofstream os;
	  os.open("wco-pso-cmap.txt"); 
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





 public:
	WCoverage( const vector<SensorInfo> & sensors,
		   const  vector<vector<double> > & dem, int size_of_tin =20, int width = 100
		     );
	void updateCoverageMatrix();
	// void updateContext();
	double  evaluateSingleCoverRedun(const int chs_id);
	double evaluateCoverageRedun(const vector<int> &chs_id);
	~WCoverage(){
	  Sensors.clear();
	  DEM.clear();
	  coverageMatrix.clear();
	  Tin_Matrix.clear();
	  CHs.clear();
	  coveringMappingMatrix.clear();
	};

};


#endif

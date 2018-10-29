#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <algorithm>
#define INFIN 999
 
using namespace std;
 
class Dijkstra
{
    	private:
        vector< vector<double>> adjacencyMatrix;
        vector<int> predecessor;
        vector<double> distance;
        vector<bool> mark; 
        int source;
        int numOfVertices;
        ofstream myfile;
	vector <int> clusterHeads;
	string routes;
        stringstream out;
	
    	public:
	~Dijkstra();
        void read(vector< vector<double>> adjacencyM, vector <int> clusterHeads);
        void initialize();
        int getClosestUnmarkedNode();
        void calculateDistance();
        double output();
        void printPath(int);
	string returnRoutes();
};
 
 


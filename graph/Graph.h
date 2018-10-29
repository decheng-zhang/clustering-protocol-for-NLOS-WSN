#include <map> 
#include <vector>
#include <iostream>  
#include <algorithm>

using namespace std;
        
typedef std::pair< std::pair<int,int>, double> data_t;
typedef std::map< std::pair<int,int>, double>::const_iterator clm_it;  

class Graph  
{  
      
private:  
    int nNodes;  
    int nEdges;  
    bool isDirected;  
      
    std::map< std::pair<int,int>, double > link_map;
		      
public:  
    Graph( const int& nodes, const bool& directed );  
    ~Graph();  
      
    int V() const;  
    int E() const;  
    bool directed() const;  
    void insert( const int& s, const int& t, const double& weight );   
    void remove( const int& s, const int& t );  
    bool edge( const int& s, const int& t);  
	std::vector<data_t> sorted_edges();
      
    clm_it getEdge( const int& s, const int& t);      
    void show() const;
    bool lequal(const data_t t1, const data_t t2); 
    vector<data_t> Kruskal( Graph G );    
};  

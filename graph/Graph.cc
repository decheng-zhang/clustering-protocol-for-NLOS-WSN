#include <iostream>  
#include <algorithm>  
#include "Graph.h"  
      
template<class T>
struct less_second
: std::binary_function<T,T,bool>
{
   inline bool operator()(const T& lhs, const T& rhs)
   {
      return lhs.second < rhs.second;
   }
};

Graph::Graph( const int& nodes, const bool& directed )  
{  
    nNodes = nodes;  
    nEdges = 0;  
    isDirected = directed;  
}  
      
Graph::~Graph()  
{}  
      
clm_it Graph::getEdge( const int& s, const int&t )  
{  
    std::pair<int,int> p( s, t );  
    clm_it mit = link_map.find( p );  
      
    return mit;  
}  
      
// Check if link exists between end nodes  
bool Graph::edge( const int& s, const int& t)  
{  
    return getEdge( s, t ) != link_map.end();  
}  
      
// Add a link: map node pair with weight  
void Graph::insert( const int& s, const int& t, const double& weight )  
{  
    std::pair<int,int> p1( s, t );  
    std::pair<int,int> p2( t, s );  
    link_map[ p1 ] = weight;  
      
    if ( !isDirected )  
    {          
        link_map[ p2 ] = weight;  
    }  
      
    nEdges++;  
}  
      
// Remove selected link - remove both directions  
// if graph is not directed  
void Graph::remove( const int& s, const int& t )  
{  
    std::pair<int,int> p1( s, t );  
    std::map< std::pair<int,int>, double>::iterator mit = link_map.find( p1 );  
    link_map.erase( mit );  
      
    if ( !isDirected )  
    {  
    std::pair<int,int> p2( t, s );  
        mit = link_map.find( p2 );  
        link_map.erase( mit );  
    }  
      
    nEdges--;  
}  
      
// Print the network topology  
void Graph::show() const  
{  
    clm_it mit = link_map.begin();  
      
    for ( ; mit != link_map.end(); ++ mit )  
    {  
        std::pair<int,int> p = (*mit).first;  
        std::cout << p.first  << " " <<  
                p.second << " " <<   
                (*mit).second << std::endl;  
    }  
}  
      
// Return true if graph is directed; false otherwose  
bool Graph::directed() const  
{  
    return isDirected;  
}  
      
// Return number of network nodes  
int Graph::V() const  
{  
    return nNodes;  
}  
      
// Return number of edges  
int Graph::E() const  
{  
    return nEdges;  
}  

// Sort the weighted edges
std::vector<data_t> Graph::sorted_edges()
{
	std::vector< data_t > vec(link_map.begin(), link_map.end());
	
	std::sort(vec.begin(), vec.end(), less_second<data_t>());
	
	return vec;
}

bool Graph :: lequal(const data_t t1, const data_t t2)
{
	double d1 = t1.second;
	double d2 = t2.second;
	return d1 < d2;
}

vector<data_t> Graph :: Kruskal( Graph G )
{
	int i, number;
	std::vector<data_t> mst;
	mst.clear();

	const int max = 1000;
	int cicles[ max ];


	std::vector<data_t> edges = G.sorted_edges();

	for( i=0; i < G.V(); i++) cicles[ i ] = i;

	while( (int) mst.size() < ( G.V() - 1) && edges.size() )
	{
		data_t dt = edges[ 0 ];
		std::pair<int,int> p = dt.first;
		int vi = p.first;
		int vj = p.second;

		if( cicles[ vi ] != cicles[vj] )
		{
			number = cicles[ vj ];
			for( i=0; i < G.V(); i++ ) 
			{
				if( cicles[ i ] == number )
				{
					cicles[ i ] = cicles[ vi ];
				}
			}

			mst.push_back( dt );
		}

		edges.erase( edges.begin(), edges.begin() + 1 );
    }

	return mst;
}

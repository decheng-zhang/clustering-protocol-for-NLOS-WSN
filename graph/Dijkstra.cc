#include<Dijkstra.h>

void Dijkstra::read(vector< vector<double>> adjacencyM, vector <int> CH)
{
   // myfile.open("Dijkstra.txt",std::ios_base::app);
    
    CH.push_back(0);
   
    sort (CH.begin(),CH.end());
    CH.erase (unique(CH.begin(),CH.end()),CH.end());

    numOfVertices = CH.size();

   // myfile << "Cluster Heads are : \n";
    for (int i = 0 ; i < numOfVertices ; i++)
    {
	vector<double> r;
        predecessor.push_back(-1);
        distance.push_back(INFIN);
        mark.push_back(false);

	clusterHeads.push_back(CH[i]);
   
     //   myfile << CH[i] << "...";

	for (int j = 0 ; j < numOfVertices ; j++)
	{
		r.push_back(0);
	}
	adjacencyMatrix.push_back(r);
    }

   // myfile << "\n";
   
    for(int i = 0; i < numOfVertices ; i++) 
    {
        for(int j = 0; j < numOfVertices ; j++) 
        {
	    int k = clusterHeads[i];
	    int m = clusterHeads[j];
	    if (adjacencyM[k][m] < 0)
	    {
	    	adjacencyMatrix[i][j] = adjacencyM[k][m] / -100;
	    }
	    // myfile << "Link from "<< m << " to " << k << " has RSSI = " << adjacencyMatrix[i][j] <<"\n";
	    
	}
    }

    source = 0;
    distance[source]= 0.0;
}

Dijkstra::~Dijkstra() 
{
  //myfile.close();

}
 
void Dijkstra::initialize()
{
    for(int i = 0 ; i < numOfVertices ; i++) 
    {
        mark[i] = false;
        predecessor[i] = -1;
        distance[i] = INFIN;
    }
    distance[source]= 0;
}
 
 
int Dijkstra::getClosestUnmarkedNode()
{
    int minDistance = INFIN;
    int closestUnmarkedNode;

    for(int i = 0 ; i < numOfVertices;i++) 
    {
        if((!mark[i]) && ( minDistance >= distance[i])) 
        {
            minDistance = distance[i];
            closestUnmarkedNode = i;
        }
    }
    return closestUnmarkedNode;
}
 
 
void Dijkstra::calculateDistance()
{
    initialize();
    int minDistance = INFIN;
    int closestUnmarkedNode;
    int count = 0;

    while(count < numOfVertices) 
    {
        closestUnmarkedNode = getClosestUnmarkedNode();
        mark[closestUnmarkedNode] = true;

        for(int i = 0 ; i < numOfVertices ; i++) 
        {
            if((!mark[i]) && (adjacencyMatrix[i][closestUnmarkedNode]>0) ) 
            {
                if(distance[i] > distance[closestUnmarkedNode]+adjacencyMatrix[i][closestUnmarkedNode]) 
                {
                    distance[i] = distance[closestUnmarkedNode]+adjacencyMatrix[i][closestUnmarkedNode];
                    predecessor[i] = closestUnmarkedNode;
                }
            }
        }
	
        count++;
    }
}
 
 
void Dijkstra::printPath(int node)
{
    int n = 0;
    if(clusterHeads[node] == source)
        n=1;
    else if(predecessor[node] == -1)
        n=2;
    else 
    {
        out <<  clusterHeads[node] << " ";
        printPath(predecessor[node]);
       // myfile << clusterHeads[node] <<"..";
    }
}
 
 
string Dijkstra::returnRoutes()
{
	return routes;
}
double Dijkstra::output()
{
    double totalTreeCost = 0; int n;
   
    for(int i=0;i<numOfVertices;i++) 
   {
        if(i == source)
            n = 1;
        else
           {
               printPath(i);
               totalTreeCost += distance[i];
               out << "0 ;";
           }
        
       // myfile<<"->"<<distance[i]<<endl;
    }

   if (totalTreeCost == 0) totalTreeCost = 1000;
   routes = out.str();
  // myfile << "totalTreeCost " << totalTreeCost << "\n"; 
   return totalTreeCost;
    
}

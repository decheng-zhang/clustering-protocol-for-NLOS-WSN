#include <CCP_LOS_3D.h>

	CCP_LOS_3D :: CCP_LOS_3D(vector< vector<double>> coverageM, vector< vector<double>> overlappingM, vector< vector<double>> adjacencyM, vector<SensorInfo> sensors)
	{
		
		networkSize = sensors.size();
		numberOfObjectives_  = 5;
		numberOfConstraints_ = 0;
		numOfCubes = coverageM.size();
		myfile << "numOfCubes"<< numOfCubes << "\n";

		problemName_         = "CCP_LOS_3D";
			
		numberOfVariables_   = networkSize;

		for (int i = 0 ; i < numOfCubes ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}
			coverageMatrix.push_back(r);
		}

		for (int i = 0 ; i < numOfCubes ; i++)
		{
			for (int j = 0 ; j < networkSize ; j++)
			{
				coverageMatrix[i][j] = coverageM[i][j];
				
			}
		}

		
		for (int i = 0 ; i < networkSize ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}
			sleepNodes.push_back(0);
			activeNodes.push_back(0);
			clusterHeads.push_back(0);
			adjacencyMatrix.push_back(r);
			overlappingMatrix.push_back(r);
			Sensors.push_back(SensorInfo());
		}

		for (int i = 0 ; i < networkSize; i++)
		{
			Sensors[i].id = sensors[i].id;
			Sensors[i].x = sensors[i].x;
			Sensors[i].y = sensors[i].y;
			Sensors[i].z = sensors[i].z;
			Sensors[i].energy = sensors[i].energy;

			for (int j = 0 ; j < networkSize ; j++)
			{
				adjacencyMatrix[i][j] = adjacencyM[i][j];
				overlappingMatrix[i][j] = overlappingM[i][j];
			}
		}
		
		solutionType_ = new BinarySolutionType(this);
		length_       = new int[numberOfVariables_];
  		for (int i = 0; i < numberOfVariables_; i++) length_  [i] = 2 ;

	}

	double CCP_LOS_3D :: clusterTheNetwork()
	{
		network.clear();
		int numberOfClusteredNodes = 0;
		
		for (int i = 1 ; i < activeNodes.size() ; i++)
     		{
			double maxRSSI = -10000;
			int cHead = 0;

			for (int j = 0; j < clusterHeads.size() ; j++)
       			{
				int node2 = activeNodes[i];
				int node1 = clusterHeads[j];

				double rssi = adjacencyMatrix[node1][node2];

				if (rssi != 0)
				{
					if ( (rssi > maxRSSI) && (rssi >= -95) && (node2 != node1 ))
       					{
							maxRSSI = rssi;
       							cHead = clusterHeads[j];
					}
				}
			}

                        if (maxRSSI != -10000)
			{
			  network.insert(pair<int,int>(activeNodes[i],cHead));
			  numberOfClusteredNodes++;
			}
		}

		return numberOfClusteredNodes;
	}

	double CCP_LOS_3D :: getRedunduncy()
	{
		double totalEffect = 0; 
		double monitoredBy = 0;
		double monitoredByOld = 0;
		double oldTotalEffect = 0; 	
		double sensorEffect = 0; 
            
	        double nmCells = 0;

		 vector<vector<double> > tmpCoverageMatrix(numOfCubes, vector<double>(networkSize,0));

                for (int i = 0 ; i < numOfCubes; i++)
		{
                        monitoredBy = 0;
			monitoredByOld = 0;
			for (int j = 1 ; j < networkSize ; j++)
			{
				tmpCoverageMatrix[i][j] = coverageMatrix[i][j];
				
				if(std::find(sleepNodes.begin(),sleepNodes.end(),j)!=sleepNodes.end()) tmpCoverageMatrix[i][j] = 0;
				monitoredBy+=tmpCoverageMatrix[i][j];
				monitoredByOld+=coverageMatrix[i][j];
                         }
			  totalEffect+= monitoredBy;
		          oldTotalEffect+= monitoredByOld;
			  if ((monitoredBy == 0) && (monitoredByOld != 0)) uncover = 1; 
		}

             double avgRedunduncy = totalEffect / numOfCubes;
	   if (uncover == 1) avgRedunduncy = 1000000;
            return avgRedunduncy;    
    	}

	double CCP_LOS_3D :: getOverlapping()
	{
		double totalOverlap;
		vector<vector<double> > tmpOverlappingMatrix(networkSize, vector<double>(networkSize,0));

		for (int i = 1 ; i < networkSize; i++)
		{
                   	for (int j = 1 ; j < networkSize ; j++)
			{
				tmpOverlappingMatrix[i][j] = overlappingMatrix[i][j];
				if(std::find(sleepNodes.begin(),sleepNodes.end(),j)!=sleepNodes.end()) tmpOverlappingMatrix[i][j] = 0;
				totalOverlap+= overlappingMatrix[i][j];
	                }
		}
	       double ratio = totalOverlap / networkSize;
	       return ratio;    
 	}

	double CCP_LOS_3D :: getCoverageRatio()
	{
		bool isCovered = false;
		double nOfCoveredCubes = 0;

		vector<vector<double> > tmpCoverageMatrix(numOfCubes, vector<double>(networkSize,0));

                for (int i = 0 ; i < numOfCubes; i++)
		{
                       isCovered = false;

			for (int j = 1 ; j < networkSize ; j++)
			{
				tmpCoverageMatrix[i][j] = coverageMatrix[i][j];
				if(std::find(sleepNodes.begin(),sleepNodes.end(),j)!=sleepNodes.end()) tmpCoverageMatrix[i][j] = 0;
				if (tmpCoverageMatrix[i][j] == 1) isCovered = true;
				
                        }
			if (isCovered) nOfCoveredCubes++;
		}
	       double ratio = numOfCubes - nOfCoveredCubes;
	      return ratio;    
 	}

	double CCP_LOS_3D :: getCompactness()
	{
		double maxRSSI = -100;
		double maxCompactness = -100;
		double maxAvgRSSI = 0;
		double totalRSSI = 0;
				
		for (int i = 0 ; i < clusterHeads.size() ; i++)
		{
		    vector<int> members;

		    for (multimap<int,int>::iterator iter = network.begin(); iter != network.end(); iter++)
		    {
			if (iter->second == clusterHeads[i]) members.push_back(iter->first);
		    }
			
		    double clusterCompactness = 0;

		    for(int j = 0 ; j < members.size() ; j++)
		    {
			int node1 = clusterHeads[i];
			int node2 = members[j];
			clusterCompactness += (adjacencyMatrix[node1][node2] / maxRSSI); 
			totalRSSI +=  adjacencyMatrix[node1][node2] / maxRSSI;
		    }

		    if (members.size() > 0)
		    {
			maxAvgRSSI = clusterCompactness / members.size();
		    }
		    else
		    {
			maxAvgRSSI = 100;
		    }

		    if (maxAvgRSSI > maxCompactness)
		    {
				maxCompactness = maxAvgRSSI;			
		    }

		}
		if (clusterHeads.size() == 0) maxCompactness = 100;
		
		return maxCompactness;
 	}


	
	
	CCP_LOS_3D :: ~CCP_LOS_3D() 
	{
		delete []length_ ;
  		delete solutionType_ ;
                adjacencyMatrix.clear();
  	}

	
	void CCP_LOS_3D :: evaluate(Solution *solution) 
	{
	     double totalEnergy = 0;
	    
	     uncover = 0;
	     network.clear();
	     clusterHeads.clear();
	     sleepNodes.clear();
	     activeNodes.clear();
	     configuration.clear();

	     Binary * variable ;
	     for (int i = 1; i < numberOfVariables_ ; i++)
	     {
     		 variable = (Binary *)(solution->getDecisionVariables()[i]) ;
               	 if (variable->cardinality() == 0){ sleepNodes.push_back(i);}
	 	 if (variable->cardinality() == 2){ clusterHeads.push_back(i);totalEnergy+= 18720 / Sensors[i].energy;}
		 if (variable->cardinality() == 1){ activeNodes.push_back(i); totalEnergy+= 18720 / Sensors[i].energy;}
	     }

            double noOfCHs = clusterHeads.size();
	    double noActiveNodes = activeNodes.size();	
	    double cnOfClusteredNodes = clusterTheNetwork();
            double compactness = getCompactness();
	    double avgRemainingEnergy = totalEnergy;
	    double numberOfUnClusteredNodes = noActiveNodes - cnOfClusteredNodes;
	    double coverageRatio = getCoverageRatio(); 
	    double avgRedunduncy = getRedunduncy();
	    double avgOverlapp = getOverlapping();
	    
	    solution->setObjective(0,noOfCHs);
	    solution->setObjective(1,totalEnergy);
	    solution->setObjective(2,compactness);
	    solution->setObjective(3,avgRedunduncy);
	    solution->setObjective(4,numberOfUnClusteredNodes);
	}



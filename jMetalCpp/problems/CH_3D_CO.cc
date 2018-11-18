#include <CH_3D_CO.h>
#include <assert.h>
	CH_3D_CO :: CH_3D_CO(vector< vector<double>> adjacencyM, vector<SensorInfo> sensors)
	{
		networkSize = sensors.size();
		numberOfObjectives_  = 4;
		numberOfConstraints_ = 0;
		
		problemName_         = "CH_3D_CO";
		
		numberOfVariables_   = networkSize;
		
		for (int i = 0 ; i < networkSize ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}
			clusterHeads.push_back(0);
			adjacencyMatrix.push_back(r);
			Sensors.push_back(SensorInfo());
		}

		for (int i = 0 ; i < networkSize; i++)
		{
			Sensors[i].id = sensors[i].id;
			Sensors[i].x = sensors[i].x;
			Sensors[i].y = sensors[i].y;
			Sensors[i].z = sensors[i].z;
			Sensors[i].energy = sensors[i].energy;
			//default sensor radius (binary coverage model)
                        Sensors[i].sensorRadius = 10;
			for (int j = 0 ; j < networkSize ; j++)
			{
				adjacencyMatrix[i][j] = adjacencyM[i][j];
			}
		}

		solutionType_ = new BinarySolutionType(this);
		length_       = new int[numberOfVariables_];
  		for (int i = 0; i < numberOfVariables_; i++) length_  [i] = 1 ;

	}
	
CH_3D_CO :: CH_3D_CO(vector< vector<double>> &adjacencyM, 
		     const vector<SensorInfo> &sensors, 
		     vector<int> &coverageM, 
		     vector<vector<int>> &coverageMapping
		     )
	{


		networkSize = sensors.size();
		numberOfObjectives_  = 5;
		numberOfConstraints_ = 0;
		coverageMatrix = coverageM;
		problemName_         = "CH_3D_CO";
		coverageMappingMatrix = coverageMapping;
		numberOfVariables_   = networkSize;
		
		for (int i = 0 ; i < networkSize ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}
			clusterHeads.push_back(0);
			adjacencyMatrix.push_back(r);
			Sensors.push_back(SensorInfo());
		}

		for (int i = 0 ; i < networkSize; i++)
		{
			Sensors[i].id = sensors[i].id;
			Sensors[i].x = sensors[i].x;
			Sensors[i].y = sensors[i].y;
			Sensors[i].z = sensors[i].z;
			
	
			Sensors[i].energy = sensors[i].energy;
			
			//assert(sensors[i].sensorRadius != 0.0 );
                        Sensors[i].sensorRadius = sensors[i].sensorRadius;
			for (int j = 0 ; j < networkSize ; j++)
			{
				adjacencyMatrix[i][j] = adjacencyM[i][j];
			}
		}

		solutionType_ = new BinarySolutionType(this);
		length_       = new int[numberOfVariables_];
  		for (int i = 0; i < numberOfVariables_; i++) length_  [i] = 1 ;

	}
          double CH_3D_CO :: _calOverlayArea(SensorInfo l , SensorInfo r) {
	      double distance = sqrt(pow((l.x-r.x),2) + pow((l.y-r.y),2) + pow((l.z-r.z), 2) );
	      double large =0.0, small = 0.0;

	      if(l.sensorRadius > r.sensorRadius) {

		large = l.sensorRadius;
		small = r.sensorRadius;

	      } else {
		large = r.sensorRadius;
		small = l.sensorRadius;

	      }

	      if (distance < large-small ) {
		return 4 * M_PI * pow(small,3) / 3;
	      } else if (distance <= large + small) {
		return (M_PI * pow((large + small - distance), 2) 
			*(pow((large + small + distance), 2) - 
			  4 * (pow(large,2) + pow(small, 2) - small* large))) / (12 * distance);
	      } else {
		return 0;    
	      }




	}
	double CH_3D_CO :: clusterTheNetwork()
	{
		network.clear();
		int numberOfClusteredNodes = 0;
		
		for (int i = 1 ; i < networkSize ; i++)
     		{
			double maxRSSI = -10000;
			int cHead = 0;

			for (int j = 0; j < clusterHeads.size() ; j++)
       			{
				int node2 = i;
				int node1 = clusterHeads[j];

				double rssi = adjacencyMatrix[node1][node2];

				if (rssi != 0)
				{
					if ( (rssi > maxRSSI) && (node2 != node1 ))
       					{
							maxRSSI = rssi;
       							cHead = clusterHeads[j];
					}
				}
			}

                        if (maxRSSI != -10000)
			{
			  network.insert(pair<int,int>(i,cHead));
			  numberOfClusteredNodes++;
			}
		}

		return numberOfClusteredNodes;
	}

double CH_3D_CO:: evaluateCoverageRedundancy(){

  int totalCoverage = 0;
  int sizeOfCH  = clusterHeads.size();
  
  for(const auto & ele : coverageMatrix){
    if (ele)   totalCoverage++;
  }
  double totalCoverageRatio =(double) totalCoverage  / (double)coverageMatrix.size();

  
  double coverageRedun=0.0 ;
  if(sizeOfCH != 0){

    for(const auto &ele : clusterHeads){
      if(ele < coverageMappingMatrix.size()){
	auto coveredTinSet = coverageMappingMatrix[ele];
	if(coveredTinSet.size() > 0 ){
	  double denominator = 0.0;
	  double numerator  = 0.0;
	  for( auto & coveredTin:  coveredTinSet ) {
	    numerator ++;
	     int coveredTimes  = coverageMatrix[coveredTin];
	     if(coveredTimes < 1 || coveredTimes >800){
	       
	       cout<<" Calculaton error, at least covered by self"<<endl;
	       exit(-1);
	     } else {
	       denominator += 1.0 / (double)coveredTimes ;
	     }
	   }
	  assert(denominator != 0.0);
	  coverageRedun += numerator/ denominator;
	}else{
	  //does nothing,  no covertin, no coverage redundancy;
	}
      }else {
	throw "EA setting problem: sensor id exceed coveragemappingmatrix length!";
      }
    }      
  } else{
    //everyone is CH
    for(int idx_ch =0;idx_ch < coverageMatrix.size();idx_ch ++){
    auto coveredTinSet = coverageMappingMatrix[idx_ch];
	if(coveredTinSet.size() > 0 ){
	  double denominator = 0.0;
	  double numerator  = 0.0;
	  for( auto & coveredTin:  coveredTinSet ) {
	    numerator ++;
	     int coveredTimes  = coverageMatrix[coveredTin];
	     if(coveredTimes < 1 ){
	       throw "Calculaton error, at least covered by self";
	     } else {
	       denominator += 1 / coveredTimes ;
	     }
	   }
	  coverageRedun += numerator/ denominator;
	}else{
	  //does nothing,  no covertin, no coverage redundancy;
	}
    }

}
  int outsideNumerator = (sizeOfCH ==0)? coverageMatrix.size() : sizeOfCH;
  return (double)outsideNumerator / coverageRedun;

};
	double CH_3D_CO :: getCompactness()
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
			maxAvgRSSI = 0;
		    }

		    if (maxAvgRSSI > maxCompactness)
		    {
				maxCompactness = maxAvgRSSI;			
		    }

		}
		if (maxCompactness == -100) maxCompactness = 100;
		return maxCompactness;
 	}

	CH_3D_CO :: ~CH_3D_CO() 
	{
		delete []length_ ;
  		delete solutionType_ ;
                adjacencyMatrix.clear();
  	}

	
	void CH_3D_CO :: evaluate(Solution *solution) 
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

                 if (variable->getIth(0))
		 {
		   	totalEnergy+= 18720 / Sensors[i].energy;
			clusterHeads.push_back(i);

                }
	    }
	
	      double noOfCHs = clusterHeads.size();
	      if (noOfCHs == 0) noOfCHs = networkSize;
	      double cnOfClusteredNodes = clusterTheNetwork();
	      double compactness = getCompactness();
	      double avgRemainingEnergy = totalEnergy;
	      double coverRedundancy = evaluateCoverageRedundancy();
	      assert(coverRedundancy != 0.0); //l310
	      double numberOfUnClusteredNodes = networkSize - cnOfClusteredNodes;
	      // minimize the no of chs
	      solution->setObjective(0,noOfCHs);
	      //I guess (maxmize link quality between inter-cluster comm)
	      solution->setObjective(1,compactness);
	      //maximize remaining energy
	      solution->setObjective(2,avgRemainingEnergy);
	      //minimize unclustered sensors
	      solution->setObjective(3,numberOfUnClusteredNodes);
	      //Optimize the coverage
	      //minimize redundant coverage ratio
	      solution->setObjective(4,coverRedundancy);
	  
	  }



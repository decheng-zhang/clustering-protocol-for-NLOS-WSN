#include "LOS_CCP_3D.h"

Define_Module(LOS_CCP_3D);

void LOS_CCP_3D::startup()
{	
	isSink = par("isSink");
	slotLength = par("slotLength");	
	percentage = par("percentage");
	roundLength = par("roundLength");
	advPacketSize = par("advPacketSize");
	joinPacketSize = par("joinPacketSize");
	tdmaPacketSize = par("tdmaPacketSize");
	dataPacketSize = par("dataPacketSize");
	numberOfRounds = par("numberOfRounds");
	sensingRange = par("sensingRange");
	applicationID = par("applicationID").stringValue(); 
		
	roundNumber=0;
       	
	networkInfo.clear();	
	clusterMembers.clear();
	eligibleNodes.clear(); 
        CHs.clear();
	
	isCH = false;
	isCM = false;
	isActive = true;
	endFormClus = false;

	myCH = 0;
	myTDMATurn = 0;
	clusterLength = 0;

	theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");
	width = getParentModule()->getParentModule()->getParentModule()->par("field_x");
	length = getParentModule()->getParentModule()->getParentModule()->par("field_y");
	height = getParentModule()->getParentModule()->getParentModule()->par("field_z");
	initialEnergy = getParentModule()->getParentModule()->getSubmodule("ResourceManager")->par("initialEnergy");

	cubeWidth = 5 ;
	numOfCubes = pow ((width / cubeWidth), 3);

	Sensors.clear();

	declareOutput("No CC: Coverage Ratio");
	declareOutput("NO CCP: Average Overlapping");
	declareOutput("No CC: Average number nodes covering a cube");
	declareOutput("No CC: Minimum Number of Nodes covering a cube");
	declareOutput("No CC: Maximum Number of Nodes covering a cube");

	declareOutput("Coverage Ratio");
	declareOutput("Maximum Number of Nodes covering a cube");
	declareOutput("Minimum Number of Nodes covering a cube");
	declareOutput("Average Number of Nodes covering a cube");


	declareOutput("PDR");
	declareOutput("Total Received Packets");
        declareOutput("Total Generated Packets");
	
	declareOutput("Average number of CHs per round");
      	declareOutput("Average number of active nodes per round");
	declareOutput("Average number of unclustered nodes per round");
	
	
     	if (isSink)
	{
		initializeMatrices();	
		updateSensors();
		updateMatrices();
		updateCoverageMatrix();	
      	}

	readXMLparams();
	setTimer(START_ROUND,0.0);
}

void LOS_CCP_3D :: updateCoverageMatrix()
{
	double c1_x = 0;
	double c1_y = 0;
	double c1_z = 0;

	double c2_x = 0;
	double c2_y = 0;
	double c2_z = 0;

	
	double c3_x = 0;
	double c3_y = 0;
	double c3_z = 0;
		
	double c4_x = 0;
	double c4_y = 0;
	double c4_z = 0;

	double c5_x = 0;
	double c5_y = 0;
	double c5_z = 0;

	double c6_x = 0;
	double c6_y = 0;
	double c6_z = 0;

	
	double c7_x = 0;
	double c7_y = 0;
	double c7_z = 0;
		
	double c8_x = 0;
	double c8_y = 0;
	double c8_z = 0;

	double total = 0;
	double counter = 0;
	bool isCovered = false;	
	double minNodes = 1000000;
	double maxNodes = -1000000;

	double numberRounds = numberOfRounds;

	int cubeNumber = 0;
	int cubeCounter = 0;

	for (int i = 0 ; i < (int)(width / cubeWidth) ; i++)
	{

		c1_y = 0;
		c2_y = 0;
		c5_y = 0;
		c6_y = 0;			

		c3_y = cubeWidth;
		c4_y = cubeWidth;
		c7_y = cubeWidth;
		c8_y = cubeWidth;

		c5_z+= cubeWidth;
		c6_z+= cubeWidth;
		c7_z+= cubeWidth;
		c8_z+= cubeWidth;
		
		for (int j = 0 ; j < (int)(width / cubeWidth) ; j++)
		{
			 c1_x = 0;
			 c2_x = cubeWidth;
	   		 c3_x = 0;
			 c4_x = cubeWidth;
	   		 c5_x = 0;
	   		 c6_x = cubeWidth;
	  		 c7_x = 0;
	  		 c8_x = cubeWidth;
	
			for (int K = 0 ; K < (int)(width / cubeWidth) ; K++)
			{
				counter = 0;
				isCovered = false;
				
				for (int j = 0 ; j < networkSize ; j++)
				{
                                	coverageMatrix[cubeCounter][j] = 0;

	                		double farthest = -1;
					double s_x = Sensors[j].x ;
					double s_y = Sensors[j].y ;
					double s_z = Sensors[j].z ;

					double dist1 = sqrt (pow ((c1_x - s_x),2) + pow ((c1_y - s_y),2) + pow ((c1_z - s_z),2));
					double dist2 = sqrt (pow ((c2_x - s_x),2) + pow ((c2_y - s_y),2) + pow ((c2_z - s_z),2) );
					double dist3 = sqrt (pow ((c3_x - s_x),2) + pow ((c3_y - s_y),2) + pow ((c3_z - s_z),2) );
					double dist4 = sqrt (pow ((c4_x - s_x),2) + pow ((c4_y - s_y),2) + pow ((c4_z - s_z),2));
					double dist5 = sqrt (pow ((c5_x - s_x),2) + pow ((c5_y - s_y),2) + pow ((c5_z - s_z),2));
					double dist6 = sqrt (pow ((c6_x - s_x),2) + pow ((c6_y - s_y),2) + pow ((c6_z - s_z),2) );
					double dist7 = sqrt (pow ((c7_x - s_x),2) + pow ((c7_y - s_y),2) + pow ((c7_z - s_z),2) );
					double dist8 = sqrt (pow ((c8_x - s_x),2) + pow ((c8_y - s_y),2) + pow ((c8_z - s_z),2));

					if ( dist1 > farthest)	farthest = dist1;
					if ( dist2 > farthest)	farthest = dist2;
					if ( dist3 > farthest)	farthest = dist3;
					if ( dist4 > farthest)	farthest = dist4;
					if ( dist5 > farthest)	farthest = dist5;
					if ( dist6 > farthest)	farthest = dist6;
					if ( dist7 > farthest)	farthest = dist7;
					if ( dist8 > farthest)	farthest = dist8;

					if (farthest < sensingRange) 
					{ 
						coverageMatrix[cubeCounter][j] = 1;
						isCovered = true;
						//trace() << "Sensor no " << j << " covers cube no " << cubeCounter << "\n";
					}
                                	counter+= coverageMatrix[cubeCounter][j];

				}

				 if (isCovered) collectOutput("No CC: Coverage Ratio","",1/numOfCubes);
				 if (counter < minNodes) minNodes = counter;
				 if (counter > maxNodes) maxNodes = counter;
                                 total+= counter;


				c1_x+= cubeWidth;
				c2_x+= cubeWidth;
				c3_x+= cubeWidth;
				c4_x+= cubeWidth;
				c5_x+= cubeWidth;
				c6_x+= cubeWidth;
				c7_x+= cubeWidth;
				c8_x+= cubeWidth;

				cubeCounter++;
			}
			c1_y+= cubeWidth;
			c2_y+= cubeWidth;
			c5_y+= cubeWidth;
			c6_y+= cubeWidth;

			 c3_y+= cubeWidth;
			 c4_y+= cubeWidth;
			 c7_y+= cubeWidth;
			 c8_y+= cubeWidth;

		}
		

		c1_z+= cubeWidth;
		c2_z+= cubeWidth;
		c3_z+= cubeWidth;
		c4_z+= cubeWidth;
	}

	 double avgRedunduncy = total/numOfCubes;
	 collectOutput("No CC: Coverage Control: Average number nodes covering a cube","",avgRedunduncy);
	 collectOutput("No CC: Minimum Number of Nodes covering a cube","",minNodes/numberRounds);
	 collectOutput("No CC: Maximum Number of Nodes covering a cube","",maxNodes/numberRounds);

}

void LOS_CCP_3D :: initializeMatrices()
{
	for (int i = 0 ; i < networkSize ; i++)
	{
		vector<double> r;

		for (int j = 0 ; j < networkSize ; j++)
		{
			r.push_back(0);
		}

		adjacencyMatrix.push_back(r);
		overlappingMatrix.push_back(r);
		Sensors.push_back(SensorInfo());
        }

	for (int j = 0 ; j < numOfCubes ; j++)
	{	
		vector<double> c;
				
		for (int i = 0 ; i < networkSize  ; i++)
		{
			c.push_back(0);
		}
		coverageMatrix.push_back(c);
	}

}

void LOS_CCP_3D :: updateSensors()
{
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;

		Sensors[0].id = 0;
		Sensors[0].x = 0;
		Sensors[0].y = 0;
		Sensors[0].z = 0;

		for (int j = 0 ; j < networkSize ; j++)
		{
		 	n = theSNModule->getSubmodule("node",j);
			r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
			l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
				
			double s_x = l->getLocation().x;
			double s_y = l->getLocation().y;
			double s_z = l->getLocation().z;
			

			Sensors[j].id = j;
			Sensors[j].x = s_x;
			Sensors[j].y = s_y;
			Sensors[j].z = s_z;
		 }
}

void LOS_CCP_3D :: updateMatrices()
{
	float PLd;		
	float bidirectionalPathLossJitter; 

	float pathLossExponent = 2.4;
	float sigma = 4.0;
	float bidirectionalSigma = 1.0;
	float PLd0 = 55;
	float d0 = 1.0;
	float signalDeliveryThreshold = -100;
	float maxTxPower = 0.0;


	float distanceThreshold = d0 *	pow(10.0,(maxTxPower - signalDeliveryThreshold - PLd0 + 3 * sigma) / (10.0 * pathLossExponent));

	double x1, x2, y1, y2, z1, z2, dist, rssi, overlappedArea, totalOverlap;

	for (int i = 0 ; i < networkSize ; i++)
	{
		x1 = Sensors[i].x;
		y1 = Sensors[i].y;
		z1 = Sensors[i].z;

		for (int j = 0 ; j < networkSize ; j++)
		{	
			x2 = Sensors[j].x;
			y2 = Sensors[j].y;
			z2 = Sensors[j].z;

			dist = sqrt (pow((x2 - x1),2) + pow((y2 - y1),2) + pow((z2 - z1),2));

			overlappedArea = findOverlappingArea(dist);
			
			if ( i == j ) PLd = 0; // Pathloss to self = 0
			else
			{
				if (dist > distanceThreshold)
				continue;

				if (dist < d0/10.0) 
				{
					PLd = 0;
					bidirectionalPathLossJitter = 0;
				}
				else 
				{
					PLd = PLd0 + 10.0 * pathLossExponent * log10(dist / d0) + normal(0, sigma);
					bidirectionalPathLossJitter = normal(0, bidirectionalSigma) / 2;
				}

				if (maxTxPower - PLd - bidirectionalPathLossJitter >= signalDeliveryThreshold) 
				{
					PLd += bidirectionalPathLossJitter;
				}

				if (maxTxPower - PLd + bidirectionalPathLossJitter >= signalDeliveryThreshold) 
				{
					PLd -= bidirectionalPathLossJitter;
				}
			}

			rssi = maxPower - PLd;
			
			adjacencyMatrix[i][j] = rssi;
			overlappingMatrix[i][j] = overlappedArea;	
			totalOverlap+= overlappedArea;
		}
	}

	double avgOverlap = totalOverlap / networkSize;
	//trace() << "Before CCP:  avgOverlap = " << avgOverlap << "\n";
	collectOutput("NO CCP: Average Overlapping","",avgOverlap);
}

double LOS_CCP_3D :: findOverlappingArea(double dist)
{
	double volume = 0;
	double factor = double(1.0)/double(12.0) ;
	
	if ( (dist == 0) || (dist >= (2 * sensingRange)) ) 
	{
		volume = 0;
	}
	
	else if (dist < (2 * sensingRange)) 
	{
		volume = factor * M_PI * ( ( 4 * sensingRange) + dist ) * pow( (( 2 * sensingRange) - dist) , 2);
	}

	return volume;
}

void LOS_CCP_3D :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		LOS_CCP_3D_Packet *netPacket = new LOS_CCP_3D_Packet("LOS_CCP_3D routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setLOS_CCP_3D_PacketKind(LOS_CCP_3D_DATA_PACKET);
		netPacket->setByteLength(dataPacketSize);
		netPacket->setSource(SELF_NETWORK_ADDRESS);
		encapsulatePacket(netPacket, pkt);
		
		if (!isCH && endFormClus)
		{
			stringstream buffer;
			buffer << myCH;
			string dst = buffer.str();
			netPacket->setDestination(dst.c_str());	
			bufferPacket(netPacket);
		}	
		else if (!isCH && !endFormClus) 
		{
			tempTXBuffer.push(netPacket);
		}
		else if (isCH) 
		{
			bufferAggregate.push_back(*netPacket);
		}
	}

	
}

void LOS_CCP_3D :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	LOS_CCP_3D_Packet *netPacket = dynamic_cast <LOS_CCP_3D_Packet*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getLOS_CCP_3D_PacketKind()) 
	{

		case LOS_CCP_3D_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				int source = atoi(netPacket->getSource());
				collectOutput("Total Received Packets","",1);
				bufferAggregate.push_back(*netPacket);	
			}
			else if (dst.compare(SINK_NETWORK_ADDRESS) == 0 && isSink) 
			{
				int n = netPacket->getNumOfPackets();
				collectOutput("Number of data packets received at BS","",n);
				toApplicationLayer(decapsulatePacket(netPacket));
			}
			
			break;	
		}
	}
}


void LOS_CCP_3D :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		network.clear();
		clusterHeads.clear();
		clusterMembers.clear();
		
		isCH = false;
		isCM = false;
		isActive = true;
		endFormClus = false;

		myCH = 0;
		myTDMATurn = 0;
		clusterLength = 0;

		par("energy") = initialEnergy - resMgrModule->getSpentEnergy();

		setStateRx();

		if (getTimer(START_SLOT) != 0) 
		{ 
			cancelTimer(START_SLOT);
		}

		if (isSink) 
		{ 
	   		setTimer(RUN_CCP,0.0);	 
		}
		else
		{
			setTimer(RUN_CDDP,5.0);
			setTimer(ENTER_STEADY_PHASE,7.0);
        	}

	       	roundNumber++;
		if (roundNumber >= numberOfRounds) break;
		
		setTimer(START_ROUND, roundLength);
		break;
	}

	case RUN_CCP:
	{
		updateSensorInfo();

		Problem   * problem   ; 
  		Algorithm * algorithm ; 
  		Operator  * crossover ; 
  		Operator  * mutation  ; 
  		Operator  * selection ; 

  		problem = new CCP_LOS_3D(coverageMatrix, overlappingMatrix, adjacencyMatrix, Sensors);
 		algorithm = new NSGAII(problem);


		int populationSize = 100;
  		int maxEvaluations = 40000;
		algorithm->setInputParameter("populationSize",&populationSize);
  		algorithm->setInputParameter("maxEvaluations",&maxEvaluations);  
		
		map<string, void *> parameters;

		parameters.clear();
		double mutationProbability = 1.0/problem->getNumberOfBits();
		parameters["probability"] = &mutationProbability;
		mutation = new BitFlipMutation(parameters);

		parameters.clear();
		double crossoverProbability = 0.9;
		parameters["probability"] = &crossoverProbability;
		crossover = new SinglePointCrossover(parameters);

		parameters.clear();
		selection = new BinaryTournament2(parameters) ;

		algorithm->addOperator("crossover",crossover);
		algorithm->addOperator("mutation",mutation);
		algorithm->addOperator("selection",selection);

		SolutionSet * population = algorithm->execute();

		Solution * bestSolution = findBestCompromiseSolution(population);
		Variable ** decisionVariables  = bestSolution->getDecisionVariables();

		double numberRounds = numberOfRounds;
		double nOfActiveSensors=0;

		stringstream out;
		vector<int> sleepNodes;
		vector<int> activeNodes;

		Binary * variable ;
	     	for (int i = 1; i < networkSize ; i++)
	     	{
     		 	variable = (Binary *)(bestSolution->getDecisionVariables()[i]) ;

                	 if (variable->cardinality() == 0){ out <<  0 << ";"; sleepNodes.push_back(i);}
		 	 if (variable->cardinality() == 2){ out <<  2 << ";"; clusterHeads.push_back(i);}
			 if (variable->cardinality() == 1){ out <<  1 << ";"; activeNodes.push_back(i); nOfActiveSensors++; collectOutput("Average number of active nodes per round","",1/numberRounds); }
		}
		
	    	testCoveragePerformance (sleepNodes);

		double Packets = nOfActiveSensors * roundLength * 1;
		collectOutput("Total Generated Packets","",Packets);

		double numberOfCHs = clusterHeads.size();
		collectOutput("Average number of CHs per round","",numberOfCHs/numberRounds);

		string networkStatus = returnConfiguration(activeNodes);		
		par("networkInformation") = networkStatus;

 		delete selection;
 		delete mutation;
  		delete crossover;
  		delete population;
  		delete algorithm;
		break;
	}

	case RUN_CDDP:
	{
		runCDDP();
		
                break;
	}

	case ENTER_STEADY_PHASE:
	{
		if ((!isCH && !isCM) || (!isActive))
		{
 			setStateSleep(); // I am not clustered or Redundunt
			trace() << "I am not clustered or redundunt \n";
		}
	
		if (isCH)
		{
			if (clusterMembers.size()!=0)
			{
				clusterLength = clusterMembers.size();
				setTimer(START_SLOT, clusterLength * slotLength);
			}
			else
			{
				setTimer(START_SLOT, slotLength);	
			}
		}
		else if (isCM)
		{
			setStateSleep();
			setTimer(START_SLOT, myTDMATurn * slotLength);
		}
		break;
	}
    	
	case START_SLOT:
	{
		if ((!isCH && !isCM) || (!isActive))
		{
 			setStateSleep(); // I am not clustered or Redundunt
			trace() << "I am not clustered \n";
		}
		if(isCH && clusterMembers.size()==0) setTimer(START_SLOT, slotLength);	
		else setTimer(START_SLOT, clusterLength * slotLength);
				
		if (isCH) 
		{
			sendAggregate(); 
			processBufferedPacket();
		}

		else 
		{
			setPowerLevel(0);
			processBufferedPacket();
			setTimer(END_SLOT, slotLength);
		}
		break;
	}

	case END_SLOT:
	{
		if (!isSink && !isCH) setStateSleep();
		break;
	}

	case TEST:
	{
		if (!isSink && !isCH) setStateSleep();
		break;
	}

	
   }
}

Solution * LOS_CCP_3D :: findBestCompromiseSolution(SolutionSet *set)
{

	vector<double> minObjectives(5,1000);
	vector<double> maxObjectives(5,-1000);
	
	minObjectives[0] = 0;
	minObjectives[0] = networkSize;

	minObjectives[1] = 0;
	maxObjectives[1] = networkSize;

	minObjectives[2] = 0.50;
	maxObjectives[2] = 0.98;
	
	minObjectives[3] = 0;
	maxObjectives[3] = networkSize;

	minObjectives[4] = 0;
	maxObjectives[4] = networkSize;

	double totalMembership;
	double totalAchievement;
	vector<double> achievement(set->size());

	for (int i = 0 ; i < set->size() ; i++)
	{
		totalMembership = 0;
		Solution * solution = set->get(i);

		for (int j = 0 ; j < solution->getNumberOfObjectives(); j++)
		{

			//trace() << "Objective[" <<j<<"] = " << solution->getObjective(j) << "\n";
			double membership;

			double objective =  solution->getObjective(j);
		
			if (objective <= minObjectives[j] ) membership = 1;
			else if (objective >= maxObjectives[j] ) membership = 0;
			else membership = (maxObjectives[j] - objective) / (maxObjectives[j] - minObjectives[j]);

			totalMembership+= membership;
						
		}

		achievement[i] = totalMembership;
		totalAchievement += totalMembership;
	}
	

	int index;
	double maxA = -1000;

	for (int i = 0 ; i < set->size() ; i++)
	{
		achievement[i] = achievement[i] / totalAchievement;

		if (achievement[i] > maxA)
		{
			maxA = achievement[i];
			index = i;
		}
	
	}

	Solution *best = set->get(index);
	return best;
}

void LOS_CCP_3D :: testCoveragePerformance(vector<int> sleepNodes)
{
	vector<vector<double> > tmpCoverageMatrix(numOfCubes, vector<double>(networkSize,0));
   
	double total = 0;
	double minNodes = 1000000;
	double maxNodes = -1000000;
	double numberRounds = numberOfRounds;

	for (int i = 0 ; i < numOfCubes; i++)
	{
                int counter = 0;
		bool isCovered = false;
					
                       	for (int j = 1 ; j < networkSize ; j++)
			{
				tmpCoverageMatrix[i][j] = coverageMatrix[i][j];
				if(std::find(sleepNodes.begin(),sleepNodes.end(),j)!=sleepNodes.end()) tmpCoverageMatrix[i][j] = 0;
				if (tmpCoverageMatrix[i][j] == 1) 
				{
					counter++;
					isCovered = true;
				}
			}
			
			if (isCovered) collectOutput("Coverage Ratio","",1/numOfCubes);
			if (counter < minNodes) minNodes = counter;
			if (counter > maxNodes) maxNodes = counter;
			total+= counter; 
	}

	double avgRedunduncy = total/numOfCubes;
	collectOutput("Average Number of Nodes covering a cube","",avgRedunduncy/numberRounds);
	collectOutput("Minimum Number of Nodes covering a cube","",minNodes/numberRounds);
	collectOutput("Maximum Number of Nodes covering a cube","",maxNodes/numberRounds);
}


void LOS_CCP_3D :: updateSensorInfo()
{
	for (int i = 0 ; i < networkSize ; i++)
	{
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;
		
              	n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
				
		double energy = c->par("energy").doubleValue(); 
		Sensors[i].energy = energy;
	}
}

void LOS_CCP_3D :: runCDDP()
{
		double nonClustered = 0;
		isActive = false;

	  	cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	
		string info = tmpModule->par("networkInformation").stringValue();
	
		char_separator<char> sep(";");
    		tokenizer<char_separator<char>> tokens(info, sep);
    		for (const auto& t : tokens) 
		{
			char_separator<char> sep2(",");
			tokenizer<char_separator<char>> data(t, sep2);
			int i = 0;
			int ch;
			for (const auto& d : data) 
			{
				if (i==0)
				{
					ch = stoi (d);
					if (self == ch)
					{
						isCH = true;
						isActive = true;
					
					}
					i++;
				}
				else
				{
					char_separator<char> sep3(" ");
					tokenizer<char_separator<char>> mmbr(d, sep3);
					int j = 0;
					for (const auto& m : mmbr) 
					{
						if (self == ch)
						{
							clusterMembers.push_back(stoi(m));
							trace() << "CH, will add " <<stoi(m) <<" to members \n";
						}
						if ( (self == stoi(m)) && (isActive))
						{
						
							myCH = ch;
							isCM = true;
							isActive = true;
							myTDMATurn = j;
							trace() << " CM, my TDMA is " <<myTDMATurn <<" \n";
						}
						j++;
						if (myCH == ch ) clusterLength = j;
				}
				
			}

		}
	}

	endFormClus = true;
}

string LOS_CCP_3D :: returnConfiguration(vector<int> activeNodes)
{
		multimap<int,int> network;

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
			}
		}


		stringstream out;
		string configuration;
		for (int i = 0 ; i < clusterHeads.size() ; i++)
		{
		    out << clusterHeads[i] << ",";

		    for (multimap<int,int>::iterator iter = network.begin(); iter != network.end(); iter++)
       		    {
			if(iter->second == clusterHeads[i] )
			{
				out << iter->first << " ";
			}
		    }
		    out << ";";
		}

		configuration = out.str();
		//myfile << "configuration" << configuration <<"\n";
		return configuration;
	}

void LOS_CCP_3D :: sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		LOS_CCP_3D_Packet *aggrPacket = new LOS_CCP_3D_Packet("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setLOS_CCP_3D_PacketKind(LOS_CCP_3D_DATA_PACKET);
		aggrPacket->setSource(SELF_NETWORK_ADDRESS);
		aggrPacket->setDestination(SINK_NETWORK_ADDRESS);
		aggrPacket->setNumOfPackets(bufferAggregate.size());

		ApplicationPacket *newPacket = new ApplicationPacket("App gen pkt", APPLICATION_PACKET);
		newPacket->setData(0);
		newPacket->getAppNetInfoExchange().destination = string("0");
		newPacket->getAppNetInfoExchange().source = selfAddress;
		newPacket->getAppNetInfoExchange().timestamp = simTime();
		newPacket->setApplicationID(applicationID.c_str());
		encapsulatePacket(aggrPacket, newPacket);
		//trace() << "CH, will buffer aggregated data \n";

		bufferPacket(aggrPacket);
		bufferAggregate.clear();
	}
}

void LOS_CCP_3D :: processBufferedPacket()
{
	string dst;
	if (isCH)
	{
		ostringstream convert;   
  		convert << 0;      
  		dst = convert.str();
	}
	else if (isCM)
	{
		ostringstream convert;   
  		convert << myCH;      
  		dst = convert.str();
	}
	while (!tempTXBuffer.empty())
	{
		cPacket *pkt = tempTXBuffer.front();	
		LOS_CCP_3D_Packet *netPacket = dynamic_cast <LOS_CCP_3D_Packet*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
	
}

void LOS_CCP_3D :: clusterTheNetwork()
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
					if ( (rssi > maxRSSI) && (rssi >= -87) && (i != clusterHeads[j]) )
       					{
							maxRSSI = rssi;
       							cHead = clusterHeads[j];
					}
				}
			}

                        if (maxRSSI != -10000)
			{
			  network.insert(pair<int,int>(i,cHead));
			  numberOfClusteredNodes = numberOfClusteredNodes + 1;
			}
		}
}

void LOS_CCP_3D :: setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void LOS_CCP_3D :: setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void LOS_CCP_3D :: setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void LOS_CCP_3D :: levelTxPower(int linkBudget)
{
	vector<int>::iterator constIterator;
	for (constIterator = powers.begin();
		constIterator != powers.end();
	        constIterator++ ) {
		if(*constIterator > (linkBudget))
		{
			setPowerLevel(*constIterator);
			break;
		}
	}
}

void LOS_CCP_3D :: readXMLparams()
{
	cXMLElement *rootelement = par("powersConfig").xmlValue();
	if (!rootelement) endSimulation();
	cXMLElement* data = rootelement->getFirstChildWithTag("maxPower");
	maxPower = atoi(data->getNodeValue());
	data = rootelement->getFirstChildWithTag("sensibility");
	sensibility = atoi(data->getNodeValue());
	data = rootelement->getFirstChildWithTag("aggrConsumption");
	aggrConsumption = atoi(data->getNodeValue());
	cXMLElementList sources = rootelement->getChildrenByTagName("power");
	for (int s = 0; s < sources.size(); s++)
		powers.push_back(atoi(sources[s]->getNodeValue()));
	if (powers.size() < 1) endSimulation();
	sort(powers.begin(), powers.end());
}




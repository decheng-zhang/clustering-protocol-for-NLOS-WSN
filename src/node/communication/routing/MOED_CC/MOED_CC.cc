#include "MOED_CC.h"

Define_Module(MOED_CC);

void MOED_CC::startup()
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

void MOED_CC :: updateCoverageMatrix()
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

void MOED_CC :: initializeMatrices()
{
	for (int i = 0 ; i < networkSize ; i++)
	{
		vector<double> r;

		for (int j = 0 ; j < networkSize ; j++)
		{
			r.push_back(0);
		}

		adjacencyMatrix.push_back(r);
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

void MOED_CC :: updateSensors()
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

void MOED_CC :: updateMatrices()
{
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
			adjacencyMatrix[i][j] = dist;
		}
	}

}

void MOED_CC :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		MOED_CC_Packet *netPacket = new MOED_CC_Packet("MOED_CC routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setMOED_CC_PacketKind(MOED_CC_DATA_PACKET);
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

void MOED_CC :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	MOED_CC_Packet *netPacket = dynamic_cast <MOED_CC_Packet*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getMOED_CC_PacketKind()) 
	{

		case MOED_CC_DATA_PACKET:
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


void MOED_CC :: timerFiredCallback(int index)
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

		Problem   * problem;   
  		Algorithm * algorithm; 
  		Operator  * crossover;
  		Operator  * mutation;  

  		problem = new MOED_CCP(coverageMatrix, adjacencyMatrix, Sensors);
 		algorithm = new MOEAD(problem);

		map<string, void *> parameters;

		int populationSize = 300;
  		int maxEvaluations = 10000;
  		algorithm->setInputParameter("populationSize",&populationSize);
  		algorithm->setInputParameter("maxEvaluations",&maxEvaluations);  
		
		string dataDirectoryValue = "";
  		algorithm->setInputParameter("dataDirectory", &dataDirectoryValue);

  		double crParameter = 1.0;
  		double fParameter = 0.5;
  		parameters["CR"] = &crParameter;
  		parameters["F"] = &fParameter;
  		crossover = new DifferentialEvolutionCrossover(parameters);
  
  		parameters.clear();
  		double probabilityParameter = 1.0/(problem->getNumberOfVariables());
  		double distributionIndexParameter = 20.0;
  		parameters["probability"] =  &probabilityParameter;
  		parameters["distributionIndex"] = &distributionIndexParameter;
  		mutation = new PolynomialMutation(parameters);

  		algorithm->addOperator("crossover",crossover);
  		algorithm->addOperator("mutation",mutation);

		SolutionSet * population = algorithm->execute();

		Solution * bestSolution = findBestCompromiseSolution(population);
		Variable ** decisionVariables  = bestSolution->getDecisionVariables();

		double numberRounds = numberOfRounds;
		double nOfActiveSensors=0;

		stringstream out;
		vector<int> sleepNodes;
		vector<int> activeNodes;


	    	for (int i = 1; i < networkSize ; i++)
	     	{
     		 	double status =  ceil ( decisionVariables[i]->getValue() * 3 ) ;
                 	
		 	if (status == 0){ out <<  0 << ";"; sleepNodes.push_back(i);}
			if (status == 2){ out <<  2 << ";"; clusterHeads.push_back(i);}
			if (status == 1){ out <<  1 << ";"; activeNodes.push_back(i); nOfActiveSensors++; collectOutput("Average number of active nodes per round","",1/numberRounds); }
	    	}

		testCoveragePerformance (sleepNodes);

		double Packets = nOfActiveSensors * roundLength * 1;
		collectOutput("Total Generated Packets","",Packets);

		double numberOfCHs = clusterHeads.size();
		collectOutput("Average number of CHs per round","",numberOfCHs/numberRounds);
		
		string networkStatus = returnConfiguration(activeNodes);		
		par("networkInformation") = networkStatus;


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

Solution * MOED_CC :: findBestCompromiseSolution(SolutionSet *set)
{

	vector<double> minObjectives(2,1000);
	vector<double> maxObjectives(2,-1000);
	
	minObjectives[0] = 1;
	maxObjectives[0] = networkSize;

	minObjectives[1] = 0;
	maxObjectives[1] = numOfCubes;

	
	double totalMembership;
	double totalAchievement;
	vector<double> achievement(set->size());

	for (int i = 0 ; i < set->size() ; i++)
	{
		totalMembership = 0;
		Solution * solution = set->get(i);

		for (int j = 0 ; j < solution->getNumberOfObjectives(); j++)
		{

			trace() << "Objective[" <<j<<"] = " << solution->getObjective(j) << "\n";
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

void MOED_CC :: testCoveragePerformance(vector<int> sleepNodes)
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


void MOED_CC :: updateSensorInfo()
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



void MOED_CC :: sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		MOED_CC_Packet *aggrPacket = new MOED_CC_Packet("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setMOED_CC_PacketKind(MOED_CC_DATA_PACKET);
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

void MOED_CC :: processBufferedPacket()
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
		MOED_CC_Packet *netPacket = dynamic_cast <MOED_CC_Packet*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
	
}
void MOED_CC :: runCDDP()
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

string MOED_CC :: returnConfiguration(vector<int> activeNodes)
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
					if ( (rssi > maxRSSI) && (rssi >= -87) && (node2 != node1 ))
       					{
							maxRSSI = rssi;
       							cHead = clusterHeads[j];
					}
				}
			}

                        if (maxRSSI != -10000)
			{
			  network.insert(pair<int,int>(i,cHead));
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


void MOED_CC :: setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void MOED_CC :: setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void MOED_CC :: setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void MOED_CC :: levelTxPower(int linkBudget)
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

void MOED_CC :: readXMLparams()
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




#include "EA_CH_3D.h"

Define_Module(EA_CH_3D);

void EA_CH_3D::startup()
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

	
	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 1; 

	Sensors.clear();

	declareOutput("PDR");
	declareOutput("Average number of CHs per round");
        declareOutput("Average number of unclustered nodes per round");
	
	
     	if (isSink)
	{
		initializeMatrices();	
		updateSensors();
		updateMatrices();
	}

	readXMLparams();
	setTimer(START_ROUND,0.0);
}

void EA_CH_3D :: initializeMatrices()
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

}

void EA_CH_3D :: updateSensors()
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

void EA_CH_3D :: updateMatrices()
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
		}
	}

}



void EA_CH_3D :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		EA_CH_3D_Packet *netPacket = new EA_CH_3D_Packet("EA_CH_3D routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setEA_CH_3D_PacketKind(EA_CH_3D_DATA_PACKET);
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

void EA_CH_3D :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	EA_CH_3D_Packet *netPacket = dynamic_cast <EA_CH_3D_Packet*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getEA_CH_3D_PacketKind()) 
	{

		case EA_CH_3D_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				int source = atoi(netPacket->getSource());
				//actualMembers.push_back(source);
				trace() << "CH, Aggregate Data Frame from "<< source << "\n";
				double p = 1 / totalPackets;
				collectOutput("PDR","",p);
				bufferAggregate.push_back(*netPacket);	
			}
			else if (dst.compare(SINK_NETWORK_ADDRESS) == 0 && isSink) 
			{
		//trace() << "SINK, Processing Data Packet from "<< atoi(netPacket->getSource()) <<"\n";
				int n = netPacket->getNumOfPackets();
				trace() << "n= "<< n << "\n";
				collectOutput("Number of data packets received at BS","",n);
				toApplicationLayer(decapsulatePacket(netPacket));
			}
			break;	
		}
	}
}


void EA_CH_3D :: timerFiredCallback(int index)
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
			updateSensorInfo();
	   		setTimer(FIND_CHS,0.0);	 
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

	case FIND_CHS:
	{
		Problem   * problem   ; // The problem to solve
  		Algorithm * algorithm ; // The algorithm to use
  		Operator  * crossover ; // Crossover operator
  		Operator  * mutation  ; // Mutation operator
  		Operator  * selection ; // Selection operator

  		problem = new CH_3D(adjacencyMatrix, Sensors);

		algorithm = new NSGAII(problem);


		int populationSize = 200;
  		int maxEvaluations = 100000;
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

		stringstream out;
		Binary * variable;
		for (int i = 1 ; i < networkSize ; ++i)
  		{ 
 		   	variable = (Binary *)(bestSolution->getDecisionVariables()[i]) ;
                   	if (variable->getIth(0)) { out <<  i << ";"; clusterHeads.push_back(i);}
		}	

		double numberOfCHs = clusterHeads.size();
		double numberOfUnclusteredNodes = bestSolution->getObjective(2); 
		double numberRounds = numberOfRounds;

		collectOutput("Average number of CHs per round","",numberOfCHs/numberRounds);
		collectOutput("Average number of unclustered nodes per round","",numberOfUnclusteredNodes/numberRounds);

		
		string configuration = returnConfiguration();
		trace()<<"configuration "<< configuration <<"\n";
		par("networkInformation") = configuration;	

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
		if (!isCH && !isCM ) setStateSleep(); // I am not clustered
	
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
		if (!isCH && !isCM) setStateSleep(); // I am not clustered
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

	}
}

string EA_CH_3D :: returnConfiguration()
{
		multimap<int,int> network;

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


Solution * EA_CH_3D :: findBestCompromiseSolution(SolutionSet *set)
{

	vector<double> minObjectives(3,1000);
	vector<double> maxObjectives(3,-1000);

	minObjectives[0] = 0;
	maxObjectives[0] = networkSize;

	minObjectives[1] = 0.50;
	maxObjectives[1] = 1.00;

	minObjectives[2] = 0;
	maxObjectives[2] = networkSize;

	
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

void EA_CH_3D :: testCoveragePerformance(vector<int> sleepNodes)
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
			
			if (isCovered) collectOutput("Coverage Ratio","",1/numberRounds);
			if (counter < minNodes) minNodes = counter;
			if (counter > maxNodes) maxNodes = counter;
			total+= counter; 
	}

	double avgRedunduncy = total/numOfCubes;
	collectOutput("Average number nodes covering a cube per round","",avgRedunduncy/numberRounds);
	collectOutput("Minimum Number of Nodes covering a cube","",minNodes/numberRounds);
	collectOutput("Maximum Number of Nodes covering a cube","",maxNodes/numberRounds);
}


void EA_CH_3D :: updateSensorInfo()
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

void EA_CH_3D :: runCDDP()
{
	cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	
 	string info = tmpModule->par("networkInformation").stringValue();
	//trace()<<"NW Info "<< info <<"\n";
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
				}
				i++;
			}
			else
			{
				//trace() << "NBRS are " << d;
				char_separator<char> sep3(" ");
				tokenizer<char_separator<char>> mmbr(d, sep3);
				int j = 0;
				for (const auto& m : mmbr) 
				{
					if (self == ch)
					{
						clusterMembers.push_back(stoi(m));
					//trace() << "CH, will add " <<stoi(m) <<" to members \n";
					}
					if ( self == stoi(m) )
					{
						
							myCH = ch;
							isCM = true;
							myTDMATurn = j;
						//trace() << " CM, my TDMA is " <<myTDMATurn <<" \n";
						
						
					}
					j++;
					if (myCH == ch ) clusterLength = j;
				}
				
			}

		}
	}
	endFormClus = true;
}


void EA_CH_3D :: sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		EA_CH_3D_Packet *aggrPacket = new EA_CH_3D_Packet("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setEA_CH_3D_PacketKind(EA_CH_3D_DATA_PACKET);
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

void EA_CH_3D :: processBufferedPacket()
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
		EA_CH_3D_Packet *netPacket = dynamic_cast <EA_CH_3D_Packet*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
	
}


void EA_CH_3D :: setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void EA_CH_3D :: setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void EA_CH_3D :: setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void EA_CH_3D :: levelTxPower(int linkBudget)
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

void EA_CH_3D :: readXMLparams()
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




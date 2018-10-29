#include "MOPSO_C.h"

Define_Module(MOPSO_C);

void MOPSO_C::startup()
{	
	/*--- The .ned file's parameters ---*/
	percentage = par("percentage");
	roundLength = par("roundLength");
	isSink = par("isSink");
	slotLength = par("slotLength");
	advPacketSize = par("advPacketSize");
	joinPacketSize = par("joinPacketSize");
	tdmaPacketSize = par("tdmaPacketSize");
	dataPacketSize = par("dataPacketSize");
	applicationID = par("applicationID").stringValue(); 
	numberOfRounds = par("numberOfRounds");
	swarmSize = par("swarmSize");
	/*numOfCHs = par("numOfCHs");
	//epoch = par("epoch");
	//mode = par("mode");*/
	
	

	/*--- Class parameters ---*/
	
	roundNumber=0;

	candidates = "";
	routes = "";
        neighbours = "";
	numOfNbrs = 0; 
	CH_Frequencey = 0;
	numOfSel = 0;
	networkInformation = " ";	

	myTDMATurn = 0;
	clusterLength = 0;

	noLiveNodes = 0;	

	neighborTable.clear();
	clusterMembers.clear();
	eligibleNodes.clear();
		
	isCH = false;
	isCM = false;
	isRelayNode = false;
	endFormClus = false;

	theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");

	for (int i = 0 ; i < networkSize ; i++)
	{
		vector<double> r;
		for (int j = 0 ; j < networkSize ; j++)
		{
			r.push_back(0);
		}
		adjacencyMatrix.push_back(r);
	}

	networkInfo.reserve(networkSize);

	declareOutput("Number of unclustered nodes");
      	declareOutput("Average number of unclustered nodes");
	declareOutput("PDR");
	declareOutput("Number of data packets received at BS");
	declareOutput("Execution Time");
        declareOutput("Average Number of Active Nodes per Round");
	//declareOutput("Weighted Encoding Best Fitness");
	//declareOutput("Periority Encoding Best Fitness");

	readXMLparams();
	setTimer(START_ROUND,0.0);

	
}

void MOPSO_C :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		MOPSORoutingPacket *netPacket = new MOPSORoutingPacket("Data Packet", NETWORK_LAYER_PACKET);
		netPacket->setMOPSORoutingPacketKind(MOPSO_ROUTING_DATA_PACKET);
		netPacket->setByteLength(dataPacketSize);
		netPacket->setSource(SELF_NETWORK_ADDRESS);
		encapsulatePacket(netPacket, pkt);
		// If cluster formation ended and you r not CH, then set dest. to CH and buffer packet
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

void MOPSO_C :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{

	MOPSORoutingPacket *netPacket = dynamic_cast <MOPSORoutingPacket*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getMOPSORoutingPacketKind()) 
	{

		case MOPSO_ROUTING_FIND_NEIGHBOURS_PACKET:
     		{
			nodeData info = netPacket->getInfo();
                        int n_id = info.id;
			double n_rssi = rssi;
			double n_remainingEnergy = info.energy;
			updateNeighborTable(n_id,n_rssi,n_remainingEnergy);

			break;	
     		}

		case MOPSO_ROUTING_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				int source = atoi(netPacket->getSource());
				//actualMembers.push_back(source);
				trace() << "CH, Aggregate Data Frame from "<< source << "\n";
				collectOutput("Number of packets received per cluster head","",1);
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


void MOPSO_C :: timerFiredCallback(int index)
{
   
	switch (index) 
    {

	case START_ROUND:
	{
		par("neighbours") = "";
		par("numOfNbrs") = 0;
		par("energy") = 18720 - resMgrModule->getSpentEnergy();
		
		trace() << "PSO CLUSTERING - START ROUND NO. " << roundNumber;
		setStateRx();
		//setPowerLevel(maxPower);

		if (getTimer(START_SLOT) != 0) 
		{ 
			cancelTimer(START_SLOT);
		}
			
		candidates = "";
		heads.clear();
		neighborTable.clear();
		networkInfo.clear();
		clusterHeads.clear();
		clusterMembers.clear();
		eligibleNodes.clear();
		
		isCH = false;
		isCM = false;
		endFormClus = false;
		isRelayNode = false;

		nextHop = 0;
		myTDMATurn = 0;
		clusterLength = 0;

		noLiveNodes = 0;
	
		for (int i = 0 ; i < networkSize ; i++)
		{
			for (int j = 0 ; j < networkSize ; j++)
			{
				adjacencyMatrix[i][j] = 0;
			}
		}
		
		double timer = uniform (0.0,5.0);
     
		setTimer(FIND_NEIGHBORS,timer);

		if (isSink) 
		{ 	   		
			setTimer(RUN_PSO,6.0);	
		}

		else
		{
			setTimer(RUN_CDDP,7.0);
			//setTimer(ENTER_STEADY_PHASE,8.0);
        	}
				
		//setTimer (TEST, 10.0);

        	roundNumber++;
		if (roundNumber >= numberOfRounds) break;
		
		setTimer(START_ROUND, roundLength);

		break;
	}

	case FIND_NEIGHBORS:
     	{  
		nodeData info;
		info.id = self;
		info.energy = par("energy").doubleValue();
		MOPSORoutingPacket *crtlPkt = new MOPSORoutingPacket("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setMOPSORoutingPacketKind(MOPSO_ROUTING_FIND_NEIGHBOURS_PACKET);
		crtlPkt->setInfo(info);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case RUN_PSO:
	{
		trace() << "In run MOPSO \n";
		clock_t t_ini, t_fin;

  		Problem   * problem;   // The problem to solve
  		Algorithm * algorithm; // The algorithm to use
  		Operator  * mutation;  // "Turbulence" operator

  		map<string, void *> parameters; // Operator parameters

  		//TODO: QualityIndicator * indicators; // Object to get quality indicators

   		problem = new Clustering("Real",numOfCHs,networkInfo,eligibleNodes,adjacencyMatrix);

  		algorithm = new SMPSO(problem);

  		// Algorithm parameters
  		int swarmSizeValue = 100;
  		int archiveSizeValue = 100;
  		int maxIterationsValue = 20;
  		algorithm->setInputParameter("swarmSize",&swarmSizeValue);
  		algorithm->setInputParameter("archiveSize",&archiveSizeValue);
  		algorithm->setInputParameter("maxIterations",&maxIterationsValue);

  		// Mutation operator
  		double probabilityParameter = 1.0/(problem->getNumberOfVariables());
  		double distributionIndexParameter = 20.0;
  		parameters["probability"] =  &probabilityParameter;
  		parameters["distributionIndex"] = &distributionIndexParameter;
  		mutation = new PolynomialMutation(parameters);

  		// Add the operators to the algorithm
  		algorithm->addOperator("mutation",mutation);

  		// Add the indicator object to the algorithm
  		//algorithm->setInputParameter("indicators", indicators) ;

  		// Execute the Algorithm
  		t_ini = clock();
  		SolutionSet * population = algorithm->execute();
  		t_fin = clock();
  		double secs = (double) (t_fin - t_ini);
  		secs = secs / CLOCKS_PER_SEC;
		     collectOutput("Execution Time","",secs);
  		// Result messages
  		trace() << "Total execution time: " << secs << "s" << "\n";
  		trace() << "Variables values have been written to file VAR" << "\n";
  		population->printVariablesToFile("VAR");
  		trace() << "Objectives values have been written to file FUN" << "\n";
  		population->printObjectivesToFile("FUN");

  		delete mutation;
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
//trace()<< "PSO-C- STEADY_PHASE I will start to send data at " << SIMTIME_DBL(simTime()) << "\n";
		if (isCH)
		{
			if (clusterMembers.size()!=0)
			{
				clusterLength = clusterMembers.size();
				setTimer(START_SLOT, clusterLength * slotLength);
				trace()<< "CH , member size > 0 , clusterLength ="<<clusterLength<<"\n";
			}
			else
			{
				setTimer(START_SLOT, slotLength);	
			}
		}
		else
		{
			setStateSleep();
			setTimer(START_SLOT, myTDMATurn * slotLength);
		}
		break;
	}
    

	case START_SLOT:
	{
		//trace() << "In Start slot \n";
		if(isCH && clusterMembers.size()==0) setTimer(START_SLOT, slotLength);	
		else setTimer(START_SLOT, clusterLength*slotLength);

		if (isCH) 
		{
			sendAggregate(); 
			processBufferedPacket();
		}

		if (!isCH)
		{
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
		break;
	}
	
    }
  
	
}


void MOPSO_C :: initProtocol()
{
	double totalEnergy;
	
	//eligibleNodes.reserve(networkSize);
	networkInfo.clear();
	eligibleNodes.clear();

	noLiveNodes = 0;
	
	for (int i = 0 ; i < networkSize ; i++)
	{
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;
		
                trace() << "Node " << i << " \n";
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));

		
		NodeControlInfo info;
		info.id = i;
		info.energy = c->par("energy").doubleValue(); 
		info.rssi = 0;
		trace() << "Energy = " << info.energy <<"\n";
		info.numOfNbrs = c->par("numOfNbrs"); 
		trace() << "Num of nbrs = " << info.numOfNbrs <<"\n";
		int numOfNbrs = c->par("numOfNbrs"); 
                //trace()<< "INIT- numOfNbrs " << numOfNbrs <<"\n";
		//if (numOfNbrs > 0) info.nbrs.reserve(numOfNbrs);	
		const char* nbrs = c->par("neighbours").stringValue();
		trace()<< "neighbors are: " << nbrs << "\n";
                char* neighbor = const_cast <char*> (nbrs);
		char* rest;
		char* token;
		char* nbr;
		char* ptr = neighbor;
		info.nbrs.reserve(numOfNbrs);
		info.nbrs.clear();
	
		while (token = strtok_r(ptr,";",&rest))// Find each nbr
		{
			nbr = token;
			ptr = rest;
			
			char* tok = strtok(nbr,",");
			NeighborRecord rec;
                        for (int j = 0 ; j < 3 ; j++)
			{
	  			if (j==0)rec.id = atoi (tok);
				else if (j==1) 	rec.rssi = atof (tok);
				else if (j==2)  rec.remainingEnergy = atof (tok);
				tok = strtok(NULL,",");
			}
			
			info.nbrs.push_back(rec);
			adjacencyMatrix[info.id][rec.id] = rec.rssi;
			//myfile <<"(n"<<i<<") edge node {"<<rec.rssi<<"} (n"<<rec.id<<")\n";
		}

		totalEnergy = totalEnergy + info.energy;

	      	if (info.energy > 5) noLiveNodes++;

               	networkInfo.push_back(info);
	}

	double avgEnergy = totalEnergy / networkSize;

	for (int j = 1 ; j < networkSize ; j++)
	{
		if ( networkInfo[j].energy >= avgEnergy)
		{
			eligibleNodes.push_back(networkInfo[j]);

		}
	}

	trace() << "Eligable Nodes size = "<< eligibleNodes.size();
	
}

void MOPSO_C :: runCDDP()
{

	double nonClustered = 0;

  	cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	
 	string info = tmpModule->par("networkInformation").stringValue();
	trace()<<"In runCDDP "<< info <<"\n";
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

	if (isCH)
	{
		trace() << "Member \n";

		for (int i = 0 ; i < clusterMembers.size() ; i++)
		{
			int member = clusterMembers[i];
			trace () << member ;

			if (find( neighbors.begin() , neighbors.end() , member) == neighbors.end())
			{
				nonClustered++;
				//trace() << " not found \n";

			}

		}
	
		trace() << nonClustered <<" nodes are not clustered \n"; 

		double numberRounds = numberOfRounds;
		double av = nonClustered/numberRounds;

		collectOutput("Number of unclustered nodes","",nonClustered);
        	collectOutput("Average number of unclustered nodes","",av);  
                collectOutput("Average Number of Active Nodes per Round","",1/numberRounds);
	}
	
	//trace() << "My Cluster Length = " << clusterLength <<"\n";
}

void MOPSO_C::updateNeighborTable(int nodeID , double rssi , double energy)
{
		NeighborRecord newRec;
		newRec.id = nodeID;
		newRec.rssi = rssi;
		newRec.remainingEnergy = energy;
		neighborTable.push_back(newRec);
		neighbors.push_back(nodeID);
}

void MOPSO_C::sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		MOPSORoutingPacket *aggrPacket = new MOPSORoutingPacket("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setMOPSORoutingPacketKind(MOPSO_ROUTING_DATA_PACKET);
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
		trace() << "CH, will buffer aggregated data \n";

		bufferPacket(aggrPacket);
		bufferAggregate.clear();
	}
}

void MOPSO_C::processBufferedPacket()
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
		MOPSORoutingPacket *netPacket = dynamic_cast <MOPSORoutingPacket*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
}

void MOPSO_C :: finishSpecific()
{
	
}


void MOPSO_C :: setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}


void MOPSO_C :: setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void MOPSO_C :: setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void MOPSO_C :: levelTxPower(int linkBudget)
{
	
}

void MOPSO_C::readXMLparams()
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



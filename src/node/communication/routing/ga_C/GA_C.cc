#include "GA_C.h"


Define_Module(GA_C);

void GA_C::startup()
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
	populationSize = par("populationSize");
	numOfCHs = par("numOfCHs");
	epoch = par("epoch");

	/*--- Class parameters ---*/
	roundNumber=0;
	networkInformation = " ";

	clusterMembers.clear();
	networkInfo.clear();
	eligibleNodes.clear(); 
	
	isCH = false;
	isCM = false;
	endFormClus = false;

	myCH = 0;
	myTDMATurn = 0;
	clusterLength = 0;

	noLiveNodes = 0;
	
	//srand (time(NULL));

	theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");
	
	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 1; 
	declareOutput("PDR");

	declareOutput("Convergence");
	declareOutput("Number of unclustered nodes");
      	declareOutput("Average number of unclustered nodes per round");
	declareOutput("Number of data packets received at BS");
	
	readXMLparams();
	setTimer(START_ROUND,0.0);
}

void GA_C :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		GARoutingPacket *netPacket = new GARoutingPacket("PSO-C routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setGARoutingPacketKind(GA_ROUTING_DATA_PACKET);
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

void GA_C :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	GARoutingPacket *netPacket = dynamic_cast <GARoutingPacket*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getGARoutingPacketKind()) 
	{

		case GA_ROUTING_FIND_NEIGHBOURS_PACKET:
     		{
			nodeData2 info = netPacket->getInfo();
                        int n_id = info.id;
			double n_rssi = rssi;
			double n_remainingEnergy = info.energy;
			updateNeighborTable(n_id,n_rssi,n_remainingEnergy);

			break;	
     		}

		case GA_ROUTING_DATA_PACKET:
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


void GA_C :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		//trace() << "GA_C - START ROUND NO. " << roundNumber << "\n";
		setStateRx();
		//setPowerLevel(maxPower);
		if (getTimer(START_SLOT) != 0) 
		{ 
			cancelTimer(START_SLOT);
		}
			
		networkInformation = " ";
		networkInfo.clear();
		eligibleNodes.clear();
		clusterMembers.clear();
		clusterHeads.clear();
		actualMembers.clear();
		neighborTable.clear();
		neighbors.clear();

		isCH = false;
		isCM = false;
		endFormClus = false;

		myCH = 0;
		myTDMATurn = 0;
		clusterLength = 0;

		noLiveNodes = 0;

		double timer = uniform (0.0,5.0);
     
		setTimer(FIND_NEIGHBORS,timer);

		if (isSink) 
		{ 
	   		initProtocol();	
			setTimer(RUN_GA,6.0);	 
		}

		else
		{
			setTimer(RUN_CDDP,7.0);
			setTimer(ENTER_STEADY_PHASE,8.0);
        	}

		//setTimer (TEST, 99.0);*/

        	roundNumber++;
		if (roundNumber >= numberOfRounds) break;
		
		setTimer(START_ROUND, roundLength);
		break;
	}

	case FIND_NEIGHBORS:
     	{  
		nodeData2 info;
		info.id = self;
		info.energy = par("energy").doubleValue();
		GARoutingPacket *crtlPkt = new GARoutingPacket("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setGARoutingPacketKind(GA_ROUTING_FIND_NEIGHBOURS_PACKET);
		crtlPkt->setInfo(info);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case RUN_GA:
	{
		clock_t t_ini, t_fin;

		  Problem   * problem   ; // The problem to solve
		  Algorithm * algorithm ; // The algorithm to use
		  Operator  * crossover ; // Crossover operator
		  Operator  * mutation  ; // Mutation operator
		  Operator  * selection ; // Selection operator


	   	  double numOfCHs = ( networkSize * percentage ) / 100;
		  GAProblem * p = new GAProblem(numOfCHs,networkInfo,eligibleNodes);
		  problem = p;

		  algorithm = new gGA(p);

		  // Algorithm parameters
		  int populationSizeValue = 50;
		  int maxEvaluationsValue = 500;
		  algorithm->setInputParameter("populationSize",&populationSizeValue);
		  algorithm->setInputParameter("maxEvaluations",&maxEvaluationsValue);

		  // Mutation and Crossover for Real codification
		  map<string, void *> parameters;
		  double crossoverProbability = 0.9;
		  double distributionIndexValue1 = 20.0;
		  parameters["probability"] =  &crossoverProbability ;
		  parameters["distributionIndex"] = &distributionIndexValue1 ;
		  crossover = new SBXCrossover(parameters);

		  //parameters.clear();
		  double mutationProbability = 1.0/problem->getNumberOfVariables();
		  double distributionIndexValue2 = 20.0;
		  parameters["probability"] = &mutationProbability;
		  parameters["distributionIndex"] = &distributionIndexValue2 ;
		  mutation = new PolynomialMutation(parameters);

		  // Selection Operator
		  parameters.clear();
		  selection = new BinaryTournament2(parameters) ;

		 // Add the operators to the algorithm
		 algorithm->addOperator("crossover",crossover);
		 algorithm->addOperator("mutation",mutation);
		 algorithm->addOperator("selection",selection);

	         // Add the indicator object to the algorithm
	        //algorithm->setInputParameter("indicators", indicators) ;

	        // Execute the Algorithm
	        t_ini = clock();
		SolutionSet * population = algorithm->execute();
		t_fin = clock();
		double secs = (double) (t_fin - t_ini);
		secs = secs / CLOCKS_PER_SEC;
		Solution * candidateSolution = population->get(0);
		t_fin = clock();
			
		for (int i = 0 ; i < numOfCHs ; ++i)
  		{ 
      			int index =  candidateSolution->getDecisionVariables()[i]->getValue();
			nodeInfo cand = eligibleNodes[index];
      			clusterHeads.push_back(cand);
		}
		
		vector<nodeInfo> clustered = p->clusterTheNetwork(clusterHeads);
		
 		string c;		
		stringstream out;
  		
		trace() << "GA_C Candidates \n"; 
		for (int i = 0 ; i < clustered.size() ; i++)
		{
			int cand =  clustered[i].id;
			trace() << cand << " ";
			out << cand << ",";
			for (int j = 0 ; j < clustered[i].members.size() ; j++)
			{
				int member = clustered[i].members[j].id;
				//trace() << "member " << member << " \n";
				out << member << " ";
			}
			out << ";";
		}
		trace() << "\n";

		c = out.str();
		par("networkInformation") = c;

		trace() << " Network Info " << c << "\n";
              	delete crossover;
	  	delete mutation;
	  	delete selection;
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

void GA_C :: initProtocol()
{
	double totalEnergy;
	networkInfo.clear();
	eligibleNodes.clear();

	noLiveNodes = 0;
	
	for (int i = 0 ; i < networkSize ; i++)
	{
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;
		
 
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));

		nodeInfo info;
		info.id = i;
		info.x = l->getLocation().x;
		info.y = l->getLocation().y;
		info.energy = c->par("energy").doubleValue(); 
		networkInfo.push_back(info);
		totalEnergy = totalEnergy + info.energy;
		if (info.energy > 5) noLiveNodes++;
	}

	double avgEnergy = totalEnergy / networkSize;

	for (int j = 1 ; j < networkSize ; j++)
	{
		if ( networkInfo[j].energy >= avgEnergy)
		{
			eligibleNodes.push_back(networkInfo[j]);

		}

	}	
}

void GA_C :: runCDDP()
{

	double nonClustered = 0;

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

	if (isCH)
	{
		//trace() << "Member \n";

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
	
		//trace() << nonClustered <<" nodes are not clustered \n"; 

		double numberRounds = numberOfRounds;
		double av = nonClustered/numberRounds;

		collectOutput("Number of unclustered nodes","",nonClustered);
        	collectOutput("Average number of unclustered nodes per round","",av);  
	}
	
	//trace() << "My Cluster Length = " << clusterLength <<"\n";

}


void GA_C::sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		GARoutingPacket *aggrPacket = new GARoutingPacket("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setGARoutingPacketKind(GA_ROUTING_DATA_PACKET);
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

void GA_C::updateNeighborTable(int nodeID , double rssi , double energy)
{
		NeighborRecord newRec;
		newRec.id = nodeID;
		newRec.rssi = rssi;
		newRec.remainingEnergy = energy;
		neighborTable.push_back(newRec);
		neighbors.push_back(nodeID);
}


void GA_C::processBufferedPacket()
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
		GARoutingPacket *netPacket = dynamic_cast <GARoutingPacket*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
}

void GA_C::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void GA_C::setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void GA_C::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void GA_C::levelTxPower(int linkBudget)
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

void GA_C::readXMLparams()
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

void GA_C::finishSpecific()
{

	
	
}


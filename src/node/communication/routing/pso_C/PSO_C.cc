#include "PSO_C.h"

typedef popot::algorithm::ParticleSPSO::VECTOR_TYPE TVector;
typedef CHSelectionProblem CHSelectionProblem;

Define_Module(PSO_C);

void PSO_C::startup()
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
	
	networkInfo.reserve(networkSize);

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

void PSO_C :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		PSORoutingPacket *netPacket = new PSORoutingPacket("PSO-C routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setPSORoutingPacketKind(PSO_ROUTING_DATA_PACKET);
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

void PSO_C :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	PSORoutingPacket *netPacket = dynamic_cast <PSORoutingPacket*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getPSORoutingPacketKind()) 
	{

		case PSO_ROUTING_FIND_NEIGHBOURS_PACKET:
     		{
			nodeData info = netPacket->getInfo();
                        int n_id = info.id;
			double n_rssi = rssi;
			double n_remainingEnergy = info.energy;
			updateNeighborTable(n_id,n_rssi,n_remainingEnergy);

			break;	
     		}

		case PSO_ROUTING_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				int source = atoi(netPacket->getSource());
				//actualMembers.push_back(source);
				trace() << "CH, Aggregate Data Frame from "<< source << "\n";
				bufferAggregate.push_back(*netPacket);
				double p = 1 / totalPackets;
				collectOutput("PDR","",p);
					
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


void PSO_C :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		par("energy") = 18720 - resMgrModule->getSpentEnergy();
		trace() << "PSO_C - START ROUND NO. " << roundNumber;
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
			setTimer(RUN_PSO,6.0);	 
		}

		else
		{
			setTimer(RUN_CDDP,7.0);
			setTimer(ENTER_STEADY_PHASE,8.0);
        	}

		//setTimer (TEST, 99.0);

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
		PSORoutingPacket *crtlPkt = new PSORoutingPacket("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setPSORoutingPacketKind(PSO_ROUTING_FIND_NEIGHBOURS_PACKET);
		crtlPkt->setInfo(info);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case RUN_PSO:
	{
		//trace() << "In RUN_PSO";
		//generateGraph();

		RNG_GENERATOR::rng_srand();
  	    	RNG_GENERATOR::rng_warm_up();

	    	numOfCHs =  ( networkSize * percentage ) / 100;
	    	
            	CHSelectionProblem p(numOfCHs,networkInfo,eligibleNodes);

		auto algo = popot::algorithm::spso2006(50,numOfCHs,
                                         [&p] (size_t index) -> double { return p.get_lbound(index); },
                                         [&p] (size_t index) -> double { return p.get_ubound(index); },
                                         [&p] (double fitness, int epoch) -> bool { return p.stop(fitness, epoch);},
                                         [&p] (TVector &pos) -> double { return p.evaluate(pos.getValuesPtr());}
                                         );


  		// We now run our algorithm

		double fitness = 1000000;
		double iterationNumber = 0;

		for (int i = 0 ; i < 500 ; i++)
		{
  			algo.step();
	trace() << "Fitness = "<< algo.getBest().getFitness() << " Iteration Number " << algo.epoch << "\n";

			if (algo.getBest().getFitness() < fitness)
			{
				fitness = algo.getBest().getFitness();
				iterationNumber = algo.epoch;
			}

		}
		
		trace() << "Best fitness = " << fitness << " at iteration no. "<< iterationNumber << "\n";

		collectOutput("Convergence","",iterationNumber);
  		//algo.generateGraph("connections.dot");

  		for (int i = 0 ; i < numOfCHs ; ++i)
  		{ 
      			int index =  algo.getBest().getPosition()[i];
			nodeInfo cand = eligibleNodes[index];
      			clusterHeads.push_back(cand);
		}

		vector<nodeInfo> clustered = p.clusterTheNetwork(clusterHeads);
		trace() << "clustered size " << clustered.size() << "\n";

 		string c;		
		stringstream out;
  		
		trace() << "PSO_C Candidates \n"; 
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

void PSO_C :: initProtocol()
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

void PSO_C :: runCDDP()
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

void PSO_C::generateGraph()
{
	cModule *n; 
	cModule *c;
	ResourceManager *r; 
	VirtualMobilityManager *l;
		

	ofstream myfile;
	myfile.open("TiKz-Trace.tex", std::ios::app);

	myfile <<"\\documentclass{article}\n";
	myfile<<"\\usepackage{tikz}\n";
	myfile<<"\\usetikzlibrary{shapes}\n";
	myfile<<"\\usetikzlibrary{arrows}\n";
	myfile<<"\\begin{document}\n";
	myfile<<"\\begin{tikzpicture}\n";
	myfile<<"[scale = .2,auto = left,every node/.style={circle,fill=blue!20}]\n";
       	for (int i = 0 ; i < networkSize ; i++)
	{
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
		myfile<<"\\node (n"<<i<<") at ("<<l->getLocation().x<<","<<l->getLocation().y<<") {"<<i<<"};\n"; 

	}
		
	myfile<<"\\end{tikzpicture}\n";
	myfile<<"\\end{document}\n";
	myfile.close();

}


void PSO_C::sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		PSORoutingPacket *aggrPacket = new PSORoutingPacket("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setPSORoutingPacketKind(PSO_ROUTING_DATA_PACKET);
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

void PSO_C::updateNeighborTable(int nodeID , double rssi , double energy)
{
		NeighborRecord newRec;
		newRec.id = nodeID;
		newRec.rssi = rssi;
		newRec.remainingEnergy = energy;
		neighborTable.push_back(newRec);
		neighbors.push_back(nodeID);
}


void PSO_C::processBufferedPacket()
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
		PSORoutingPacket *netPacket = dynamic_cast <PSORoutingPacket*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
}

void PSO_C::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void PSO_C::setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void PSO_C::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void PSO_C::levelTxPower(int linkBudget)
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

void PSO_C::readXMLparams()
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

void PSO_C::finishSpecific()
{
	//trace()<< "PSO-C - finishSpecific " << SIMTIME_DBL(simTime()) << "\n";
}


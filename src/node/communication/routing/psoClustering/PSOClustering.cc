#include "PSOClustering.h"

typedef popot::algorithm::ParticleSPSO::VECTOR_TYPE TVector;
typedef RoutingProblem RoutingProblem;
typedef ClusteringProblem ClusteringProblem;

Define_Module(PSOClustering);

void PSOClustering::startup()
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
	
	routingSwarmSize = par("routingSwarmSize");

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
	initialEnergy = getParentModule()->getParentModule()->getSubmodule("ResourceManager")->par("initialEnergy");

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

	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 1; 
	declareOutput("PDR");

	declareOutput("Number of unclustered nodes");
	declareOutput("Average Number of Cluster Heads per Round");
      	declareOutput("Average number of unclustered nodes");
	declareOutput("Number of data packets received at BS");
	declareOutput("Execution Time");
        declareOutput("Average Number of Active Nodes per Round");
	//declareOutput("Weighted Encoding Best Fitness");
	//declareOutput("Periority Encoding Best Fitness");

	readXMLparams();
	setTimer(START_ROUND,0.0);
}

void PSOClustering :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		PSOClusteringPacket *netPacket = new PSOClusteringPacket("PSO-Clustering routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setPSOClusteringPacketKind(PSO_CLUSTERING_DATA_PACKET);
		netPacket->setByteLength(dataPacketSize);
		netPacket->setSource(SELF_NETWORK_ADDRESS);
		encapsulatePacket(netPacket, pkt);
		// If cluster formation ended and you r not CH, then set dest. to CH and buffer packet
		if (!isCH && endFormClus)
		{
			stringstream buffer;
			buffer << myCH.id;
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

void PSOClustering :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	PSOClusteringPacket *netPacket = dynamic_cast <PSOClusteringPacket*>(pkt);

	if (!netPacket) return;
  	
	switch (netPacket->getPSOClusteringPacketKind()) 
	{

		case PSO_CLUSTERING_FIND_NEIGHBOURS_PACKET:
     		{
			nodeData5 info = netPacket->getInfo();
                        int n_id = info.id;
			double n_rssi = rssi;
			double n_remainingEnergy = info.energy;
			updateNeighborTable(n_id,n_rssi,n_remainingEnergy);

			string nbrs = par("neighbours").stringValue();

			stringstream out;
 			string nbrr;
			out <<  nbrs << n_id << "," << n_rssi << "," << n_remainingEnergy << ";";
			nbrr = out.str();
 			par("neighbours") = nbrr;
			break;	
     		}

		case PSO_CLUSTERING_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			if (isCH){
				trace() << "Node " << self << " Aggregate Data Frame from "<< netPacket->getSource()<<"\n";
				bufferAggregate.push_back(*netPacket);	
				double p = 1 / totalPackets;
				collectOutput("PDR","",p);
			}

			else if (isSink) 
			{
			trace() << "Sink Processing Data Packet from "<< netPacket->getSource() <<" \n";
				double n = netPacket->getNumOfPackets();
				//trace() << "n= "<< n << "\n";
				collectOutput("Number of data packets received at BS","",n);
				toApplicationLayer(decapsulatePacket(netPacket));
			}

			else if (isRelayNode)
			{
				PSOClusteringPacket *fwdPkt = netPacket->dup();
				fwdPkt->setDestination(SINK_NETWORK_ADDRESS);
				//tempTXBuffer.push(fwdPkt);
				string dstn;
				ostringstream convert;   
  				convert << nextHop;      
  				dstn = convert.str();
				trace()<<"rcvd will send to "<< dstn << "\n";
				toMacLayer(netPacket->dup(),resolveNetworkAddress(dstn.c_str()));
				//bufferPacket(netPacket->dup());
			}
			break;	
		}
		
	}   
}


void PSOClustering :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{

		par("neighbours") = "";
		par("numOfNbrs") = 0;
		par("energy") = initialEnergy - resMgrModule->getSpentEnergy();
		setStateRx();
		
		trace() << "PSO CLUSTERING - START ROUND NO. " << roundNumber;
		
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
		startTime = 0;
	        endTime = 0;
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
		
		double timer = uniform (0.0,3.0);
     
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
		//setPowerLevel(maxPower);
		nodeData5 info;
		info.id = self;
		info.energy = par("energy").doubleValue();
		PSOClusteringPacket *crtlPkt = new PSOClusteringPacket("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setPSOClusteringPacketKind(PSO_CLUSTERING_FIND_NEIGHBOURS_PACKET);
		crtlPkt->setInfo(info);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case RUN_PSO:
	{
		//trace() << "In RUN_PSO";
                 startTime = SIMTIME_DBL(simTime());
		initProtocol();	
                 RNG_GENERATOR::rng_srand();
		RNG_GENERATOR::rng_warm_up();

	    	numOfCHs =  ( noLiveNodes * percentage ) / 100;
		trace() << "Number of CHS = "<< numOfCHs << "\n";
		TVector m;
		ClusteringProblem p(numOfCHs,networkInfo,eligibleNodes,adjacencyMatrix);
  	    	//auto topology = popot::PSO::topology::ring_fillNeighborhoods<&m,true>;
		//Let's create a swarm 
		// we might use spso2006, spso2007 or spso2011
		auto algo = popot::algorithm::spso2011(50,numOfCHs,
                                         [&p] (size_t index) -> double { return p.get_lbound(index); },
                                         [&p] (size_t index) -> double { return p.get_ubound(index); },
                                         [&p] (double fitness, int epoch) -> bool { return p.stop(fitness, epoch);},
                                         [&p] (TVector &pos) -> double { return p.evaluate(pos.getValuesPtr());}
                                         );


  		// We now run our algorithm
                t_ini = clock();
  		algo.run();
  		//algo.generateGraph("connections.dot");
		
		for (int i = 0 ; i < numOfCHs ; ++i)
  		{ 
      			int index =  algo.getBest().getPosition()[i];
			NodeControlInfo cand = eligibleNodes[index];
			heads.push_back(cand.id);
      			clusterHeads.push_back(cand);
		}

		vector<NodeControlInfo> clustered = p.clusterTheNetwork(clusterHeads);
		//trace() << "clustered size " << clustered.size() << "\n";

 		string c;		
		stringstream out;
  		
		trace() << "PSO_Clustering candidates are:  \n"; 
		for (int i = 0 ; i < clustered.size() ; i++)
		{
			int cand =  clustered[i].id;
			trace() << cand << " ";
			out << cand << ",";
			//trace() << "Number of members = " << clustered[i].members.size()<<"\n";
			for (int j = 0 ; j < clustered[i].members.size() ; j++)
			{
				int member = clustered[i].members[j].id;
				//trace() << "member " << member << " ";
				out << member << " ";
			}
			out << ";";
		}
		trace() << "\n";

		c = out.str();
		par("networkInformation") = c;

		trace() << " Network Info " << c << " Fitness = " << algo.getBest().getFitness()<<"\n";
				  
 		setTimer (FIND_ROUTES, 0.0 );
		break;
	}

	case FIND_ROUTES:
	{
	   	trace() << " In FIND_ROUTES: 2nd tier PSO: Round "<< roundNumber << "\n" ; 
		RNG_GENERATOR::rng_srand();
  	   	RNG_GENERATOR::rng_warm_up();

           	size_t dimension = networkInfo.size();

           	RoutingProblem p2(dimension , adjacencyMatrix , heads , networkInfo);
  	    
  	   	// Let's create a swarm 
  	   	// we might use spso2006, spso2007 or spso2011
  	   	auto algo2 = popot::algorithm::spso2011(50,dimension,
                                         [&p2] (size_t index) -> double { return p2.get_lbound(index); },
                                         [&p2] (size_t index) -> double { return p2.get_ubound(index); },
                                         [&p2] (double fitness, int epoch) -> bool { return p2.stop(fitness, epoch);},
                                         [&p2] (TVector &pos) -> double { return p2.evaluate(pos.getValuesPtr());}
                                         );


		algo2.run();
		t_fin = clock();
                double secs = (double) (t_fin - t_ini);
  		secs = secs / CLOCKS_PER_SEC;
		trace() << "Execution time =  " << secs << "\n ";
                collectOutput("Execution Time","",secs);

   		stringstream out1;
           	string c1;

	   	double priorities[networkSize];

           	for(int i = 0 ; i < networkSize ; ++i)
           	{ 
              		double hop =  algo2.getBest().getPosition()[i];
	      		priorities[i] = hop;
	    	}

           	//c1 = out1.str();
	  	vector<Path> routes= p2.decodeRoutingParticle (priorities); 
	   	InterClusterRoutes = routes;
	   	trace() <<" Optimal Paths are : \n";
	   
	   	for (int x = 0 ; x < routes.size() ; x++)
    	   	{
     			for (int y = 0 ; y < routes[x].route.size() ; y++)
     			{
				int hop = routes[x].route[y] ;
     				trace() << hop << " ";
				out1 << hop << " ";
     			}
			out1 << ";" ;
     			trace() << " \n"; 
	  	}
            	c1 = out1.str();
 	    	par("routes") = c1;
	    	trace()<< "route is " << c1 << " Fitness = " << algo2.getBest().getFitness();
		//collectOutput("Periority Encoding Best Fitness", algo2.getBest().getFitness());
		 endTime = SIMTIME_DBL(simTime());
		  double totalTime = endTime;
		
	  break;
	}
	
	
	
	case RUN_CDDP:
	{
		runCDDP();
                break;
	}

	case ENTER_STEADY_PHASE:
	{
		trace()<< "PSOClustering - STEADY_PHASE I will start to send data at " << SIMTIME_DBL(simTime()) << "\n";
		if (!isCH && !isCM && !isRelayNode) setStateSleep(); // I am not clustered
	
		if (isCH)
		{
			if (clusterMembers.size()!=0)
			{
				clusterLength = clusterMembers.size();
				setTimer(START_SLOT, clusterLength * slotLength);
			//trace()<< "CH , member size > 0 , clusterLength ="<<clusterLength<<"\n";
			}
			else
			{
				setTimer(START_SLOT, slotLength);	
			}
		}
		else if (isCM)
		{
			if (!isRelayNode) setStateSleep();
			setTimer(START_SLOT, myTDMATurn * slotLength);
		}
		break;
	}
    	
	case START_SLOT:
	{
		double timer = uniform (0.0,1.0);
		if (!isCH && !isCM && !isRelayNode) setStateSleep(); // I am not clustered
		if(isCH && clusterMembers.size()==0) setTimer(START_SLOT, slotLength);	
		else setTimer(START_SLOT, clusterLength * slotLength);
				
		if (isCH) 
		{
			sendAggregate(); 
			processBufferedPacket();
		}

		else 
		{
			processBufferedPacket();
			setTimer(END_SLOT, slotLength);
		}
		break;
	}

	case END_SLOT:
	{
		if (!isSink && !isCH && !isRelayNode) setStateSleep();
		break;
	}
	
	case TEST:
	{
		
		break;
	}

   }
}

void PSOClustering::updateNeighborTable(int nodeID , double rssi , double energy)
{
		NeighborRecord newRec;
		newRec.id = nodeID;
		newRec.rssi = rssi;
		newRec.remainingEnergy = energy;
		neighborTable.push_back(newRec);
		
		par("numOfNbrs") = neighborTable.size();
}


void PSOClustering :: runCDDP()
{
 	cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	
 	string info = tmpModule->par("networkInformation").stringValue();
	string routes = tmpModule->par("routes").stringValue();
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
					trace() << "I am a CH !!! \n";
				}
				i++;
			}
			else
			{
				//trace() << "Members are " << d;
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
						
							myCH.id = ch;
							myCH.rssi = getRSSI(ch);
							nextHop = ch;
							isCM = true;
							myTDMATurn = j;
						//trace() << " CM, my TDMA is " <<myTDMATurn <<" \n";
					}
					j++;
					if (myCH.id == ch ) clusterLength = j;
				}
				
			}

		}
	}

	
	
	char_separator<char> sep1(";");
    	
	tokenizer<char_separator<char>> tokens1(routes, sep1);

    	for (const auto& t : tokens1) 
	{
		char_separator<char> sep2(" ");
		tokenizer<char_separator<char>> data(t, sep2);
		int i = 0;
		int relay;

	for (tokenizer<char_separator<char>>::iterator it = data.begin() ; it != data.end() ; ++it) 			{
			relay = stoi (*it);
					
			if (self == relay)
			{
				isRelayNode = true;
				nextHop = stoi (*++it);
				trace() << "I am a Relay Node !!! \n";
				trace() << "Next Hop "<< nextHop << "\n";
				
			}
		}
	}

	double numberRounds = numberOfRounds;
	if (isCH  || isRelayNode) collectOutput("Average Number of Active Nodes per Round","",1/numberRounds);
	
	if (!isCH && !isCM && !isRelayNode)
	{
		trace()<<"Iam not clustered!!\n";
		trace()<<"numberOfRounds "<<numberOfRounds<<"\n";
		
		if (!isCH && !isCM )collectOutput("Number of unclustered nodes","",1);
		if (!isCH && !isCM )
		{
			collectOutput("Average number of unclustered nodes","",1/numberRounds);
			setStateSleep(); // I am not clustered
		}
	} 
	//trace() << "My Cluster Length = " << clusterLength <<"\n";
	endFormClus = true;
}

double PSOClustering :: getRSSI(int nbr)
{
	double nbrRSSI;

	for (int i = 0 ; i < neighborTable.size() ; i++)
	{
		if (neighborTable[i].id == nbr)
		{
			nbrRSSI = neighborTable[i].rssi;
			break;
		}

	}
	//trace() << "nbrRSSI "<< nbrRSSI <<"\n";
	return nbrRSSI;
}	

void PSOClustering::finishSpecific()
{
	trace()<< "PSOClustering - finishSpecific \n";
	trace()<< "neighborTable size "<< neighborTable.size() << "\n";
	//if (isSink) generateGraph();
}


void PSOClustering::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}



void PSOClustering :: initProtocol()
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



void PSOClustering::sendAggregate()
{
	string dst;
	ostringstream convert;   
  	convert << nextHop;      
  	dst = convert.str();

	if(bufferAggregate.size() != 0)
	{	
		//double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		//double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
	PSOClusteringPacket *aggrPacket = new PSOClusteringPacket("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setPSOClusteringPacketKind(PSO_CLUSTERING_DATA_PACKET);
		aggrPacket->setSource(SELF_NETWORK_ADDRESS);
		aggrPacket->setDestination(SINK_NETWORK_ADDRESS);
		//trace()<< "buffer size = "<<bufferAggregate.size() <<"\n";
		aggrPacket->setNumOfPackets(bufferAggregate.size());
		
		ApplicationPacket *newPacket = new ApplicationPacket("App gen pkt", APPLICATION_PACKET);
		newPacket->setData(0);
		newPacket->getAppNetInfoExchange().destination = string("0");
		newPacket->getAppNetInfoExchange().source = selfAddress;
		newPacket->getAppNetInfoExchange().timestamp = simTime();
		newPacket->setApplicationID(applicationID.c_str());
		encapsulatePacket(aggrPacket, newPacket);

		bufferPacket(aggrPacket);
		bufferAggregate.clear();
	}
}

void PSOClustering::processBufferedPacket()
{
	string dst;
	ostringstream convert;   
  	convert << nextHop;      
  	dst = convert.str();
		
	trace() << self << " my next hop is " << dst << "\n";
	while (!tempTXBuffer.empty())
	{
		cPacket *pkt = tempTXBuffer.front();	
		PSOClusteringPacket *netPacket = dynamic_cast <PSOClusteringPacket*>(pkt);
		//netPacket->setDestination(SINK_NETWORK_ADDRESS);
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}

	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		trace() << "Sending \n";
		TXBuffer.pop();
	}	
}

void PSOClustering::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void PSOClustering::setStateTx()
{
	send(createRadioCommand(SET_STATE, TX), "toMacModule");	
}
void PSOClustering::setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void PSOClustering::levelTxPower(int linkBudget)
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

void PSOClustering::readXMLparams()
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


bool cmpCHsRssi(CH a, CH b){
	return (a.rssi > b.rssi);
}


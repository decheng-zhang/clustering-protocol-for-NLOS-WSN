#include "LeachCentralized.h"

Define_Module(LeachCentralized);

void LeachCentralized::startup()
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
	epoch = par("epoch");

	/*--- Class parameters ---*/
	roundNumber=0;
	networkInformation = " ";

	initialTemp = 200;
	coolingRate = 0.01;

	clusterMembers.clear();
	networkInfo.clear();
	eligibleNodes.clear(); 
	
	
	isCH = false;
	isCM = false;
	endFormClus = false;

	myCH = 0;
	myTDMATurn = 0;

	noLiveNodes = 0;
	
	theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");

	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 1; 
	declareOutput("PDR");
	
	declareOutput("Number of unclustered nodes");
      	declareOutput("Average number of unclustered nodes");
	declareOutput("Number of packets received per cluster head");
	declareOutput("Number of data packets received at BS");

	
	readXMLparams();
	setTimer(START_ROUND,0.0);
}

void LeachCentralized :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		LeachCentralizedRoutingPacket *netPacket = new LeachCentralizedRoutingPacket("Leach-C routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setLeachCentralizedRoutingPacketKind(LEACHCENTRALIZED_ROUTING_DATA_PACKET);
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

void LeachCentralized :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{

	LeachCentralizedRoutingPacket *netPacket = dynamic_cast <LeachCentralizedRoutingPacket*>(pkt);

	if (!netPacket)
		return;

	switch (netPacket->getLeachCentralizedRoutingPacketKind()) 
	{

		case LEACHCENTRALIZED_FIND_NEIGHBOURS_PACKET:
     		{
			nodeData1 info = netPacket->getInfo();
                        int n_id = info.id;
			double n_rssi = rssi;
			double n_remainingEnergy = info.energy;
			updateNeighborTable(n_id,n_rssi,n_remainingEnergy);

			break;	
     		}

		case LEACHCENTRALIZED_ROUTING_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();
			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				int source = atoi(netPacket->getSource());
				//actualMembers.push_back(source);
				trace() << "Node " << self << " Aggregate Data Frame \n";
				double p = 1 / totalPackets;
				collectOutput("PDR","",p);
				bufferAggregate.push_back(*netPacket);	
			}
			else if (dst.compare(SINK_NETWORK_ADDRESS) == 0 && isSink) 
			{
				int n = netPacket->getNumOfPackets();
				trace() << "n= "<< n << "\n";
				collectOutput("Number of data packets received at BS","",n);
				trace() << "Node " << self << " Processing Data Packet \n";
				toApplicationLayer(decapsulatePacket(netPacket));
			}
			break;	
		}
	}
	
}


void LeachCentralized :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		//trace() << "LEACH_C - START ROUND NO. " << roundNumber;
		par("energy") = 18720 - resMgrModule->getSpentEnergy();
		setStateRx();

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

		noLiveNodes = 0;
		
		double timer = uniform (0.0,5.0);
     
		setTimer(FIND_NEIGHBORS,timer);

		myCH = 0;
		myTDMATurn = 0;
		clusterLength = 0;

		if (isSink) 
		{ 
	   		initProtocol();	
			setTimer(RUN_SA,6.0);	 
		}

		else
		{
			setTimer(RUN_CDDP,7.0);
			setTimer(ENTER_STEADY_PHASE,8.0);
        	}
		
		//setTimer(TEST,99.0);		

		roundNumber++;
		if (roundNumber >= (numberOfRounds)) break;
		setTimer(START_ROUND, roundLength);

		break;
	}

	case FIND_NEIGHBORS:
     	{  
		nodeData1 info;
		info.id = self;
		info.energy = par("energy").doubleValue();
   LeachCentralizedRoutingPacket *crtlPkt = new LeachCentralizedRoutingPacket("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setLeachCentralizedRoutingPacketKind(LEACHCENTRALIZED_FIND_NEIGHBOURS_PACKET);
		crtlPkt->setInfo(info);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case RUN_SA:
	{
		//trace() << "In RUN_SA \n";

		numOfCHs =  ( networkSize * percentage ) / 100;
						
		//trace() << " Number of CHS = "<< numOfCHs << "\n";
		
		
		// Initialize intial solution
		vector<NodeInfo> currentSolution = generateInitialSolution();
		double fitness = evaluateSolution(currentSolution);
		double bestFitness = fitness;

		//Set as current best
		vector<NodeInfo> best;
  		best.reserve(currentSolution.size());
  		std::copy(currentSolution.begin(),currentSolution.end(),back_inserter(best));
		 
            	//Loop until system has cooled
		for (int i = 0 ; i < 200 ; i++) 
		{

			//trace() << " In While \n";
            		// Create new neighbour tour
            		vector<NodeInfo> newSolution;
			newSolution.reserve(currentSolution.size());
  			std::copy(currentSolution.begin(),currentSolution.end(),back_inserter(newSolution));
		 

            		// Get a random positions in the tour
            		int candidatePos = rand() % currentSolution.size();
            		
           		//change CH at selected Pos.
			newSolution[candidatePos] = getRandomNode(currentSolution);

			/*trace() << " New soln. is \n";

			for (int i = 0 ; i < newSolution.size() ; i++)
			{
				trace()<< newSolution[i].id << " ";
			}
			
            		trace() << "\n";*/

            		// Get energy of solutions

			double currentEngery = evaluateSolution(currentSolution);
            		double neighbourEngery = evaluateSolution(newSolution);

			//trace() << "Current Fitness = " << currentEngery << " \n";
			//trace() << "Neighbour Fitness = " << neighbourEngery << " \n";

			double r = ((double) rand() / (RAND_MAX));

            		// Decide if we should accept the neighbour
            		if (acceptanceProbability(currentEngery, neighbourEngery, initialTemp) >r) 				{
				//trace()<< " going to accept \n";
				currentSolution.clear();
			std::copy(newSolution.begin(),newSolution.end(),back_inserter(currentSolution));
                	}

            		// Keep track of the best solution found
            		if (evaluateSolution(currentSolution) < evaluateSolution(best)) 
			{
				best.clear();
				std::copy(currentSolution.begin(),currentSolution.end(),back_inserter(best));               	}
            
            		// Cool system
            		initialTemp *= 1-coolingRate;
	        }
		
		vector<NodeInfo> clustered = clusterTheNetwork(best);

		string c;		
		stringstream out;
  		
	 	trace() << "LEACH_C Candidates: \n";
		for (int i = 0 ; i < clustered.size() ; i++)
		{
			int cand =  clustered[i].id;
			trace() << cand << " ";
			out << cand << ",";
			for (int j = 0 ; j < clustered[i].members.size() ; j++)
			{
				int member = clustered[i].members[j].id;
				out << member << " ";
			}
			out << ";";
		}
		//trace() << "\n";
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
		//trace()<< "STEADY_PHASE\n";
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

vector<NodeInfo> LeachCentralized :: clusterTheNetwork(vector<NodeInfo> candidates)
{
	int numOfCHs = candidates.size();
	int numOfnodes = networkInfo.size();

	sort(networkInfo.begin(), networkInfo.end(),ID_Comparer());
    	sort(candidates.begin(), candidates.end(),ID_Comparer());
    	vector<NodeInfo> network;
	set_difference(networkInfo.begin(), networkInfo.end(), candidates.begin(), candidates.end(), back_inserter(network),ID_Comparer());

        for (int i = 1 ; i < network.size() ; i++)
     	{
       		double minDistance = 10000;
       		int ch = 0;

       		for (int j = 0; j < numOfCHs ; j++)
       		{
         		double dist = sqrt (pow ((network[i].x - candidates[j].x),2) + pow ((network[i].y - candidates[j].y),2) );
         
       
        		if ((dist < minDistance) && (network[i].id != candidates[j].id))
         		{
           			minDistance = dist;
           			ch = candidates[j].id;
	 		}
         
       		}

       		//myfile << " node " << networkInf[i].id << " belongs to CH " << ch << " \n "; 
       		// Push the node to CM
       		for (int x = 0; x < candidates.size(); x++)
       		{ 
          		//myfile << " inside push id: "<< candidates[x].id <<" ch "<< ch <<" \n";
         		if (ch == candidates[x].id)
         		{
           			// myfile << " inside push to  " << ch;
            			candidates[x].members.push_back(network[i]);
			}

       		}
     }
	
     return candidates;

}

void LeachCentralized :: initProtocol()
{
	noLiveNodes = 0;

	networkInfo.clear();
	eligibleNodes.clear();
	
	double totalEnergy = 0;

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

		NodeInfo info;
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

void LeachCentralized :: runCDDP()
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
						//trace() << " I am a CH, will add " <<stoi(m) <<" to members \n";
					}
					if ( self == stoi(m) )
					{
						if (!isCH && !isSink)
						{
							myCH = ch;
							isCM = true;
							myTDMATurn = j;
						//trace() << " CM, my TDMA is " <<myTDMATurn <<" \n";
						}
						
					}
					j++;
					if (myCH == ch ) clusterLength = j;
				}
				
			}

		}
	}
	endFormClus = true;
	//trace() << "My Cluster Length = " << clusterLength <<"\n";
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
        	collectOutput("Average number of unclustered nodes","",av);  
	}
	
	//trace() << "My Cluster Length = " << clusterLength <<"\n";

	

}

vector<NodeInfo> LeachCentralized::generateInitialSolution()
{
	vector<NodeInfo> candidates;
	vector<NodeInfo> networkData;

	for (int j = 0 ; j < eligibleNodes.size() ; j++)
	{
		NodeInfo n = eligibleNodes[j];
		networkData.push_back(n);
	}

	random_shuffle(networkData.begin(), networkData.end());

	for (int i = 0 ; i < numOfCHs ; i++)
	{
		candidates.push_back(networkData[i]);
	}

	return candidates;
}


NodeInfo LeachCentralized::getRandomNode(vector<NodeInfo> currentSolution)
{
	NodeInfo random;
	int n = rand() % eligibleNodes.size();

	while (contains(currentSolution,eligibleNodes[n]))
	{
		n = rand() % eligibleNodes.size();
	}

	random = eligibleNodes[n];
	return random;
}

bool LeachCentralized::contains(vector<NodeInfo> currentSolution,NodeInfo n)
{
	bool contained = false;

	for (int i = 0 ; i < currentSolution.size() ; i++)
	{
		if (currentSolution[i].id == n.id)
		{
			contained = true;
			break;
		}

	}

	return contained;
}


double LeachCentralized::evaluateSolution(vector<NodeInfo> candidates)
{
	sort(networkInfo.begin(), networkInfo.end(),ID_Comparer());
    	sort(candidates.begin(), candidates.end(),ID_Comparer());
    	vector<NodeInfo> network;
	set_difference(networkInfo.begin(), networkInfo.end(), candidates.begin(), candidates.end(), back_inserter(network),ID_Comparer());

	double totalDistance = 0;
	/******* Cluster N/W ***************/

	int numOfCHs = candidates.size();
	int numOfnodes = network.size();

	for (int i = 1 ; i < network.size() ; i++)
     	{
		double minDistance = 10000;
       		int ch = 0;

       		for (int j = 0; j < numOfCHs ; j++)
       		{
         		double dist = sqrt (pow ((network[i].x - candidates[j].x),2) + pow ((network[i].y - candidates[j].y),2) );
         
       
        		if ((dist < minDistance) && (network[i].id != candidates[j].id))
         		{
           			minDistance = dist;
           			ch = candidates[j].id;
	 		}
         
       		}

       		//myfile << " node " << networkInf[i].id << " belongs to CH " << ch << " \n "; 
       		// Push the node to CM
       		for (int x = 0; x < candidates.size(); x++)
       		{ 
          		//myfile << " inside push id: "<< candidates[x].id <<" ch "<< ch <<" \n";
         		if (ch == candidates[x].id)
         		{
           			// myfile << " inside push to  " << ch;
            			candidates[x].members.push_back(network[i]);
				totalDistance = totalDistance + minDistance;
         		}

       		}
     }
	
	//trace()<< "Total Distance is "<< totalDistance <<"\n";
	return totalDistance;
}

void LeachCentralized::sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		LeachCentralizedRoutingPacket *aggrPacket = new LeachCentralizedRoutingPacket("ClusterHead Aggredated Packet", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setLeachCentralizedRoutingPacketKind(LEACHCENTRALIZED_ROUTING_DATA_PACKET);
		aggrPacket->setSource(SELF_NETWORK_ADDRESS);
		aggrPacket->setDestination(SINK_NETWORK_ADDRESS);
		aggrPacket->setNumOfPackets(bufferAggregate.size());

		ApplicationPacket *newPacket = new ApplicationPacket("App generic packet", APPLICATION_PACKET);
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

void LeachCentralized::processBufferedPacket()
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
	LeachCentralizedRoutingPacket *netPacket = dynamic_cast <LeachCentralizedRoutingPacket*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}

	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}
	
}

void LeachCentralized::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void LeachCentralized::setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void LeachCentralized::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void LeachCentralized::levelTxPower(int linkBudget)
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

double LeachCentralized :: acceptanceProbability(int engery, int newEngery, double temperature) 
{
        // If the new solution is better, accept it
        if (newEngery < engery) 
	{
            return 1.0;
        }
        // If the new solution is worse, calculate an acceptance probability
        return exp((engery - newEngery) / temperature);
}

void LeachCentralized::updateNeighborTable(int nodeID , double rssi , double energy)
{
		NeighborRecord newRec;
		newRec.id = nodeID;
		newRec.rssi = rssi;
		newRec.remainingEnergy = energy;
		neighborTable.push_back(newRec);
		neighbors.push_back(nodeID);
}


void LeachCentralized::finishSpecific()
{

}

void LeachCentralized::readXMLparams()
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

void LeachCentralized::generateGraph()
{
	cModule *n; 
	cModule *c;
	ResourceManager *r; 
	VirtualMobilityManager *l;
		

	ofstream myfile;
	myfile.open("TiKz-Trace(Leach-C).tex", std::ios::app);

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

bool cmpCHeadsRssi(CHInfo a, CHInfo b){
	return (a.rssi > b.rssi);
}

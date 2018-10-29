#include "EEHC_LEACH.h"

Define_Module(EEHC_LEACH);

void EEHC_LEACH::startup()
{	
	/*--- The .ned file's parameters ---*/
	roundLength = par("roundLength");
	isSink = par("isSink");
	slotLength = par("slotLength");
	advPacketSize = par("advPacketSize");
	joinPacketSize = par("joinPacketSize");
	tdmaPacketSize = par("tdmaPacketSize");
	dataPacketSize = par("dataPacketSize");
	applicationID = par("applicationID").stringValue(); 
	numberOfRounds = par("numberOfRounds");
	
	/*--- Class parameters ---*/
	CHcandidates.clear();
	clusterMembers.clear();
	roundNumber=0;
	probability = 0;
	percentage = 0;
	isCH = false;
	isCM = false;
	endFormClus = false;
	isCt = false;

	m = 0.2;
	m0 = 0.5;
	beta = 2;
	alpha = 3;

	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");
initialEnergy = getParentModule()->getParentModule()->getSubmodule("ResourceManager")->par("initialEnergy");
	
	if (initialEnergy == 6240)
	{
		isNormalNode = true;
		trace() << "Normal Node \n";
	}
	else if (initialEnergy == ( beta * 6240))
	{
		isSuperNode = true;
		trace() << "Super Node \n";
	}
	else if (initialEnergy == ( alpha * 6240))
	{
		isAdvancedNode = true;
		trace() << "Advanced Node \n";
	}

	k_opt = 2.61 * sqrt ( networkSize / 6.28 );
	p_opt = k_opt / networkSize;
	percentage = p_opt;

	//trace() << "percentage =  "<< percentage << "\n";

	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 2; 
	declareOutput("PDR");

	declareOutput("Number of Cluster Heads");
	declareOutput("Average Number of Cluster Heads per Round");
	declareOutput("Number of unclustered nodes");
      	declareOutput("Average number of unclustered nodes");
	declareOutput("Number of data packets received at BS");

	readXMLparams();
	if(!isSink) setTimer(START_ROUND, 0.0);
}

void EEHC_LEACH::fromApplicationLayer(cPacket *pkt, const char *destination)
{	
		if(!isSink)
	{
		string dst(destination);
		EEHC_LEACH_Packet *netPacket = new EEHC_LEACH_Packet("data packet", NETWORK_LAYER_PACKET);
		netPacket->setEEHC_LEACH_PacketKind(EEHC_LEACH_DATA_PACKET);
		netPacket->setByteLength(dataPacketSize);
		netPacket->setSource(SELF_NETWORK_ADDRESS);
		encapsulatePacket(netPacket, pkt);
		if (!isCH && endFormClus)
		{
			CHInfo info = *CHcandidates.begin();
			stringstream buffer;
			buffer << info.src;
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

void EEHC_LEACH::fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{

	EEHC_LEACH_Packet *netPacket = dynamic_cast <EEHC_LEACH_Packet*>(pkt);

	if (!netPacket)
		return;

	switch (netPacket->getEEHC_LEACH_PacketKind()) {

		case EEHC_LEACH_DATA_PACKET:{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();
			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0){
				//trace() << "Node " << self << " Aggregate Data Frame \n";
				double p = 1 / totalPackets;
				collectOutput("PDR","",p);
				bufferAggregate.push_back(*netPacket);	
			}else if (dst.compare(SINK_NETWORK_ADDRESS) == 0 && isSink)  {
				//trace() << "Node " << self << " Processing Data Packet \n";
				int n = netPacket->getNumOfPackets();
				trace() << "n= "<< n << "\n";
				collectOutput("Number of data packets received at BS","",n);
				toApplicationLayer(decapsulatePacket(netPacket));
				//collectOutput("Number of Data Packets received at The Base Station", 1);
			}
			break;	
		}
		case EEHC_LEACH_ADV_PACKET:{
			
			if(!isCH && !isSink)
			{	
	//trace() << "Node " << self << " Received a Advertisement Message of node " << netPacket->getSource() << " with RSSI: " << rssi << "\n";	
				CHInfo rec;
				rec.src = atoi(netPacket->getSource());
				rec.rssi = rssi;
				CHcandidates.push_front(rec);
			}
			break;
		}
		case EEHC_LEACH_JOIN_PACKET:{
			string dst(netPacket->getDestination());
			if(isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0) {
				//trace() << "Node " << self << " Received a Join Request. Adding to clusterMembers\n";
				clusterMembers.push_back(atoi(netPacket->getSource()));
			}
	 		break;
		}
		case EEHC_LEACH_TDMA_PACKET:{
			if(!isCH && !isSink)
			{
				clusterLength = netPacket->getScheduleArraySize();
				for(int i=0; i<netPacket->getScheduleArraySize(); i++) {
					if (netPacket->getSchedule(i)==atoi(SELF_NETWORK_ADDRESS)) {
						isCM = true;	
						setStateSleep();
						setTimer(START_SLOT, i*slotLength);
				trace() << "Node " << self << " Received TDMA pkt, I am: "<< i << "th \n";
						trace()<< "Clustered\n";
						break;
					}			
				}
			}
			break;
		}
	}
	
		
}

void EEHC_LEACH::timerFiredCallback(int index)
{

	switch (index) {
		
		case START_ROUND:
		{
			setStateRx();
			endFormClus = false;
			CHcandidates.clear();
			clusterMembers.clear();
			if (getTimer(START_SLOT) != 0) { 
				cancelTimer(START_SLOT);
			}
			
			if (roundNumber >= 1/percentage) {
				roundNumber=0;
				isCt = false;
				isCH = false;
			}	
			
			double randomNumber = uniform(0,1);
			double timer = uniform(0,1);

			if(isCH) 
			{
				isCH = false;
				isCt = true;
			}
			if(isCt) 
			{
				probability = 0;
			}
			else 
			{
				if (roundNumber >= (1/percentage - 1))
				{
					probability = 1;
				}
				else 
				{
				   if (isNormalNode)
				   {
					percentage = (p_opt / (1 + m * ( alpha + m0 * beta)));
					trace()<< "Normal Node \n"; 
				   }
				   else if (isSuperNode)	
				   {
					percentage = (p_opt / (1 + m * ( alpha + m0 * beta))) * (1 + beta);
					trace()<< "Super Node \n"; 	
				   }
				   else if (isAdvancedNode)	
				   {
					percentage = (p_opt / (1 + m * ( alpha + m0 * beta))) * (1 + alpha);
					trace()<< "Advanced Node \n"; 
				   }
				
				  // trace() << "percentage = " << percentage << "\n";
				probability = percentage/(1-percentage*(roundNumber % (int) (1/percentage)));
				}
			}	
			if (randomNumber < probability) 
			{	
				setTimer(SEND_ADV, (timer));
				setTimer(MAKE_TDMA, 8.0+timer);
				trace() << "EEHC_LEACH Candidate: "<< self <<"\n";
				isCH=true;
				double c = 1;
				double numRounds = numberOfRounds;
				collectOutput("Number of Cluster Heads","",1);
				collectOutput("Average Number of Cluster Heads per Round","",c/numRounds);

			}
			if (!isCH) setTimer(JOIN_CH, (7.0 + timer));
			roundNumber++;
			//setTimer(TEST,99.8);
			double energy = 18720 - resMgrModule->getSpentEnergy();
			if ( energy < 2) collectOutput("Number of dead nodes",1);
			if (roundNumber >= (numberOfRounds)) break;
			setTimer(START_ROUND, roundLength);
			break;
		}
		case SEND_ADV:
		{	
		EEHC_LEACH_Packet *crtlPkt = new EEHC_LEACH_Packet("CH Adv. Packet", NETWORK_LAYER_PACKET);
			crtlPkt->setByteLength(advPacketSize);
			crtlPkt->setEEHC_LEACH_PacketKind(EEHC_LEACH_ADV_PACKET);
			crtlPkt->setSource(SELF_NETWORK_ADDRESS);
			crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
			toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
			//trace() << "Node " << self << " Sent Beacon";
			break;
		}
		case JOIN_CH:
		{
			if(CHcandidates.size()!=0)
			{	
				CHcandidates.sort(cmpRssi1);
			EEHC_LEACH_Packet *crtlPkt = new EEHC_LEACH_Packet("CM join", NETWORK_LAYER_PACKET);
				crtlPkt->setEEHC_LEACH_PacketKind(EEHC_LEACH_JOIN_PACKET);
				crtlPkt->setByteLength(joinPacketSize);	
				crtlPkt->setSource(SELF_NETWORK_ADDRESS);
				CHInfo info = *CHcandidates.begin();
				stringstream buffer;
				buffer << info.src;
				string dst = buffer.str();
				crtlPkt->setDestination(dst.c_str());			
				toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
				endFormClus = true;
				//trace() << "Node " << self << " Sent Join Request to " << dst;
			}
			break;
		}
		case MAKE_TDMA:
		{
			//trace() << "Make TDMA \n";
			if (clusterMembers.size()!=0)
			{
			EEHC_LEACH_Packet *crtlPkt = new EEHC_LEACH_Packet("TDMA", NETWORK_LAYER_PACKET);
				crtlPkt->setByteLength(tdmaPacketSize);
				crtlPkt->setEEHC_LEACH_PacketKind(EEHC_LEACH_TDMA_PACKET);
				crtlPkt->setSource(SELF_NETWORK_ADDRESS);
				crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
				clusterLength = clusterMembers.size();	
				crtlPkt->setScheduleArraySize(clusterMembers.size());	
				for(int i=0; i<clusterLength; i++) crtlPkt->setSchedule(i,clusterMembers[i]);
				toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
				//trace() << "Node " << self << " Sent TDMA pkt";
				setTimer(START_SLOT, clusterLength*slotLength);
			}
			else setTimer(START_SLOT, slotLength);					
			break;
		}
		case START_SLOT:
		{
			//trace() << "Start slot \n";
			if(isCH && clusterMembers.size()==0) setTimer(START_SLOT, slotLength);	
			else setTimer(START_SLOT, clusterLength*slotLength);
			if (isCH) {
				sendAggregate(); 
				processBufferedPacket();
				//trace() << "Node " << self << " Sent Pkt Aggr"  << "\n";
			}
			if (!isCH){
				//CHInfo info = *CHcandidates.begin();
				//int power = maxPower - ((info.rssi)-(sensibility));
				//levelTxPower(power);
				//trace() << "Node " << self << " Sent Data Packet"  << "\n";
				processBufferedPacket();
				setTimer(END_SLOT, slotLength);
			}
			break;
		}
		case END_SLOT:
		{
			trace() << "Node " << self << " Sleept"  << "\n";
			if (!isSink && !isCH) setStateSleep();
			break;
		}
		case TEST:
		{
			double c = 1;
			double numRounds = numberOfRounds;
			if (isCH)
			{			
				collectOutput("Number of Cluster Heads","",1);
				collectOutput("Average Number of Cluster Heads per Round","",c/numRounds);
			}

			if (!isCM && !isCH)
			{
				trace()<<"Iam not clustered!!\n";
				collectOutput("Number of unclustered nodes","",1);
				double av = c/numRounds;
				trace()<< "av = "<< av << "\n";
      				collectOutput("Average number of unclustered nodes","",av);
			} 
			break;
		}
	}
}

void EEHC_LEACH :: finishSpecific()
{
	
}

void EEHC_LEACH ::sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		EEHC_LEACH_Packet *aggrPacket = new EEHC_LEACH_Packet("CH Agg. Packet", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setEEHC_LEACH_PacketKind(EEHC_LEACH_DATA_PACKET);
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

void EEHC_LEACH::processBufferedPacket()
{
	string dst;
	CHInfo info = *CHcandidates.begin();
	if (isCH)
	{
		ostringstream convert;   
  		convert << 0;      
  		dst = convert.str();
	}
	else if (isCM)
	{
		ostringstream convert;   
  		convert << info.src;      
  		dst = convert.str();
	}
	while (!tempTXBuffer.empty())
	{
		cPacket *pkt = tempTXBuffer.front();	
		EEHC_LEACH_Packet *netPacket = dynamic_cast <EEHC_LEACH_Packet*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}

	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
}

void EEHC_LEACH ::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void EEHC_LEACH ::setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void EEHC_LEACH ::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void EEHC_LEACH ::levelTxPower(int linkBudget)
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

void EEHC_LEACH ::readXMLparams()
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

bool cmpRssi1(CHInfo a, CHInfo b){
	return (a.rssi > b.rssi);
}

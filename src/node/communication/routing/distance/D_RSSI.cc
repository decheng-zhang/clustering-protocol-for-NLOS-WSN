#include "D_RSSI.h"

Define_Module(D_RSSI);

void D_RSSI::startup()
{	
	readXMLparams();
	setTimer(START_ROUND,0.0);
}

void D_RSSI :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	
}

void D_RSSI :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	D_RSSI_Packet *netPacket = dynamic_cast <D_RSSI_Packet*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getD_RSSI_PacketKind()) 
	{

		case D_RSSI_ADV_PACKET:
     		{

			trace() << "I received an RSSI ADV packet from " << netPacket->getSource() << " RSSI = " << rssi << "\n";
			
			break;	
     		}
	}
}


void D_RSSI :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		double timer = uniform (0.0,5.0);
     
		setTimer(ADV_RSSI , 1.0 );
		setTimer(ADV_RSSI2 , 100.0 );
		
		break;
	}

	case ADV_RSSI:
     	{  
		trace() << "In ADV_RSSI \n";
		D_RSSI_Packet *crtlPkt = new D_RSSI_Packet("ADV RSSI",NETWORK_LAYER_PACKET);
		crtlPkt->setD_RSSI_PacketKind(D_RSSI_ADV_PACKET);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case ADV_RSSI2:
     	{  
		trace() << "In ADV_RSSI2 \n";
		D_RSSI_Packet *crtlPkt = new D_RSSI_Packet("ADV RSSI",NETWORK_LAYER_PACKET);
		crtlPkt->setD_RSSI_PacketKind(D_RSSI_ADV_PACKET);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	
    }
}

void D_RSSI::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void D_RSSI::setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void D_RSSI::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void D_RSSI::levelTxPower(int linkBudget)
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

void D_RSSI::readXMLparams()
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

void D_RSSI::finishSpecific()
{
	//trace()<< "PSO-C - finishSpecific " << SIMTIME_DBL(simTime()) << "\n";
}


//****************************************************************************
//*  Copyright (c) Federal University of Para, brazil - 2011                 *
//*  Developed at the Research Group on Computer Network and Multimedia      *
//*  Communication (GERCOM)     				             *
//*  All rights reserved    				                     *
//*                                                                          *
//*  Permission to use, copy, modify, and distribute this protocol and its   *
//*  documentation for any purpose, without fee, and without written         *
//*  agreement is hereby granted, provided that the above copyright notice,  *
//*  and the author appear in all copies of this protocol.                   *
//*                                                                          *
//*  Module:   LEACH Clustering Protocol for Castalia Simulator              *
//*  Version:  0.2                                                           *
//*  Author(s): Adonias Pires <adonias@ufpa.br>                              *
//*             Claudio Silva <claudio.silva@itec.ufpa.br>                   *
//****************************************************************************/

cplusplus {{
#include "RoutingPacket_m.h"
}}

class RoutingPacket;


enum EEHC_LEACH_Packet_Type
{
	EEHC_LEACH_ADV_PACKET 	= 1;		
	EEHC_LEACH_JOIN_PACKET 	= 2;		
	EEHC_LEACH_TDMA_PACKET 	= 3;		
	EEHC_LEACH_DATA_PACKET 	= 4;
};

packet EEHC_LEACH_Packet extends RoutingPacket
{	
	int EEHC_LEACH_PacketKind enum (EEHC_LEACH_Packet_Type);   
	int schedule[];		
	int numOfPackets;
};

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


enum EHE_LEACH_Packet_Type
{
	EHE_LEACH_ADV_PACKET 	= 1;		
	EHE_LEACH_JOIN_PACKET 	= 2;		
	EHE_LEACH_TDMA_PACKET 	= 3;		
	EHE_LEACH_DATA_PACKET 	= 4;
};

packet EHE_LEACH_Packet extends RoutingPacket
{	
	int EHE_LEACH_PacketKind enum (EHE_LEACH_Packet_Type);   
	int schedule[];		
	int numOfPackets;
};

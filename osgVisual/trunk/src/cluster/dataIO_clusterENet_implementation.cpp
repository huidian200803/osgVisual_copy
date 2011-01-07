/* -*-c++-*- osgVisual - Copyright (C) 2009-2010 Torben Dannhauer
 *
 * This library is based on OpenSceneGraph, open source and may be redistributed and/or modified under 
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * osgVisual requires for some proprietary modules a license from the correspondig manufacturer.
 * You have to aquire licenses for all used proprietary modules.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include "dataIO_clusterENet_implementation.h"

using namespace osgVisual;

int dataIO_clusterENet_implementation::activeENetInstances = 0;

dataIO_clusterENet_implementation::dataIO_clusterENet_implementation(std::string& receivedTransportContainer_)
: receivedTransportContainer(receivedTransportContainer_)
{
	std::cout << "Instantiated server class# "<< activeENetInstances << std::endl;

	enetInitialized = false;
	host = NULL;
	// Start ENet
	if (activeENetInstances++ == 0)
	{
		if( enet_initialize() != 0)
		{
			std::cout <<  "An error occurred while initializing ENet." << std::endl;
		}
		else
		{
			std::cout << "Starting ENet subsystem successful" << std::endl;
		}
	}
}

dataIO_clusterENet_implementation::~dataIO_clusterENet_implementation()
{
	// Delete Host Object if instantiated
	if(host)
		enet_host_destroy(host);

	// Stop ENet if it is the last element.
	if(--activeENetInstances == 0)
	{
		std::cout << "Close ENet subsystem" << std::endl;
		enet_deinitialize();
	}
	std::cout << "Destroyed server class# "<< activeENetInstances << std::endl;
}

bool dataIO_clusterENet_implementation::init(dataIO_clusterENet_implementation::role role_, unsigned short port_, int maxClients_, int maxChannels_, int maxInBandwidth_, int maxOutBandwidth_)
{
	port = port_;
	currentRole = role_;

	if(currentRole == dataIO_clusterENet_implementation::SERVER)
	{
		/* Bind the server to the default localhost.     */
		/* A specific host address can be specified by   */
		/* enet_address_set_host (& address, "x.x.x.x"); */

		address.host = ENET_HOST_ANY;
		/* Bind the server to port. */
		address.port = port;

		host = enet_host_create (& address /* the address to bind the server host to */, 
									 maxClients_      /* allow up to 32 clients and/or outgoing connections */,
									 maxChannels_      /* allow up to 2 channels to be used, 0 and 1 */,
									 maxInBandwidth_      /* assume any amount of incoming bandwidth */,
									 maxOutBandwidth_      /* assume any amount of outgoing bandwidth */);
		if (host == NULL)
		{
			std::cout <<  "An error occurred while trying to create an ENet server." << std::endl;
			return false;
		}
	}	// IF SERVER END

	if(currentRole == dataIO_clusterENet_implementation::CLIENT)
	{
		 host = enet_host_create (NULL /* create a client host */,
									 maxClients_      /* allow up to 32 clients and/or outgoing connections */,
									 maxChannels_      /* allow up to 2 channels to be used, 0 and 1 */,
									 maxInBandwidth_      /* assume any amount of incoming bandwidth */,
									 maxOutBandwidth_      /* assume any amount of outgoing bandwidth */);
		 if (host == NULL)
		{
			std::cout <<  "An error occurred while trying to create an ENet client." << std::endl;
			return false;
		}

	}	// IF CLIENT END

	enetInitialized = true;
	return true;
}

void dataIO_clusterENet_implementation::processEvents(int timeout_ms_)
{
	if(!enetInitialized)
		return;

	ENetEvent event;
	while(enet_host_service (host, & event, timeout_ms_) > 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				onConnect(&event);
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				onReceivePacket(&event);
			
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy (event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				onDisconnect(&event);
				break;
			
			case ENET_EVENT_TYPE_NONE:
				// nothing to do
				break;

			default: 
				std::cout << "Unknown Eventtype" << std::endl;
		}	// SWITCH CASE END
	}	// WHILE EVENT END

}

void dataIO_clusterENet_implementation::sendPacket( ENetPacket* packet_, enet_uint8 channelID_, unsigned int peerID_, bool autoFlush_ )
{
	// are connected peers available?
	if( peerList.size() == 0 )
	{
		//std::cout << "dataIO_clusterENet_implementation::sendPacket() - ERROR: No connected peer available!" << std::endl;
		
		// If we can't send the packet, we have to destroy it to free it's memory (In a case of send ENet takes ownership and frees after sending.).
		enet_packet_destroy (packet_);

		return;
	}

	// Client
	if(currentRole == dataIO_clusterENet_implementation::CLIENT)
	{
		enet_peer_send (peerList[0], channelID_, packet_);
	}

	// Server
	if(currentRole == dataIO_clusterENet_implementation::SERVER)
	{
		if(peerID_ < peerList.size())
			enet_peer_send (peerList[peerID_], channelID_, packet_);
		else
			std::cout << "dataIO_clusterENet_implementation::sendPacket() - ERROR: Peer #"<<peerID_<<" is not available, only peers 0-"<<(peerList.size()-1)<<" are connected!" << std::endl;
	}

	if(autoFlush_)
		enet_host_flush( host );
}

void dataIO_clusterENet_implementation::sendPacket( ENetPacket* packet_, enet_uint8 channelID_, std::string peerName_, bool autoFlush_ )
{
		// are connected peers available?
	if( peerList.size() == 0 )
	{
		std::cout << "dataIO_clusterENet_implementation::sendPacket() - ERROR: No connected peer available!" << std::endl;
		return;
	}

	// Client
	if(currentRole == dataIO_clusterENet_implementation::CLIENT)
	{
		enet_peer_send (peerList[0], channelID_, packet_);
	}

	// Server
	if(currentRole == dataIO_clusterENet_implementation::SERVER)
	{
		int peerID_=-1;
		for(unsigned int i=0;i<peerList.size();i++)	// Search peer
		{
			if( *((std::string*)peerList[i]->data) == peerName_ )
			{
				peerID_ = i;
				break;
			}
		}
		if( peerID_ >= 0 && peerID_ < (int)peerList.size())
			enet_peer_send (peerList[peerID_], channelID_, packet_);
		else
			std::cout << "dataIO_clusterENet_implementation::sendPacket() - ERROR: Peer #"<<peerID_<<" is not available, only peers 0-"<<(peerList.size()-1)<<" are connected!" << std::endl;
	}

	if(autoFlush_)
		enet_host_flush( host );
}

void dataIO_clusterENet_implementation::broadcastPacket( enet_uint8 channelID_, ENetPacket* packet_, bool autoFlush_ )
{
	if(currentRole != dataIO_clusterENet_implementation::SERVER)
		return;

	enet_host_broadcast( host, channelID_, packet_ );
	if(autoFlush_)
		enet_host_flush( host );
}

bool dataIO_clusterENet_implementation::connectTo( const char* remoteAddr_, int connectTimeout_ms_, int clientInfo_,  int channelToAlloc_ )
{
	if(currentRole != dataIO_clusterENet_implementation::CLIENT)
	{
		std::cout << "dataIO_clusterENet_implementation::connectTo() : ERROR: ENet does not work as client - ignoring to connect!" << std::endl;
		return false;
	}

	ENetAddress address;
	enet_address_set_host (& address, remoteAddr_);
    address.port = port;

	
	ENetPeer* tmpPeer = enet_host_connect( host, &address, channelToAlloc_, clientInfo_ );    // host, remote address, number of channels, describing data.
   
    if (tmpPeer == NULL)
    {
		std::cout << "No available peers for initiating an ENet connection." << std::endl;
       return false;
    }
    
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service (host, & event, connectTimeout_ms_) > 0
		&& event.type == ENET_EVENT_TYPE_CONNECT)
    {
		peerList.push_back( tmpPeer );
		std::cout << "Connection to " << remoteAddr_ << ":"<<port<<" succeeded." << std::endl;
		// Note down peers remote IP.
		char *hostIP = new char[20];
		enet_address_get_host_ip( &address, hostIP, 20 );
		tmpPeer->data = hostIP;

		return true;
    }
    else
    {
        /* Either the n seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the n seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset (tmpPeer);

		std::cout << "Connection to " << remoteAddr_ << ":"<<port<<" failed." << std::endl;
		return false;
    }
}

void dataIO_clusterENet_implementation::onReceivePacket(ENetEvent* event_)
{
		receivedTransportContainer.assign((char*)(event_->packet->data), event_->packet->dataLength);
		enet_packet_destroy(event_->packet);   
		//std::cout << "A packet of length "<<receivedTransportContainer.size()<<" containing "<<receivedTransportContainer<<" was received from "<<receivedTransportContainer<<" on channel "<<(int)(event_->channelID)<<std::endl;
}

void dataIO_clusterENet_implementation::onConnect(ENetEvent* event_)
{
	/* Get connect remote IP */
	char *hostIP = new char[20];
	enet_address_get_host_ip(&(event_->peer->address), hostIP, 20);

	/* debug output */
	std::cout << "A new client connected from "<<hostIP<<"." << std::endl; 
		
	/* Store any relevant client information here. */
	event_->peer ->data = hostIP;	

	/* note peer for duplex usage of the connection */
	peerList.push_back(event_->peer);	
}

void dataIO_clusterENet_implementation::onDisconnect(ENetEvent* event_)
{
	// remove peer pionter from peerList
	int foundOn = -1;
	for(unsigned int i=0;i<peerList.size();i++)
	{
		if(peerList[0] == event_->peer)
		{
			peerList.erase(peerList.begin()+i);
			break;
		}
	}


	if(currentRole == dataIO_clusterENet_implementation::SERVER)
	{
		std::cout << "Client " << (char*)event_->peer->data << " disconnected." << std::endl;
	}

	if(currentRole == dataIO_clusterENet_implementation::CLIENT)
	{
		std::cout << "Server "<< (char*)event_->peer->data<<"disconnected." << std::endl;
	}

	// Reset the peer information
	event_->peer->data = NULL;
}
#pragma once
/* -*-c++-*- osgVisual - Copyright (C) 2009-2011 Torben Dannhauer
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


#include <osg/Referenced>
#include <enet/enet.h>
#include <iostream>
#include <string>
#include <vector>

namespace osgVisual
{

/**
 * \brief This class is a object oriented wrapper for the ENet UDP implementation.
 * 
 * This class allows to create a handler for the complete ENet operations.
 * ENet is a cross plattform UDP implementation with some reliability functions.
 * For further details, loot at http://enet.bespin.org/index.html
 * 
 * @author Torben Dannhauer
 * @date  Jul 2010
 */ 
class dataIO_clusterENet_implementation :	public osg::Referenced
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Constructor: Constructs this class and starts the ENet subsystem if this instance is the first instantiated one in the program.
	 * 
	 * @param receivedTransportContainer_ : TransportContainer to interact with. See also receivedTransportContainer
	 */ 
	dataIO_clusterENet_implementation(std::string& receivedTransportContainer_);

	/**
	 * \brief Destructor: Destructs this class. In the last program wide application it shuts the ENet subsystem down.
	 * 
	 */ 
	~dataIO_clusterENet_implementation();

	/**
	 * This enum contains valid roles of this class. In contrast to the dataIO_cluster implementation this class do NOT use MASTER/SLAVE,
	 * because this class implements an non-abstract DUP connection with SERVER and CLIENT. As soon as osgVisual uses only the convenience
	 * functions of clusterENet, the roles are called as usual MASTER/SLAVE.
	 */ 
	enum role {SERVER, CLIENT};

	/**
	 * \brief This function initializes this class and allow to use it's service. Call this function as SERVER as well as CLIENT prior to any usage.
	 * 
	 * @param role_ : Role to act like (SERVER/CLIENT)
	 * @param port_ : Port to use.
	 * @param maxClients_ : Maximum of allowed connected clients. (Affects only the role SERVER)
	 * @param maxChannels_ : Maximum of channels per client. (Affects only the role SERVER)
	 * @param maxInBandwidth_ : Maximal incoming bandwith in bytes per second [bps]. Set 0 to rely on ENets internal throtteling algorithm.
	 * @param maxOutBandwidth_ : Maximal outgoing bandwith in bytes per second [bps]. Set 0 to rely on ENets internal throtteling algorithm.
	 * @return : True on successful init.
	 */ 
	bool init( dataIO_clusterENet_implementation::role role_, unsigned short port_, int maxClients_=10, int maxChannels_=2, int maxInBandwidth_=0, int maxOutBandwidth_=0 );	

	/**
	 * \brief This function connects to a ENet serverat the specified adress. The function affects only the CLIENT role.
	 * 
	 * @param remoteAddr_ : Address of the ENet server to connect to.
	 * @param connectTimeout_ms_ : Connect timeout in milliseconds [ms].
	 * @param clientInfo_ : Client data to transfer to the SERVER during connect. Usually not required.
	 * @param channelToAlloc_ : Max number of non-concurring channels to open.
	 * @return : True on successful connect.
	 */ 
	bool connectTo( const char* remoteAddr_, int connectTimeout_ms_, int clientInfo_=0,  int channelToAlloc_=2 );
	
	/**
	 * \brief This function send a packet to the peer with the specified peer ID (number of the peer in the peer vector). This function works bidirectional from SERVER to CLIENT and vice versa. This function takes ownership of the packet and will destroy it after (un-)successful transmission.
	 * 
	 * @param packet_ : Data packet to broadcast.
	 * @param channelID_ : ID on which channel the broadcast should be done.
	 * @param peerID_ : Peer ID to send the packet to.
	 * @param autoFlush_ : Indicates if the packed should be send immediately after append the packet to the ENets internal send list (like processEvents() with only sending the packet).  This function works bidirectional from SERVER to CLIENT and vice versa.
	 */ 
	void sendPacket( ENetPacket* packet_, enet_uint8 channelID_, unsigned int peerID_=0, bool autoFlush_=false );

	/**
	 * \brief This function send a packet to the peer with the specified peerName. This function works bidirectional from SERVER to CLIENT and vice versa. This function takes ownership of the packet and will destroy it after (un-)successful transmission.
	 * 
	 * @param channelID_ : ID on which channel the broadcast should be done.
	 * @param peerName_ : Peer name to send to data to.
	 * @param packet_ : Data packet to broadcast.
	 * @param autoFlush_ : Indicates if the packed should be send immediately after append the packet to the ENets internal send list (like processEvents() with only sending the packet.).
	 */ 
	void sendPacket( ENetPacket* packet_, enet_uint8 channelID_, std::string peerName_, bool autoFlush_=false );

	/**
	 * \brief : This function emulates a UDP broadcast by sending the packet manually to all connected peers. This function works only as SERVER. Calls as CLIENT are ignored. 
	 * 
	 * @param channelID_ : ID on which channel the broadcast should be done.
	 * @param packet_ : Data packet to broadcast.
	 * @param autoFlush_ : Indicates if the packed should be send immediately after append the packet to the ENets internal send list (like processEvents() with only sending the packet.).
	 */ 
	void broadcastPacket( enet_uint8 channelID_, ENetPacket* packet_, bool autoFlush_=false );

	/**
	 * \brief : Call this function to process all pending events like connects, disconnects, receives or sends.
	 * 
	 * @param timeout_ms_ :  Maximal time to wait for incomming events in milliseconds [ms].
	 */ 
	void processEvents( int timeout_ms_ = 0 );
	
	/**
	 * \brief This function handles the receive of a data packet.
	 * 
	 * @param event_ : Receive event.
	 */ 
	virtual inline void onReceivePacket(ENetEvent* event_);

	/**
	 * \brief This function handles the connect of a peer. This function will be called only by the host waiting for the connection.
	 * 
	 * @param event_ : Connect event.
	 */ 
	virtual inline void onConnect(ENetEvent* event_);
	
	/**
	 * \brief This function handles the disconnect of a peer.
	 * 
	 * @param event_ : Disconnect event.
	 */ 
	virtual inline void onDisconnect(ENetEvent* event_);

protected:
	/**
	 * UDP port to use.
	 */ 
	unsigned short port;

	/**
	 * This enum indicates the current role this class at as.
	 */ 
	dataIO_clusterENet_implementation::role currentRole;

	/**
	 * This variable indicates the number of programm wide used instances of this class. one the first class starts the ENet subsystem and only the 
	 * last one shuts the ENet subsystem down.
	 */
	static int activeENetInstances;

	/**
	 * This flag indicates if the complete ENet wrapper class is initialized. (After init())
	 */ 
	bool enetInitialized;

	/**
	 * ENet adress structure, which contains information regarding the interface and address.
	 */ 
	ENetAddress address; 

	/**
	 * Host structure, which contains the own host role for the conmmunication.
	 */ 
    ENetHost* host;

	/**
	 * Lisst of the ENet remote peers. This list contains only the server during acting as CLIENT (peerList[0]=server), and multiple slave entries during acting as SERVER (peerList[0-n]=client0-n).
	 */ 
	std::vector<ENetPeer*> peerList;

	/**
	 * Eventclass which contains the data of the events dureing connect, resieve or send.
	 */ 
	ENetEvent event;

	/**
	 * Reference to the transportContainer class to store data into on receive and read from during sending.
	 */ 
	std::string& receivedTransportContainer;
};

}	// END NAMESPACE
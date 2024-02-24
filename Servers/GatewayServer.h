//
// Created by Anshul Gowda on 2/8/24.
//

#ifndef ODYSSEYGAMESERVER_GATEWAYSERVER_H
#define ODYSSEYGAMESERVER_GATEWAYSERVER_H

#include <enet/enet.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <map>
#include <vector>
#include <deque>
#include <iostream>
#include "../Player.h"
#include "../Buffers/PacketBuffer.h"
#include "../Components/Gateway.h"
#include "../Components/ExecutorSlave.h"


using namespace std;

/*
 * The Gateway server serves as the gateway for the Odyssey Game Server. It is an independent microservice that will be the initial point of contact for all requests, with a possibility for
 * independent access to the Asset Server. The Gateway Server will consist of an ENetHost to manage networking through an independent networking thread. Packets will be forwarded to a middle
 * buffer where slave threads will pull packets and execute them. These packets once executed will be moved to an output buffer which will flush them back to the clients.
 */

class GatewayServer {
public:
    GatewayServer(string gatewayServerIP, int gatewayServerPort, int maxConnections, int numChannels, int incomingBandwith, int outgoingBandwith);
    ~GatewayServer();

    // run method
    void start();

    // shutdown method
    void shutdown();

    void determineNumExecutorSlaves();

    void addGameServer();

    void removeGameServer();

private:
    PacketBuffer receiveBuffer;
    PacketBuffer outputBuffer;

    // Components:
    Gateway gateway;
    vector<ExecutorSlave*> executorSlaves;

    int numExecSlaves = 10;
    deque<pair<string, ENetPeer *>> gameServerStack; // possible stack architecture to keep the newest servers at the top
    vector<pair<string, string>> activeGameServers; // (IP, Port) to forward requests

    // TODO spawn Executor Slaves;



    // TODO: find a way for the Executor Slaves to access these global functions. One example is that the Gateway Server should
    // know and house which game servers are active since that is global information, but the ExecutorSlaves must know so that
    // they know where to forward requests

    // authenticate client
    void authenticateClient();

    // relevant methods to forward packets to other microservices
    void forwardClientToGameServer();

};
#endif //ODYSSEYGAMESERVER_GATEWAYSERVER_H

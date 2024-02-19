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


using namespace std;

/*
 * The Gateway server serves as the gateway for the Odyssey Game Server. It is an independent microservice that will be the initial point of contact for all requests, with a possibility for
 * independent access to the Asset Server. The Gateway Server will consist of an ENetHost to manage networking through an independent networking thread. Packets will be forwarded to a middle
 * buffer where slave threads will pull packets and execute them. These packets once executed will be moved to an output buffer which will flush them back to the clients.
 */

class GatewayServer {
private:
    ENetHost *server;
    ENetAddress address;
    int port;
    string serverIP;
    thread networkThread;
    mutex networkMutex;
    condition_variable networkCV;
    atomic<bool> running = false;

    map<string, pair<string, ENetPeer *>> clientList;
    deque<pair<string, ENetPeer *>> gameServerStack;
// possible stack architecture to keep the newest servers at the top
    vector<pair<string, string>> activeGameServers; // (IP, Port) to forward requests

    // Components:
    PacketBuffer receiveBuffer;

    // create the middle buffer

    // network loop`
    void networkLoop();

    // authenticate client
    void authenticateClient();

    // relevant methods to forward packets to other microservices
    void forwardClientToGameServer();

public:
    GatewayServer(string gatewayServerIP, int gatewayServerPort, int maxConnections, int numChannels, int incomingBandwith, int outgoingBandwith);

    ~GatewayServer();

    // run method
    void run();;

    // shutdown method
    bool shutdown();
    // connect method



};
#endif //ODYSSEYGAMESERVER_GATEWAYSERVER_H

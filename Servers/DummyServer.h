//
// Created by Anshul Gowda on 2/14/24.
//

#ifndef ODYSSEYGAMESERVER_DUMMYSERVER_H
#define ODYSSEYGAMESERVER_DUMMYSERVER_H

#include <enet/enet.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
using namespace std;


class DummyServer {
private:
    ENetHost *server;
    ENetAddress serverAddress;
    vector<ENetPeer *> clientPeerList;
    string serverIP;
    int serverPort;

    int numConnections;
    int numChannels;
    int incomingBandwith;
    int outgoingBandwith;


public:

    DummyServer(string serverIP, int port, int numConnections = 32, int numChannels = 2, int incomingBandwith = 0, int outgoingBandwith = 0);

    bool connect(string clientAddressToConnect, int clientPortToConnect);

    void disconnect(ENetPeer *peerToDisconnect);

    void sendPacket(ENetPeer *peerToSend, string data);

    void listenForPacket();
};

#endif //ODYSSEYGAMESERVER_DUMMYSERVER_H

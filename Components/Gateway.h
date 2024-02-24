//
// Created by Anshul Gowda on 2/18/24.
//

#ifndef ODYSSEYGAMESERVER_GATEWAY_H
#define ODYSSEYGAMESERVER_GATEWAY_H

#include <string>
#include <map>

#include "../Buffers/PacketBuffer.h"

using namespace std;


class Gateway {
public:
    Gateway(string gatewayIP, int gatewayPort, int maxConnections, int numChannels, int incomingBandwith, int outgoingBandwith, PacketBuffer& buffer);
    ~Gateway();

    void start();

    void shutdown();
//    GatewayServer(string gatewayServerIP, int gatewayServerPort, int maxConnections, int numChannels, int incomingBandwith, int outgoingBandwith);
    ENetHost *getServer() const;

    const ENetAddress &getAddress() const;

    int getPort() const;

    const atomic<bool> &getShutdownFlag() const;

    const map<string, pair<string, ENetPeer *>> &getClientList() const;

private:

    void networkLoop();

    ENetHost* server;
    ENetAddress address;
    int port;

    thread networkThread;
    mutex networkMutex;
    condition_variable networkCV;
    atomic<bool> shutdownFlag;

    PacketBuffer& receiveBuffer;
    map<string, pair<string, ENetPeer *>> clientList;

};


#endif //ODYSSEYGAMESERVER_GATEWAY_H

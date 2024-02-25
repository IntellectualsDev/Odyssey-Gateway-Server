//
// Created by Anshul Gowda on 2/24/24.
//

#ifndef ODYSSEYGAMESERVER_TRANSMITTER_H
#define ODYSSEYGAMESERVER_TRANSMITTER_H

#include <thread>
#include <map>

#include "../Buffers/PacketBuffer.h"


class Transmitter {
public:
    Transmitter(ENetHost* server, ENetAddress address, int port, PacketBuffer& transmitBuffer);

    void start();

    void shutdown();




private:
    // Pass down in GatewayServer constructor
    void transmitLoop();
    void transmitPacket(unique_ptr<Packet> packet);

    ENetHost* server;
    ENetAddress address;
    int port;

    thread transmitThread;
    atomic<bool> shutdownFlag;

    PacketBuffer& transmitBuffer;

};


#endif //ODYSSEYGAMESERVER_TRANSMITTER_H

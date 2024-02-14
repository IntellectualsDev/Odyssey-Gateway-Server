//
// Created by Anshul Gowda on 2/3/24.
//

#ifndef ODYSSEYGAMESERVER_ASSETMANAGER_H
#define ODYSSEYGAMESERVER_ASSETMANAGER_H

#include "enet/enet.h"
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

using namespace std;

class AssetManager {
private:
    queue<ENetPacket*> packetBuffer;
    mutex bufferLock;
    thread listenForNewPackets;
    condition_variable newPacketCondition;
    bool running;

//    bool newpacketFlag;

public:
    AssetManager();
    ~AssetManager();

    void run();
    void shutDown();
    void listenForNewRequests();
    void enquePackets(ENetPacket* packet);
//    ENetPacket* dequePackets();
    void processPacket(ENetPacket *packet);

    const queue<ENetPacket *> &getPacketBuffer() const;
    void setPacketBuffer(const queue<ENetPacket *> &packetBuffer);
    const mutex &getBufferLock() const;
    void setBufferLock(const mutex bufferLock);
};


#endif //ODYSSEYGAMESERVER_ASSETMANAGER_H

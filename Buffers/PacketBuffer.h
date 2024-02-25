//
// Created by Anshul Gowda on 2/5/24.
//

#ifndef ODYSSEYGAMESERVER_PACKETBUFFER_H
#define ODYSSEYGAMESERVER_PACKETBUFFER_H

#include <enet/enet.h>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <string>
#include <mutex>
#include <queue>
#include <memory>

using namespace std;

/*
 * The PacketBuffer acts as an interface between the network "gateway" and the slave threads that will process the packets from the "gateway". Now that we have
 *  seperated the Game Server from the Gateway. The gateway will not receive real-time packets, so there is no need for a priority queue
 *  implementation.
 *
 *  Unique_ptr Use:
 *  In this multithreaded context where Packets are being passed between components, ownership becomes obscure, but it is important for memory
 *  deallocation and performance that ownership is known. Thus Packets are referenced to using unique_ptrs which allows for owner transfership
 *  and unique_ptrs automatically handle memory deallocation and also avoids the case where pass by value would be used to stop memory leaks.

 *
 *  //TODO: Performance Considerations:         Currently the buffer accomplishes thread-safe operations using mutexes and locking mechniams, simplifying it to atomic operations would make this more efficeint but that is notoriously difficult
 *
 *
 */

struct Packet {
    string label;
    ENetPacket *packet;
    chrono::high_resolution_clock::time_point timeAdded;

    bool isSendAddressSet;
    ENetAddress toSendAddress;
    int toSendChannel;

    Packet(string lbl, ENetPacket* pkt)
    : label(std::move(lbl)), packet(pkt), timeAdded(chrono::high_resolution_clock::now()), isSendAddressSet(false)
    {
        toSendAddress.host = ENET_HOST_ANY;
        toSendAddress.port = 0;
        toSendChannel = -1;
    };
};

using SubscriberCallback = function<void (const Packet*)>;

class PacketBuffer {
    public:
        PacketBuffer();
//        ~PacketBuffer();

        // delete the copy constructor and copy assignment operator
        // This is because in our multi-threaded context the Packet Buffer is shared between components and it has unique ownership
        // and responsibility of its contents and copying means that it must share ownership. Since our Packet Buffer's queue is made
        // up of unique_ptrs copying it would lead to compile time errors
        PacketBuffer(const PacketBuffer&) = delete;
        PacketBuffer& operator=(const PacketBuffer&) = delete;

        // add to the buffer, (acquire the lock and add to end)
        void addPacket(unique_ptr<Packet> packet);

        // remove from the buffer, wait until not empty or shutdown & acquire the lock and pop from front
        unique_ptr<Packet> removePacket();

        // wake all waiting threads
        void notifyAll();

        // TODO: get buffer stats, (% full, service rate, how often full, etc)

        // initiate shutdown event
        void shutdown();

    private:
        int numberOfPackets;
        atomic<bool> shutdownFlag;
        mutex bufferMutex;
        condition_variable buffer_Condition;


        queue<unique_ptr<Packet>> packetQueue;
};

#endif //ODYSSEYGAMESERVER_PACKETBUFFER_H

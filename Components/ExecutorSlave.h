//
// Created by Anshul Gowda on 2/18/24.
//

#ifndef ODYSSEYGAMESERVER_EXECUTORSLAVES_H
#define ODYSSEYGAMESERVER_EXECUTORSLAVES_H

#include <memory>
#include <thread>
#include <iostream>

#include "../Buffers/PacketBuffer.h"



class ExecutorSlave {
public:
    // constrcutor
    ExecutorSlave(PacketBuffer& buffer);
    ~ExecutorSlave();

    void start();
    void stop();

    // run

    // shutdown

private:

    atomic<bool> stopFlag;
    PacketBuffer& buffer;
    thread workerThread;

    void run();
    void processPacket(unique_ptr<Packet> packet);
};


#endif //ODYSSEYGAMESERVER_EXECUTORSLAVES_H

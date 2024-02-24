//
// Created by Anshul Gowda on 2/18/24.
//

#include "ExecutorSlave.h"

ExecutorSlave::ExecutorSlave(PacketBuffer &buffer)
: buffer(buffer), stopFlag(false)
{}

ExecutorSlave::~ExecutorSlave() {
    stop();
}

void ExecutorSlave::start() {
    stopFlag.store(false);
    workerThread = thread(&ExecutorSlave::run, this);
}

void ExecutorSlave::stop() {
    stopFlag.store(true);
    buffer.notifyAll();

    if(workerThread.joinable()){
        workerThread.join();
    }
}

void ExecutorSlave::run() {
    while(!stopFlag.load()){
        auto packet = buffer.removePacket();
        if(packet)
            processPacket(std::move(packet)); // transfer ownership of the packet to the ExecutorSlave scope
        }
        else{
            // handle that the packet is a nullptr
            break;
        }
    }
}

void ExecutorSlave::processPacket(unique_ptr<Packet> packet) {
    printf("Executor Slave stole packet of size %zu and data = \"%s\"\n\n", packet->packet->dataLength, packet->packet->data);

    // TODO: determine output actions:
    //  (1) DMG requesting Game Files --> Forward AssetManager MicroService contact information
    //  (2) Player requesting to join game --> Authenticate via Auth MicroService(blocking) & pass newest GameServer info
    //  (3)

}

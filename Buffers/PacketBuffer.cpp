//
// Created by Anshul Gowda on 2/5/24.
//

#include "PacketBuffer.h"

using namespace std;

PacketBuffer::PacketBuffer():
shutdownFlag(false), numberOfPackets(0) {}

void PacketBuffer::addPacket(unique_ptr<Packet> packet) {
    {
        unique_lock<mutex> lock(bufferMutex);

        if(shutdownFlag){
            cout << "Packet Buffer is in shutdown. Packets cannot be queued for service in shutdown.\n" << endl;
            // TODO: deal with packets already in the system. Don't want data loss
            return;
        }
        packetQueue.push(std::move(packet));
        numberOfPackets++;
        printf("Packet received in Receive Buffer.\n\tlength = %zu\n\tdata = %s\n", packetQueue.front()->packet->dataLength, packetQueue.front()->packet->data);
    }

    buffer_Condition.notify_one();
}

/*
 * In removePacket() a lock based on the bufferMutex mutex is acquired, once acquired if the queue is empty the thread enters a wait
 * state based upon the buffer_Condition condition variable and it gives up the lock. This condition variable is passed the lock. Thus when this thread is
 * notified to wake up by cv.notify_one() or cv.notify_all() it will wake up and try to reacquire the lock (handled by the wait funciton)
 * , and once it reacquires the lock it will begin the service the request.
 */
unique_ptr<Packet> PacketBuffer::removePacket() {
    unique_lock<mutex> lock(bufferMutex); // lock the buffer

    // enter wait state and unlock lock until the packetQueue is notified, then check if it satisfies the lambda function if not
    // go back to waiting. This approach prevents random wakeups as even if it is woken up randomly it will not proceed unless it
    // can
    buffer_Condition.wait(lock, [this] {
        return (!packetQueue.empty() || shutdownFlag.load());
    });

    if(packetQueue.empty() && shutdownFlag.load()){
        cout << "Packet Buffer is in shutDown. " << shutdownFlag.load() << endl << "All existing packets have been serviced." << endl;
        return nullptr; //
    }

    auto packet = std::move(packetQueue.front()); // pull out the packet
    packetQueue.pop();
    numberOfPackets--;
    return packet;
}

void PacketBuffer::notifyAll() {
    buffer_Condition.notify_all();
}

void PacketBuffer::shutdown() {
    shutdownFlag.store(true);
    buffer_Condition.notify_all(); // wake all threads waiting of this shutdown command
}


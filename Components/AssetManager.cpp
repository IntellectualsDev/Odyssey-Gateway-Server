//
// Created by Anshul Gowda on 2/3/24.
//

#include "AssetManager.h"

AssetManager::AssetManager(){

}

void AssetManager::run(){
    running = true;
    listenForNewPackets = thread(&AssetManager::listenForNewRequests, this);
}

void AssetManager::shutDown(){
    running = false;

    if(listenForNewPackets.joinable()){
        listenForNewPackets.join();
    }
}

void AssetManager::listenForNewRequests(){
    unique_lock<mutex> lock(bufferLock); // initially lock the buffer
    while(running){
        newPacketCondition.wait(lock, [this]{  // during wait the lock is unlocked do other threads can access
            return !packetBuffer.empty() || !running;
        });

        if(!running){
            break;
        }

        ENetPacket * packet = packetBuffer.front();
        packetBuffer.pop();
        lock.unlock(); // unlock while processing packet dequed

        processPacket(packet);
        lock.lock(); // re-lock to check the condition or wait again
    }
//
//    while(running){
//        if(newpacketFlag){
//            ENetPacket *packet = dequePackets();
//            if(packet != nullptr){
//                processPacket(packet);
//
//                // TODO: any cleanup needed
//            }
//        }
//    }
}


void AssetManager::enquePackets(ENetPacket *packet) {
    lock_guard<mutex> guard(bufferLock);
    packetBuffer.push(packet);
    newPacketCondition.notify_one(); //notify the waiting thread that a packet has been added
//    newpacketFlag = true;
}

//ENetPacket* AssetManager::dequePackets() {
//    lock_guard<mutex> guard(bufferLock);
//    if(!packetBuffer.empty()){
//        ENetPacket *front = packetBuffer.front();
//        packetBuffer.pop();
//        return front;
//    }
//    else{
//        newpacketFlag = false;
//        return nullptr;
//    }
//}

void AssetManager::processPacket(ENetPacket *packet) {
//    TODO: Decode packet unserialize/parse JSON contents
//     - Authenticate request for game files
//     - Access Dedicated Server File System/server storage location for requested game files
//     - package all of them into one packet with reliable flag set
//     - move to OutputManager buffer
}


const queue<ENetPacket *> &AssetManager::getPacketBuffer() const {
    return packetBuffer;
}

void AssetManager::setPacketBuffer(const queue<ENetPacket *> &packetBuffer) {
    AssetManager::packetBuffer = packetBuffer;
}

const mutex &AssetManager::getBufferLock() const {
    return bufferLock;
}

void AssetManager::setBufferLock(const mutex &bufferLock) {
    AssetManager::bufferLock = bufferLock;
}



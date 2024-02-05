//
// Created by Anshul Gowda on 2/5/24.
//

#include "PacketBuffer.h"

PacketBuffer::SubscriptionHandle PacketBuffer::subscribe(const std::string& topic, SubscriberCallback callback) {
    unique_lock<mutex> lock(bufferMutex);
    auto handle = subscriberHandleNext++;
    subscribers[topic][handle] = callback;
    return handle; // this handle can be used to go identify a subscriber's specific callback when they unsub
}

void PacketBuffer::unsubscribe(const std::string &topic, SubscriptionHandle handle) {
    unique_lock<mutex> lock(bufferMutex);
    if(subscribers.find(topic) != subscribers.end()){
        subscribers[topic].erase(handle);
        if(subscribers[topic].empty()){
            subscribers.erase(topic);
        }
    }
}

void PacketBuffer::publish(const std::string &topic, ENetPacket *packet) {
    unique_lock<mutex> lock(bufferMutex);
    messagesQueue.emplace(topic, packet); // emplace adds to end of queue, but no copy/move instead the args are passed to the constructor
    buffer_Condition.notify_one(); // TODO: which thread is this notifying?
}



void PacketBuffer::processMessages() {
    while(true){
        pair<string, ENetPacket*> message;
        {
            unique_lock<mutex> lock(bufferMutex); // lock from others accessing in this scope
            buffer_Condition.wait(lock, [this] { // unlock the scope & wait until the messageQueue is not empty
                return !messagesQueue.empty();
            });

            // once exited wait, pull out from the front of the queue
            message = messagesQueue.front();
            messagesQueue.pop();
        }

        // exit scope of lock to allow other threads to modify the queue
        auto& [topic, packet] = message;
        auto sub_iterator = subscribers.find(topic);

        // notify all those
        if(sub_iterator != subscribers.end()){
            for(auto &[handle, callback]: sub_iterator->second){
                callback(packet);
            }
        }
    }
}
//
// Created by Anshul Gowda on 2/5/24.
//

#ifndef ODYSSEYGAMESERVER_PACKETBUFFER_H
#define ODYSSEYGAMESERVER_PACKETBUFFER_H

#include <enet/enet.h>
#include <thread>;
#include <condition_variable>;
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <queue>

using namespace std;


//struct Message {
//    string message;
//    ENetPacket *packet;
//};

using SubscriberCallback = function<void (const ENetPacket*)>;

class PacketBuffer {
public:
    using SubscriptionHandle = int;
    SubscriptionHandle subscribe(const string& topic, SubscriberCallback callback);
    void unsubscribe(const string& topic, SubscriptionHandle handle);
    void publish(const string& topic, ENetPacket* packet);
    void processMessages();
private:
    mutex bufferMutex;
    condition_variable buffer_Condition;
    map<string, map<SubscriptionHandle , SubscriberCallback>> subscribers;
//    queue<Message> messageQueue;
    queue<pair<string, ENetPacket*>> messagesQueue;
    int subscriberHandleNext = 0;
};


#endif //ODYSSEYGAMESERVER_PACKETBUFFER_H

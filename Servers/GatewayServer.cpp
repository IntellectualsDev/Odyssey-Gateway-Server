//
// Created by Anshul Gowda on 2/8/24.
//

#include "GatewayServer.h"
#include <enet/enet.h>

using namespace std;


// The member objects are instantiated in the order they are declared in the .h file, ref() should be used if hte order of instantiation
// is different. I.E if the gateway is instantiated before the receiveBuffer
GatewayServer::GatewayServer(string gatewayServerIP, int gatewayServerPort, int maxConnections, int numChannels,
                             int incomingBandwith, int outgoingBandwith) :
        gateway(gatewayServerIP, gatewayServerPort, maxConnections, numChannels, incomingBandwith, outgoingBandwith, receiveBuffer),
        receiveBuffer(),
        outputBuffer() {

//    numExecSlaves = numExecutorSlaves();

    for(int i = 0; i < numExecSlaves; i++){
       auto slave = new ExecutorSlave(receiveBuffer);
        executorSlaves.push_back(slave);
    }
}


//TODO: Implement the destructor to make sure the network thread has been killed
GatewayServer::~GatewayServer(){
    receiveBuffer.shutdown();
    outputBuffer.shutdown();
    gateway.shutdown();
    for(auto i:executorSlaves){
        i->stop();
    }
}

// spawns the networkLoop thread function and saves the running state
void GatewayServer::start() {
    gateway.start();
    for(auto i:executorSlaves){
        i->start();
    }
}



void GatewayServer::shutdown() {
    printf("Initializing Gateway Server Shutdown Process\n");
    ENetEvent event;

    // TODO: check if this works as I am using Gateway's getServer to get the ENetHost pointer
    if(!gameServerStack.empty()){
        printf("Disconnecting from Gateway Server's Game Server Connections:\n");
        deque<pair<string, ENetPeer *>>::iterator it;

        for(it = gameServerStack.begin(); it != gameServerStack.end(); it++){
            enet_peer_disconnect(it->second, 0);

            while(enet_host_service (gateway.getServer(), &event, 200) > 0){
                switch(event.type) {
                    case ENET_EVENT_TYPE_RECEIVE:
                        enet_packet_destroy(event.packet);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        printf("\t%x:%u disconnected\n", it->second->address.host,
                               it->second->address.port);
                }
            }
            enet_peer_reset(it->second);
        }
    }
    return ;
}


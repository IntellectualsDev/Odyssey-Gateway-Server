//
// Created by Anshul Gowda on 2/8/24.
//

#include "GatewayServer.h"
#include <enet/enet.h>

GatewayServer::GatewayServer(string serverIP, int serverPort, int maxConnections, int numChannels, int incomingBandwith, int outgoingBandwith)
: receiveBuffer() //instantiate the receiveBuffer member variable (not needed as it would automatically constructed with the GatewayServer
{
    char* serverAddressChar = new char[serverIP.length()+1]; // convert string IP to char * used in enet set host ip
    strcpy(serverAddressChar, serverIP.c_str());
    printf("char array for Gateway Server = %s\n", serverAddressChar);

    enet_address_set_host_ip(&address, serverAddressChar);
    address.port = serverPort;
    port = serverPort;

    if(maxConnections > 4000){
        fprintf(stderr, "ENet only supports < 4000 connections to one ENetHost\n");
        exit(EXIT_FAILURE);
    }

    server = enet_host_create(&address,
                              maxConnections,
                              numChannels,
                              incomingBandwith,
                              outgoingBandwith
    );

    if (server == NULL) {
        fprintf(stderr, "An error occurred while trying to create Gateway Server ENetHost instance\n");
        exit(EXIT_FAILURE);
    }
    printf("Created Gateway Server ENetHost  instance @ %x:%u\n", server->address.host, server->address.port);
}

//TODO: Implement the destructor to make sure the network thread has been killed
GatewayServer::~GatewayServer(){

}

// spawns the networkLoop thread function and saves the running state
void GatewayServer::run() {
    running.store(true);
    printf("In run method");

    if(server == NULL){
        fprintf(stderr, "Gateway Server was not initialized. No ENetHost exists.\n");
        exit(EXIT_FAILURE);
    }

    // create the network loop thread and save it as an attirbute
    networkThread = thread(&GatewayServer::networkLoop, this);
//    networkThread.join(); // the join method will stop the execution of the current thread until the networkThread is completed
}

void GatewayServer::networkLoop() {
    if(server == NULL){
        fprintf(stderr, "Gateway Server was not initialized. No ENetHost exists.\n");
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    printf("Within network loop");

    while (running.load()) {
        cout << "Running: " << noboolalpha << running.load() << endl;
        //        printf("In the network loop");
        while (enet_host_service(server, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    //TODO: Save IP temporarily & Add as ENet peer but flag the peer as NOT authenticated
                    printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                    break;

                case ENET_EVENT_TYPE_RECEIVE: {
                    //TODO: Pass to the middle buffer, if NEW CLIENT slaves will translate IP to MAC ID if authenticated
//                    Packet* packet = new Packet("Some label", event.packet);
                    Packet packet("Some Label", event.packet);
//                    packet.label = "Some Label "; // TODO: give a real packet label
//                    packet.packet = event.packet;

                    receiveBuffer.addPacket(packet);

                    printf("A packet of length %u containing %s was received from %s on channel %u.\n\tIt was added to receive buffer\n",
                           event.packet->dataLength,
                           event.packet->data,
                           event.peer->data,
                           event.channelID);
                    /* Clean up the packet now that we're done using it. */
//                    enet_packet_destroy(event.packet);
                    break;
                }

                    // TODO: Either client was successfully to game server or connection was dropped. No Action Required
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%s disconnected.\n", event.peer->data);
            }
        }
    }
}

bool GatewayServer::shutdown() {
    printf("Initializing Gateway Server Shutdown Process");
    ENetEvent event;
    running.store(false);

    if (!clientList.empty()){
        printf("Disconnecting from Gateway Server's Clients:\n");
        map<string, pair<string, ENetPeer *>>::iterator it;

        for(it = clientList.begin(); it != clientList.end(); it++){
            enet_peer_disconnect(it->second.second, 0);

            while(enet_host_service (server, &event, 200) > 0){
                    switch(event.type) {
                        case ENET_EVENT_TYPE_RECEIVE:
                            enet_packet_destroy(event.packet);
                            break;
                        case ENET_EVENT_TYPE_DISCONNECT:
                            printf("\t%x:%u disconnected\n", it->second.second->address.host,
                                   it->second.second->address.port);
                    }
            }

            // if a disconnect event is not received, manually kill the connection
            enet_peer_reset(it->second.second);
        }
    }

    if(!gameServerStack.empty()){
        printf("Disconnecting from Gateway Server's Game Server Connections:\n");
        deque<pair<string, ENetPeer *>>::iterator it;

        for(it = gameServerStack.begin(); it != gameServerStack.end(); it++){
            enet_peer_disconnect(it->second, 0);

            while(enet_host_service (server, &event, 200) > 0){
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

    // Notify waiting threads to wake up and complete any work, for the future. Can be removed in current implementation.
    {
        unique_lock<mutex> lock(networkMutex);
        networkCV.notify_all();
    }

    networkThread.join(); // the main thread will not wait for the network thread to wait before executing
    delete this;
    return true;
}


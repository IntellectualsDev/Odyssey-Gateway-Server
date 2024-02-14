//
// Created by Anshul Gowda on 2/14/24.
//

#include "DummyServer.h"

DummyServer::DummyServer(string serverIP, int port, int numConnections, int numChannels, int incomingBandwith, int outgoingBandwith){
    char* serverAddressChar = new char[serverIP.length()+1]; // convert string IP to char * used in enet set host ip
    strcpy(serverAddressChar, serverIP.c_str());
    printf("char array, serverAddressHost = %s\n", serverAddressChar);

    enet_address_set_host_ip(&serverAddress, serverAddressChar);
//    serverAddress.host = ENET_HOST_ANY;
    serverAddress.port = port;
    serverPort = port;



    server = enet_host_create(&serverAddress,
                              numConnections,
                              numChannels,
                              incomingBandwith,
                              outgoingBandwith
                              );

    if (server == NULL){
        fprintf(stderr, "An error occurred while trying to create server instance of ENetHost\n");
        exit(EXIT_FAILURE);
    }
    printf("Created ENetHost Server instance @ %x:%u\n", server->address.host, server->address.port);
}

bool DummyServer::connect(string clientAddressToConnect, int clientPortToConnect){
    ENetAddress clientAddress;
    ENetEvent event;
    ENetPeer *client;

    char* clientAddressChar = new char[clientAddressToConnect.length()+1];
    strcpy(clientAddressChar, clientAddressToConnect.c_str());

    enet_address_set_host_ip(&clientAddress, clientAddressChar);
    clientAddress.port = clientPortToConnect;

    client = enet_host_connect(server, &clientAddress, 2, 0);

    if(client == NULL){
        fprintf(stderr, "No available peers to initiate an ENet connection. Max client limit reached\n");
        exit(EXIT_FAILURE);
    }

    if(enet_host_service(server, &event, 15000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){
        fprintf(stderr, "Successfully connected to %x:%u.\n", clientAddress.host, clientAddress.port);
        clientPeerList.push_back(client);
        return true;
    }
    else{ // successfully connected to the client specified
        enet_peer_reset(client);
        fprintf(stderr, "Server failed to connect to %x:%u.\n", clientAddress.host, clientAddress.port);
        return false;
    }
}

void DummyServer::disconnect(ENetPeer *peerToDisconnect){
    ENetEvent event;
    enet_peer_disconnect(peerToDisconnect, 0);

    while(enet_host_service(server, &event, 3000) > 0){
        switch(event.type){
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Successfully disconnected from %x:%u\n", peerToDisconnect->address.host, peerToDisconnect->address.port);
                return;
        }
    }

    // if we arrive here then the connection must be forced to close;
    enet_peer_reset(peerToDisconnect);
};

void DummyServer::sendPacket(ENetPeer *peerToSend, string data){
    char* dataChar = new char[data.length()+1]; // convert string IP to char * used in enet set host ip
    strcpy(dataChar, data.c_str());

    ENetPacket *packet = enet_packet_create(dataChar, strlen(dataChar)+1, ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(peerToSend, 0, packet);
    enet_host_flush(server);
}

void DummyServer::listenForPacket(){
    ENetEvent event;

    while(enet_host_service(server, &event, 0) > 0){

        switch(event.type){
            case ENET_EVENT_TYPE_CONNECT:
                printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                        event.packet -> dataLength,
                        event.packet -> data,
                        event.peer -> data,
                        event.channelID);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf ("%s disconnected.\n", event.peer -> data);
        }
    }
}
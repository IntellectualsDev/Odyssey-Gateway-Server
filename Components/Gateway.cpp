//
// Created by Anshul Gowda on 2/18/24.
//

#include "Gateway.h"

Gateway::Gateway(std::string gatewayIP, int gatewayPort, int maxConnections, int numChannels, int incomingBandwith,
                 int outgoingBandwith, PacketBuffer &buffer) : receiveBuffer(buffer) {
    char* serverAddressChar = new char[gatewayIP.length()+1]; // convert string IP to char * used in enet set host ip
    strcpy(serverAddressChar, gatewayIP.c_str());
    printf("char array for Gateway Server = %s\n", serverAddressChar);

    enet_address_set_host_ip(&address, serverAddressChar);
    address.port = gatewayPort;
    port = gatewayPort;

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

Gateway::~Gateway() {
    if(shutdownFlag.load()){
        shutdown();
    }
}

void Gateway::start() {
    shutdownFlag.store(false);
    networkThread = thread(&Gateway::networkLoop, this); // create the networkLoop thread and allow it to execute
}

void Gateway::networkLoop() {
    if(server == NULL){
        fprintf(stderr, "Gateway was not initialized. No ENetHost exists.\n");
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    printf("Network Loop Initialized\n");

    while (!shutdownFlag.load()) {
//        cout << "Gateway Running: " << noboolalpha << shutdownFlag.load() << endl;
        //        printf("In the network loop");
        while (enet_host_service(server, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    //TODO: Save IP temporarily & Add as ENet peer but flag the peer as NOT authenticated
                    printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                    break;

                case ENET_EVENT_TYPE_RECEIVE: {
                    //TODO: Pass to the middle buffer, if NEW CLIENT slaves will translate IP to MAC ID if authenticated

                    unique_ptr<Packet> packet = make_unique<Packet>("Some Label", event.packet);
                    receiveBuffer.addPacket(std::move(packet));

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

void Gateway::shutdown() {
//    printf("Initializing Gateway Server Shutdown Process\n");
    ENetEvent event;
    shutdownFlag.store(false);

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

    // Notify waiting threads to wake up and complete any work, for the future. Can be removed in current implementation.
    {
        unique_lock<mutex> lock(networkMutex);
        networkCV.notify_all();
    }

    networkThread.join(); // the main thread will not wait for the network thread to wait before executing
//    delete this;
    return;
}

ENetHost* Gateway::getServer() const {
    return server;
}

const ENetAddress& Gateway::getAddress() const {
    return address;
}

int Gateway::getPort() const {
    return port;
}

const atomic<bool> &Gateway::getShutdownFlag() const {
    return shutdownFlag;
}

const map<string, pair<string, ENetPeer *>> &Gateway::getClientList() const {
    return clientList;
}

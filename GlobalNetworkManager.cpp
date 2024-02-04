//
// Created by Anshul Gowda on 1/31/24.
//

#include "GlobalNetworkManager.h"



GlobalNetworkManager::GlobalNetworkManager(int port, int clients, int channels) {
    initialize(port, clients, channels);
}

GlobalNetworkManager::~GlobalNetworkManager() {

}

bool GlobalNetworkManager::initialize(int port, int clients, int channels) {
    if (enet_initialize()) {
        fprintf(stderr, "Error occured in GlobalNetworkManager initialization");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    this->address.host = ENET_HOST_ANY;

//    TODO: determine if the port is being used
    this->address.port = port;
    setServer(enet_host_create(&address, clients, 2, 0, 0));

    if(getServer() == NULL){
        fprintf(stderr, "Error occured creating ENet Server Host");
        return EXIT_FAILURE;
    }
    else{
        return true;
    }
}

void GlobalNetworkManager::run() {
    running = true;
    if(server == NULL){
        throw runtime_error("Error Global Network Manager not initialized. Run initialize()");
    }

    // Create the network thread and save it
    networkThread = thread(&GlobalNetworkManager::networkLoop, this);

}

void GlobalNetworkManager::networkLoop() {
    ENetEvent networkEvent; // holder for network event received in the service function
    while(running && enet_host_service(server, &networkEvent, 1000) > 0){
        switch(networkEvent.type){
            case ENET_EVENT_TYPE_CONNECT:
                printf("New Client connected from %x:%u.\n",
                       networkEvent.peer->address.port,
                       networkEvent.peer->address.host);

                // MAC addresses cannot be sent with connection
                // give temporary ID for the client. replace with MAC Address from intro packet (historically first packet)

            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet received:\nlength: %u\ndata: %s\npeer:%s\nchannel: %u\n",
                       networkEvent.packet->dataLength,
                       networkEvent.packet->data,
                       networkEvent.peer->data,
                       networkEvent.channelID);
                handleIncomingPacket(networkEvent.packet);

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected ", networkEvent.peer->address.host);
                networkEvent.peer->data = NULL;
        }


        // extract packet information and take the correct steps
    }
}

void GlobalNetworkManager::handleIncomingPacket(ENetPacket *packet) {
    enet_uint8 *data = packet->data; //stored data
    // if serialized unserialize, or parse the sent JSON
    // TODO: Create Unserialize/JSON parses


    // TODO: parse packet and determine if:
    //  (1) new client --> authenticate client + store MAC Address (ID)
    //  (2) existing client -> distribute to lobby
    //  (3) client requesting game assets --> authenticate
//    if(new client){
//        authenticateClient();
//    }
//    if(existing client){
//        distributePacketToGameLobby();
//    }
//    if(client requesting game assets){
//        distributePacketToAssetManager();
//    }

}

void GlobalNetworkManager::authenticateClient() {

}

void GlobalNetworkManager::distributePacketToAssetManager(ENetPacket *packet) {

}

void GlobalNetworkManager::distributePacketToGameLobby(ENetPacket *packet, int lobbyID) {

}

void GlobalNetworkManager::shutDown() {
    running = false;

    // execute any work leftover in the Network Thread
    // the thread will be automatically killed after execution, and it will be destructed automatically
    if(networkThread.joinable()){
        networkThread.join();
    }
    if(server != nullptr){
        enet_host_destroy(server);
        server = nullptr;
    }

}

ENetHost * GlobalNetworkManager::getServer() {
    return server;
}

void GlobalNetworkManager::setServer(ENetHost *server) {
    setServer(server);
}

ENetAddress GlobalNetworkManager::getAddress() {
    return address;
}

void GlobalNetworkManager::setAddress(ENetAddress address) {
    setAddress(address);
}


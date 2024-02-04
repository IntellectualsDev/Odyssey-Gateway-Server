//
// Created by Anshul Gowda on 1/31/24.
//

#ifndef ODYSSEYGAMESERVER_GLOBALNETWORKMANAGER_H
#define ODYSSEYGAMESERVER_GLOBALNETWORKMANAGER_H

#include <enet/enet.h>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include "Player.h"


using namespace std;


class GlobalNetworkManager {
private:
    ENetHost *server;
    ENetAddress address;
    bool running = false;
    thread networkThread;
    int currentPlayerCount; // housekeeping for the number of players
//    int tempPlayerIDCounter = 0;
    int lobbyIDCounter = 0;

    //TODO: Consider alternatives to keep track of clients and lobbies
    map<string, Player> clientList; // map client MAC address to their player object
//    map<int, Lobby>
    map<int, vector<Player>> lobbyList; // map lobbyID generated to a list of players (replace with Lobby object)

    void networkLoop();
    void handleIncomingPacket(ENetPacket *packet);
    void authenticateClient();
    void distributePacketToAssetManager(ENetPacket * packet);
    void distributePacketToGameLobby(ENetPacket *packet, int lobbyID);

public:
    GlobalNetworkManager(int port, int clients, int channels);
    ~GlobalNetworkManager();

    bool initialize(int port, int clients, int channels);
    void run();
    void shutDown();
    ENetHost * getServer();
    void setServer(ENetHost *server);
    ENetAddress getAddress();
    void setAddress(ENetAddress address);
};


#endif //ODYSSEYGAMESERVER_GLOBALNETWORKMANAGER_H

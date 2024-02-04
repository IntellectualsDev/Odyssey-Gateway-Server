//
// Created by Anshul Gowda on 2/3/24.
//

#ifndef ODYSSEYGAMESERVER_PLAYER_H
#define ODYSSEYGAMESERVER_PLAYER_H

#include <string>
using namespace std;


class Player {
private:
    string MACaddress;
    string IPaddress;
    int port;
    int clientID;
    int gameLobby;

public:

    const string &getMaCaddress() const;
    void setMaCaddress(const string &maCaddress);
    const string &getIPaddress() const;
    void setIPaddress(const string &iPaddress);
    int getPort() const;
    void setPort(int port);
    int getClientId() const;
    void setClientId(int clientId);
    int getGameLobby() const;
    void setGameLobby(int gameLobby);
};


#endif //ODYSSEYGAMESERVER_PLAYER_H

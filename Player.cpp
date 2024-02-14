//
// Created by Anshul Gowda on 2/3/24.
//

#include "Player.h"
using namespace std;

const string &Player::getMaCaddress() const {
    return MACaddress;
}

void Player::setMaCaddress(const string &maCaddress) {
    MACaddress = maCaddress;
}

const string &Player::getIPaddress() const {
    return IPaddress;
}

void Player::setIPaddress(const string &iPaddress) {
    IPaddress = iPaddress;
}

int Player::getPort() const {
    return port;
}

void Player::setPort(int port) {
    this->port = port;
}

int Player::getClientId() const {
    return clientID;
}

void Player::setClientId(int clientId) {
    clientID = clientId;
}

int Player::getGameLobby() const {
    return gameLobby;
}

void Player::setGameLobby(int gameLobby) {
    Player::gameLobby = gameLobby;
}

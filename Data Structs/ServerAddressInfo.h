//
// Created by Anshul Gowda on 2/24/24.
//

#ifndef ODYSSEYGAMESERVER_SERVERADDRESSINFO_H
#define ODYSSEYGAMESERVER_SERVERADDRESSINFO_H

#include "../Templates/ThreadSafeData.h"
#include <unordered_map>
#include <string>

using namespace std;

class ServerAddressInfo: public ThreadSafeData{
public:
    ServerAddressInfo() = default;
    virtual ~ServerAddressInfo() = default;

    void updateAddress(const string& serviceName, const string& address, int port);
    pair<string, int> getAddress(const string& serviceName) const;

private:
    unordered_map<string, pair<string, int>> serverInfo;

};


#endif //ODYSSEYGAMESERVER_SERVERADDRESSINFO_H

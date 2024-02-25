//
// Created by Anshul Gowda on 2/24/24.
//

#include "ServerAddressInfo.h"

void ServerAddressInfo::updateAddress(const string &serviceName, const string &address, int port) {
    // Using the [&] in the parameter list simply means that all operators within the scope are now pass by reference. This means
    // that the writeAction method will modify the underlying data struct since it is pass by ref.

    writeAction([&] {
        serverInfo[serviceName] = make_pair(address, port);
    });
}

pair<string, int> ServerAddressInfo::getAddress(const std::string &serviceName) const {
    return readAction([&] () -> pair<string, int> {
        auto it = serverInfo.find(serviceName);
        if(it != serverInfo.end()) {
            return it->second;
        }
        return make_pair("", -100);
    });
}

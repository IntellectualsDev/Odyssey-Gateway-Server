#include <iostream>
using namespace std;
#include <enet/enet.h>

ENetAddress address;
ENetHost * server;
ENetEvent event;





int main() {
    if (enet_initialize() != 0){
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    else{
        fprintf(stdout, "ENet was properly initialized.\n");
    }
    atexit(enet_deinitialize);

    return 0;
}

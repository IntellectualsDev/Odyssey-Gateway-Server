//#include <iostream>
#include <enet/enet.h>
#include <thread>

#include "Servers/GatewayServer.h"

using namespace std;

int main() {
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);

    // only allows max 4000 connections

    auto gateway = new GatewayServer("127.0.0.1", 5450, 3500, 20, 0, 0);

    this_thread::sleep_for(chrono::seconds(2));

    gateway->start();
    this_thread::sleep_for(chrono::seconds(15));

    gateway->shutdown();
    cout << "Shutdown Complete. Exiting :)" << endl;
}

#include <iostream>
#include <enet/enet.h>
#include "Servers/DummyServer.h"
#include <thread>

using namespace std;

int main() {
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);

    DummyServer server = DummyServer("127.0.0.1", 5450);
    while(true){
        server.listenForPacket();
    }

//    DummyClient client = DummyClient();
//    this_thread::sleep_for(chrono::seconds(2));
//    client.connect("127.0.0.1", 5450);

    return 0;
}

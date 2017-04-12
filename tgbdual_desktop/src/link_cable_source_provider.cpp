//
// Created by Christopher Sawczuk on 06/04/2017.
//

#include <getopt.h>
#include <network/tcp_server.h>
#include <network/multicast_transmitter.h>
#include <iostream>
#include <network/tcp_client.h>
#include <network/null_link_source.h>
#include <link_cable_source_provider.h>

link_cable_source *provideLinkCableSource(int *argc, char ***argv) {
    int option = 0;
    link_cable_source *selected_cable_source = nullptr;
    while ((option = getopt(*argc, *argv, "smc:")) != -1) {
        switch (option) {
            case 's': {
                selected_cable_source = create_tcp_server_cable();
                break;
            }
            case 'm': {
                std::cout << "Broadcasting availability as client" << std::endl;
                multicast_transmitter mc_transmitter{"239.0.10.0", 1337};
                mc_transmitter.transcieve([&](std::string addr) {
                    std::cout << "Should connect to " << addr << std::endl;
                    selected_cable_source = create_client_cable(addr);
                });
                break;
            }
            case 'c': {
                selected_cable_source = create_client_cable(optarg);
                break;
            }
            default:
            case '?': {
                if (optopt == 'c') {
                    std::cerr << "Target address must be passed in with -c" << std::endl;
                } else {
                    std::cerr << "Unknown option " << optopt << std::endl;
                }
                break;
            }
        }
    }

    (*argc) -= optind;
    (*argv) += optind;

    if (selected_cable_source == nullptr) {
        selected_cable_source = new null_link_source();
    }
    return selected_cable_source;
}
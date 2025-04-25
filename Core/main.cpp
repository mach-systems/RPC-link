/*
 * Example RPC server to be used with Vanetza vehicular networking library. Skeleton is to be used as
 * Autotalks device link layer wrapper.
 *
 * Implementation note: PromiseFulfillerPair does not work between different threads.
 *
 * See v2x_rx_thread_entry() in autotalks.cpp for contents of simulated V2X RX traffic. Change define
 * RX_TRAFFIC_TYPE for changing between example data.
 *
 * Module led.cpp contains example implementation of a timer, that could be used e.g. for controlling device LEDs.
 *
 * Author: MACH SYSTEMS s.r.o.
 * Created: 2025-03-04
 */


#include <capnp/rpc-twoparty.h>
#include <csignal>
#include <iostream>
#include <kj/async-unix.h>
#include "autotalks.hpp"
#include "led.hpp"
#include "linkLayer.hpp"
#include "relay.hpp"
#include "rpcServer.hpp"


const char* const help =
"\nUsage: ./autotalks-link [parameters]\n\
e.g. ./rpc-link eth=eth0\n\n\
eth=X\t\t-\tInterface name (e.g. eth0) for IP address (ip not needed)\n\
ip=Y\t\t-\tIP address\n\
port=Z\t\t-\tIP port\n\
tx-dump\t\t-\tDump all TX packets (GeoNetworking)\n\
rx-dump\t\t-\tDump all RX packets (GeoNetworking)\n\
phys-print\t-\tDump networking layer RX/TX data\n\n";


int main(int argc, char** argv)
{
    bool print = false;

    RelayConfig relayConfig = {.IpAddress = "localhost", .Port = PortDefault, .EthName = ""};

    for (int i = 1; i < argc; i++)
    {
        std::string param(argv[i]);

        if ((param.length() >= 3 && strncmp(param.c_str(), "ip=", 3) == 0))
        {
            relayConfig.IpAddress = param.erase(0, 3);
        }
        else if ((param.length() >= 5 && strncmp(param.c_str(), "port=", 5) == 0))
        {
            relayConfig.Port = param.erase(0, 5);
        }
        else if ( (param.length() >= 6 && strncmp(param.c_str(), "--help", 6) == 0)
                   || (param.length() >= 2 && strncmp(param.c_str(), "-h", 2) == 0)
                   || (param.length() >= 3 && strncmp(param.c_str(), "--h", 3) == 0)
                   || (param.length() >= 5 && strncmp(param.c_str(), "-help", 5) == 0)
                   || (param.length() >= 4 && strncmp(param.c_str(), "help", 4) == 0)
                   || (param.length() >= 1 && strncmp(param.c_str(), "?", 1) == 0) )
        {
            fprintf(stdout, help);
            return 1;
        }
        else if ((param.length() >= 5 && strncmp(param.c_str(), "print", 5) == 0))
        {
            print = true;
        }
        else if ((param.length() >= 4 && strncmp(param.c_str(), "eth=", 4) == 0))
        {
            relayConfig.EthName = param.erase(0, 4);
        }
        else
        {
            fprintf(stderr, "Unknown parameter: %s\nExiting.\n", param.c_str());
            return 2;
        }
    }

    kj::AsyncIoContext asyncio = kj::setupAsyncIo();
    auto& waitScope = asyncio.waitScope;

    Led::LedControl led(asyncio.lowLevelProvider->getTimer());
    // Evaluate the first time right away
    auto ledTimerPromise = led.ScheduleTimer().eagerlyEvaluate([](kj::Exception&& exception) {
            std::cerr << "LED Timer failed: " << exception.getDescription().cStr() << std::endl;
        }
    );

    if (v2x_device_init() != EXIT_SUCCESS)
    {
        std::cerr << "Error initializing the device. Program will exit.\n" << std::endl;
        return EXIT_FAILURE;
    }

    std::unique_ptr<AutotalksLink> linkLayer;
    linkLayer.reset(new AutotalksLink(print, led));

    Relay relay(*linkLayer, print);
    std::string address = parseEndpoint(relayConfig);

    auto& ioprovider = *asyncio.provider;
    auto& network = ioprovider.getNetwork();
    kj::Own<kj::NetworkAddress> addr;
    try {
        addr = network.parseAddress(address).wait(waitScope);
    } catch (std::exception& e) {
        std::cout << "Network endpoint parse error " << e.what() << std::endl;
        return 3;
    }
    auto listener = addr->listen();
    capnp::TwoPartyServer server(kj::heap<LinkLayerImpl>(relay));
    auto serverPromise = server.listen(*listener);
    std::cout << "[SERVER] Listening on endpoint " << address << std::endl;

    // Catch SIGINT and SIGTERM signals to end properly
    kj::UnixEventPort::captureSignal(SIGINT);
    kj::UnixEventPort::captureSignal(SIGTERM);

    // Lambda that handles either signal
    auto shutdownHandler = [&waitScope](siginfo_t info) mutable {
        KJ_DBG("Termination requested", info.si_signo);
        SetThreadEnd();                 // Prepare reception thread for ending
        waitScope.cancelAllDetached();  // Cancel detached promises
        waitScope.poll();               // Run the possible enqueued synchronous executions
        autotalks_device_deinit();
    };
    auto shutdownPromise = asyncio.unixEventPort.onSignal(SIGINT).then(shutdownHandler)
        .exclusiveJoin(asyncio.unixEventPort.onSignal(SIGTERM).then(shutdownHandler));
    shutdownPromise.exclusiveJoin(kj::mv(serverPromise))
                   .exclusiveJoin(kj::mv(ledTimerPromise)).wait(waitScope);

    std::cout << "Shutting down server..." << std::endl;

    return EXIT_SUCCESS;
}

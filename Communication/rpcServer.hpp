/*
 * rpcServer.hpp
 *
 *  Created on: Mar 20, 2025
 *      Author: karel
 */

#ifndef RPCSERVER_HPP_
#define RPCSERVER_HPP_

#include <atomic>
#include <kj/debug.h>
#include <kj/async-io.h>
#include <kj/async.h>
#include "vanetza.capnp.h"

#include "relay.hpp"
#include "defs.hpp"

#define SERVER_ID       0x55AA55AA55AA55AAULL
#define SERVER_VERSION  0x00000001UL



class LinkLayerImpl final : public vanetza::rpc::LinkLayer::Server
{
    public:
        LinkLayerImpl(Relay& relay);

        virtual ~LinkLayerImpl() { }

        /**
         * Request a V2X frame transmission - called by the RPC client.
         * @param  context Transmission data context
         * @retval Fulfilled promise
         */
        kj::Promise<void> transmitData(TransmitDataContext context);

        /**
         * Identify the server.
         * @param  context Identify context
         * @retval Fulfilled promise
         */
        kj::Promise<void> identify(IdentifyContext context);

        /**
         * Set V2X Source Address.
         * @param  Request context
         * @retval Fulfilled promise
         */
        kj::Promise<void> setSourceAddress(SetSourceAddressContext context) override;

        /**
         * Callback when client subscribes to V2X reception.
         * @param  context Subscription context
         * @retval Fulfilled promise
         */
        kj::Promise<void> subscribeData(SubscribeDataContext context) override;

        /**
         * Callback when client subscribes to CBR measurement.
         * @param  context Subscription context
         * @retval Fulfilled promise
         */
        kj::Promise<void> subscribeCbr(SubscribeCbrContext context) override;

        /**
         * Request sending of V2X frame to net (on reception).
         * @param  v2xFrame Frame to transmit
         * @retval None
         */
        void request(const V2xFrame& v2xFrame);

        /**
         * Request sending of DCC measurement (CBR in this case) to net.
         * @param  cbr Value to transmit
         * @retval None
         */
        void requestDcc(const ChannelBusyRatio& cbr);

    private:
        /**
         * Store the data listener client
         */
        capnp::Capability::Client dataListenerClient;

        /**
         * Store the CBR listener client
         */
        capnp::Capability::Client m_CbrListenerClient;

        /**
         * Connection to Autotalks link layer for data reception
         */
        Relay& m_Relay;

        /**
         * Set to true when any client is connected
         */
        std::atomic<bool> clientConnected_;

        /**
         * Set to true when any client is connected to CBR measurement
         */
        std::atomic<bool> m_CbrClientConnected;

        /**
         * Executor to be accessible from another threads
         */
        const kj::Executor& m_Executor;
};



#endif /* RPCSERVER_HPP_ */

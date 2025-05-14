/*
 * rpcServer.cpp
 *
 *  Created on: Mar 20, 2025
 *      Author: karel
 */

#include <cstdio>
#include <iostream>

#include "autotalks.hpp"    // SetAddress()
#include "linkLayerImpl.hpp"


LinkLayerImpl::LinkLayerImpl(Relay& relay)
                             : dataListenerClient(nullptr),
                               m_CbrListenerClient(nullptr),
                               m_Relay(relay),
                               clientConnected_(false),
                               m_CbrClientConnected(false),
                               m_Executor(kj::getCurrentThreadExecutor())
{
    relay.SetNetIndication(std::bind(&LinkLayerImpl::request, this, std::placeholders::_1));
    relay.SetNetDccIndication(std::bind(&LinkLayerImpl::requestDcc, this, std::placeholders::_1));
}

kj::Promise<void> LinkLayerImpl::transmitData(TransmitDataContext context)
{
    V2xFrame v2xFrame;

    auto request = context.getParams();     // Get request struct
    auto frame = request.getFrame();        // Extract Frame structure
    auto result = context.initResults();    // Prepare response

    auto source = frame.getSourceAddress();
    auto destination = frame.getDestinationAddress();
    auto payload = frame.getPayload();
    auto params = request.getTxParams();

    printf("Transmitted frame:\n");
    printf("  Source Address: ");
    for (auto byte : source) {
        printf("%02X ", byte);
        v2xFrame.SrcAddress.push_back(byte);
    }
    printf("\n  Destination Address: ");
    for (auto byte : destination) {
        printf("%02X ", byte);
        v2xFrame.DstAddress.push_back(byte);
    }
    printf("\n  Payload: ");
    for (auto byte : payload) {
        printf("%02X", byte);
        v2xFrame.Data.push_back(byte);
    }

    (void) printf("\n  Parameters:  ");
    result.setError(vanetza::rpc::LinkLayer::ErrorCode::OK);
    result.setMessage("Transmission successful");
    if (vanetza::rpc::LinkLayer::TxParameters::WLAN == params.which())
    {
        v2xFrame.WlanParameters.Datarate = params.getWlan().getDatarate();
        v2xFrame.WlanParameters.Power = params.getWlan().getPower();
        v2xFrame.WlanParameters.Priority = params.getWlan().getPriority();
        (void) printf("datarate: %u, power: %d, priority: %d", v2xFrame.WlanParameters.Datarate,
                                                               v2xFrame.WlanParameters.Power,
                                                               v2xFrame.WlanParameters.Priority);
    }
    else if (vanetza::rpc::LinkLayer::TxParameters::CV2X == params.which())
    {
        v2xFrame.Cv2xParameters.Power = params.getCv2x().getPower();
        v2xFrame.Cv2xParameters.Priority = params.getCv2x().getPriority();
        (void) printf("power: %d, priority: %d", v2xFrame.Cv2xParameters.Power,
                                                 v2xFrame.Cv2xParameters.Priority);
    }
    else
    {
        (void) printf("invalid");
        result.setError(vanetza::rpc::LinkLayer::ErrorCode::INVALID_ARGUMENT);
        result.setMessage("Frame parameter mismatch.");
    }
    printf("\n");

    m_Relay.PacketSend(v2xFrame);
    return kj::READY_NOW;
}

kj::Promise<void> LinkLayerImpl::identify(IdentifyContext context)
{
    auto response = context.initResults();
    response.setId(SERVER_ID);
    response.setVersion(SERVER_VERSION);
    response.setInfo("Lorem ipsum");
    return kj::READY_NOW;
}

kj::Promise<void> LinkLayerImpl::setSourceAddress(SetSourceAddressContext context)
{
    std::vector<uint8_t> addressVector;
    auto address = context.getParams().getAddress();
    printf("Setting source address: ");
    for (auto byte : address) {
        printf("%02X ", byte);
        addressVector.push_back(byte);
    }
    printf("\n");

    if (SetAddress(addressVector))
    {
        context.getResults().setError(vanetza::rpc::LinkLayer::ErrorCode::OK);
    }
    else
    {
        context.getResults().setError(vanetza::rpc::LinkLayer::ErrorCode::INTERNAL_ERROR);
    }
    return kj::READY_NOW;
}

kj::Promise<void> LinkLayerImpl::subscribeData(SubscribeDataContext context)
{
    if (!clientConnected_)
    {
        dataListenerClient = context.getParams().getListener();     // Store the client's listener
        std::cout << "[SERVER] Client was connected." << std::endl;
        clientConnected_ = true;
    }
    return kj::READY_NOW;
}

kj::Promise<void> LinkLayerImpl::subscribeCbr(SubscribeCbrContext context)
{
    if (!m_CbrClientConnected)
    {
        m_CbrListenerClient = context.getParams().getListener();
        std::cout << "[SERVER] CBR callback subscribed" << std::endl;
        m_CbrClientConnected = true;
    }
    return kj::READY_NOW;
}

void LinkLayerImpl::request(const V2xFrame& v2xFrame)
{
    if (clientConnected_)
    {
        m_Executor.executeSync([v2xFrame, this]() {
            auto listener = dataListenerClient.castAs<vanetza::rpc::LinkLayer::DataListener>();
            auto request = listener.onDataIndicationRequest();
            auto frame = request.initFrame();

            frame.setSourceAddress(kj::heapArray<const capnp::byte>(v2xFrame.SrcAddress.data(), v2xFrame.SrcAddress.size()));
            frame.setDestinationAddress(kj::heapArray<const capnp::byte>(v2xFrame.DstAddress.data(), v2xFrame.DstAddress.size()));
            frame.setPayload(kj::heapArray<const capnp::byte>(v2xFrame.Data.data(), v2xFrame.Data.size()));

            auto rxParams = request.initRxParams();
            auto wlanParams = rxParams.initWlan();
            wlanParams.setPriority(v2xFrame.WlanParameters.Priority);
            wlanParams.setPower(v2xFrame.WlanParameters.Power);
            wlanParams.setDatarate(v2xFrame.WlanParameters.Datarate);
            // TODO implement C_V2X mode
            auto timestamp = rxParams.initTimestamp();
            if (false)
            {
                timestamp.setHardware(v2xFrame.Timestamp);	// Here the hardware timestamp would be set
            }
            else
            {
                timestamp.setSoftware(v2xFrame.Timestamp);
            }

            request.send().then([this](capnp::Response<vanetza::rpc::LinkLayer::DataListener::OnDataIndicationResults> results) mutable {
                // Data sent successfully
                // printf("Sent Data Indication.\n");
            })
            .catch_([this](kj::Exception&& e) -> kj::Promise<void> {
                if (e.getType() == kj::Exception::Type::DISCONNECTED) {
                    KJ_DBG("[SERVER] Client was disconnected.");
                    clientConnected_ = false;
                } else {
                    KJ_LOG(ERROR, "request.send() exception", e);
                }
                return kj::READY_NOW;
            }).eagerlyEvaluate([](kj::Exception&& exception) {
              KJ_LOG(ERROR, exception);
            }).detach([this](kj::Exception&& e) {
                KJ_LOG(ERROR, "Frame sending exception", e);
            });
            // wait() cannot be called - "wait() is not allowed from within event callbacks"
        });
    }
}

void LinkLayerImpl::requestDcc(const ChannelBusyRatio& cbr)
{
    if (m_CbrClientConnected)
    {
        m_Executor.executeSync([cbr, this]() {
            auto listener = m_CbrListenerClient.castAs<vanetza::rpc::LinkLayer::CbrListener>();
            auto request = listener.onCbrReportRequest();
            auto frame = request.initCbr();

            frame.setBusy(cbr.Busy);
            frame.setSamples(cbr.Samples);

            request.send().then([this](capnp::Response<vanetza::rpc::LinkLayer::CbrListener::OnCbrReportResults> results) mutable {
                // CBR sent successfully
                //printf("CBR sent successfully.\n");
            })
            .catch_([this](kj::Exception&& e) -> kj::Promise<void> {
                if (e.getType() == kj::Exception::Type::DISCONNECTED) {
                    KJ_DBG("[SERVER] Client was disconnected.");
                    m_CbrClientConnected = false;
                } else {
                    KJ_LOG(ERROR, "request.send() exception", e);
                }
                return kj::READY_NOW;
            }).eagerlyEvaluate([](kj::Exception&& exception) {
              KJ_LOG(ERROR, exception);
            }).detach([this](kj::Exception&& e) {
                KJ_LOG(ERROR, "CBR sending exception", e);
            });
            // wait() cannot be called - "wait() is not allowed from within event callbacks"
        });
    }
}

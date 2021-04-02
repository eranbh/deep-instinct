#pragma once
#include <string>
#include "message_types.h"


class ProducerTask
{
    public:

        ProducerTask(std::string socketName);

        // main entry point
        void run();

    private:

        void startProducing();

        void handleConnect();

        void sendMsgToConsumer(ConsumerDataMsg);

        ProducerAckMsg getConsumerAck();

        std::string m_socketName;
        int m_listfd{};
};
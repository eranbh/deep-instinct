#pragma once
#include <iostream>
#include "task.h"
#include "message_types.h"


class ConsumerTask
{
    public:

        ConsumerTask(std::string socketName);

        void run();

    private:

        int handleEvents();

        void handleNewConnection(int connfd);
       
        ConsumerDataMsg readConsumerMsg(int connfd);

        void sendAck(int connfd, ConsumerDataMsg msg);

        std::string m_socketName;

        int m_listfd;
};
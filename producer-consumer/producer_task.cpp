#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include "producer_task.h"
#include "task.h"
#include "serialization_utils.h"

ProducerTask::ProducerTask(std::string socketName) :
    m_socketName{std::move(socketName)}
{
    m_listfd = createNewListeningSocket();
}

void  ProducerTask::run()
{
    std::cout << "--- STARTING PRODUCER ---" << std::endl;
    startProducing();
}

void  ProducerTask::startProducing()
{
    int msgIndex{1};
    while(1)
    {
        handleConnect();

        // take a message from a db / outer space
        ConsumerDataMsg msg {.type=MsgTypes::DATA , .id=msgIndex};

        sendMsgToConsumer(std::move(msg));

        ProducerAckMsg ack = getConsumerAck();

        std::cout << "producer: got ack for: " << ack.lastMsgId << std::endl;

        msgIndex++;
    }   
}

void ProducerTask::handleConnect()
{
    struct sockaddr saddr = {AF_UNIX, ""};
    memcpy(saddr.sa_data, m_socketName.c_str(), m_socketName.size());
    socklen_t saddrlen = sizeof(struct sockaddr);

    while(1)
    {
        if (auto conn = connect(m_listfd, &saddr, saddrlen) ; conn == 0 || errno == EISCONN)
        {
            break;
        }
        else
        {
            ::close(m_listfd);
            m_listfd = createNewListeningSocket();
            std::cout << "consumer down. reconnecting ... " << errno << std::endl;
            sleep(1);
        }
    }
}

void ProducerTask::sendMsgToConsumer(ConsumerDataMsg msg)
{
    writeToSocket(m_listfd, std::move(msg));
}

ProducerAckMsg ProducerTask::getConsumerAck()
{
    ProducerAckMsg ack = readFromSocket<ProducerAckMsg>(m_listfd);
    return ack;
}


int main(int aaa, char* bbb[])
{
    Task<ProducerTask> consumerTask{SOCK_NAME};
    consumerTask.start();
    return 0;
}
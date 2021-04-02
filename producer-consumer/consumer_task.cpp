#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "task.h"
#include "consumer_task.h"
#include "serialization_utils.h"

void ConsumerTask::run()
{
    std::cout << "--- STARTING CONSUMER ---" << std::endl;
    handleEvents();
}

ConsumerTask::ConsumerTask(std::string socketName) :
  m_socketName{std::move(socketName)}
{
    // setting the re-use addr opt doesnt work on
    // unix sockets for some reason. so unlink
    // a socket if one exists. this will not be
    // an issue if we use annonimus sockets
    unlink(SOCK_NAME.c_str());

    m_listfd = createNewListeningSocket();

    struct sockaddr saddr = {AF_UNIX, ""};
    memcpy(saddr.sa_data, m_socketName.c_str(), m_socketName.size());
    socklen_t saddrlen = sizeof(struct sockaddr);
    
    if(bind(m_listfd, &saddr, saddrlen) == -1)
    {
        std::cout << "consumer: bind failed  " << errno << std::endl;
        exit(-1);
    }

    if(listen(m_listfd, 10) == -1)
    {
        std::cout << "consumer: listen failed. " << errno << std::endl;
        exit(-1);
    }
}

int ConsumerTask::handleEvents()
{

    // if we want to support multiple connections
    // there should be a reactor here: e.g. epoll(3)   
    while(1){
        auto connfd = accept(m_listfd, NULL, NULL);

        std::cout << "consumer: got new connection request on fd " << connfd << std::endl;

        
        // this could be off loaded to another thread
        // so that this thread accepts new connections
        handleNewConnection(connfd);
    }
}

void ConsumerTask::
handleNewConnection(int connfd)
{
    while(1)
    {
        ConsumerDataMsg msg = readConsumerMsg(connfd);

        if(msg.id == 0)
        {
            std::cout << "consumer: producer went down. waiting for new connections" << std::endl;
            return;
        }
        
        // handle msg --> put it in a queue for another thread? process it?

        sendAck(connfd, std::move(msg));
    }
    
}

ConsumerDataMsg ConsumerTask::readConsumerMsg(int connfd)
{

    ConsumerDataMsg msg = readFromSocket<ConsumerDataMsg>(connfd);
    return msg;                             
}

void ConsumerTask::sendAck(int connfd, ConsumerDataMsg msg)
{
    ProducerAckMsg ack{.lastMsgId = msg.id};
    writeToSocket(connfd, ack);
}

int main(int aaa, char* bbb[])
{
    Task<ConsumerTask> consumerTask{SOCK_NAME};
    consumerTask.start();
    return 0;
}
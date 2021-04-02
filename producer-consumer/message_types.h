#pragma once
#include <array>

enum class MsgTypes : int {DATA, CONTROL, ERROR, STOP};

// data sent to the consumer
// super over simplified
struct ConsumerDataMsg
{
    MsgTypes type;
    int id{0};
    std::array<char, 1024> data2{};
};

// ack for the producer
struct ProducerAckMsg
{
    int lastMsgId{};
};

inline static const std::string SOCK_NAME{"server-socket"};
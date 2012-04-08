#ifndef MESSAGES_H
#define MESSAGES_H

enum MessageType {
    MESSAGE_INIT,
    MESSAGE_DATA,
    MESSAGE_COMPLETED,
    MESSAGE_LOGD_START_COMPLETE,
    MESSAGE_LOGD_SHUTDOWN,
    MESSAGE_LOGD_SHUTDOWN_COMPLETED
};

struct MessageHeader {
    MessageType m_type;
    MessageHeader( MessageType in_type )
        :m_type( in_type ) {
    }
};

template<MessageType _MessageType>
struct SimpleMessage {
    MessageHeader m_header;
    SimpleMessage()
        :m_header( _MessageType ) {
    }
};

typedef SimpleMessage<MESSAGE_INIT> MessageInit;
typedef SimpleMessage<MESSAGE_COMPLETED> MessageCompleted;
typedef SimpleMessage<MESSAGE_LOGD_START_COMPLETE> MessageLogStartComplete;
typedef SimpleMessage<MESSAGE_LOGD_SHUTDOWN> MessageLogdShutdown;
typedef SimpleMessage<MESSAGE_LOGD_SHUTDOWN_COMPLETED> MessageLogdShutdownCompleted;

struct MessageData {
    MessageHeader m_header;
    char msg[BUFSIZ];
    MessageData()
        :m_header( MessageType::MESSAGE_DATA ) {
    }
};

#endif //MESSAGES_H

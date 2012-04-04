#ifndef MESSAGES_H
#define MESSAGES_H

enum MessageType {
    MESSAGE_INIT,
    MESSAGE_DATA,
    MESSAGE_COMPLETED
};

struct MessageHeader {
    MessageType m_type;
    MessageHeader( MessageType in_type )
        :m_type( in_type ) {
    }
};

struct MessageInit {
    MessageHeader m_header;
    MessageInit()
       :m_header( MessageType::MESSAGE_INIT ) {
    }
};

struct MessageData {
    MessageHeader m_header;
    char msg[BUFSIZ];
    MessageData()
        :m_header( MessageType::MESSAGE_DATA ) {
    }
};

struct MessageCompleted {
    MessageHeader m_header;
    MessageCompleted()
       :m_header( MessageType::MESSAGE_COMPLETED ) {
    }
};

#endif //MESSAGES_H

//
// Created by Cederic on 4/02/2022.
//

#ifndef COUPLE_LAMP_ZMQ_LIB_H
#define COUPLE_LAMP_ZMQ_LIB_H

#include <ESP8266WiFi.h>

#define zGreetingSigVerSize 10
#define zGreetingEndSize 53
#define zReadySUBSize 27
#define zReadyPUSHSize 28
#define zSubStartSize 3
#define zSendHeaderSize 2
#define MAXSENDSTRING 96

class Zmq_lib {
public:
    enum types{
        none,
        push,
        sub,
    };
    Zmq_lib() : set(false), port(0), type(none) {};
    void settings(String &hosttmp, int porttmp, types typetmp) { host = hosttmp; port = porttmp; type = typetmp; set = true;}
    bool connect();
    bool subscribe(const char *toppic);
    bool sendMessage(const char *message);
    bool readMessage(String &returnString);

private:

    bool MakeHeader(uint16_t size, char *header, bool flag);

    bool set;
    String host;
    int port;
    types type;
    WiFiClient client;
    const unsigned char zGreetingSigVer[zGreetingSigVerSize] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7F};
    const unsigned char zGreetingEnd[zGreetingEndSize] = {0x01, 'N', 'U', 'L', 'L', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const unsigned char zReadyPUSH[zReadyPUSHSize] = {0x04,0x1A,0x05,'R','E','A','D','Y',0x0B,'S','o','c','k','e','t','-','T','y','p','e',0x00,0x00,0x00,0x04,'P','U','S','H'};
    const unsigned char zReadySUB[zReadySUBSize] = {0x04,0x19,0x05,'R','E','A','D','Y',0x0B,'S','o','c','k','e','t','-','T','y','p','e',0x00,0x00,0x00,0x03,'S','U','B'};
    const unsigned char zSubStart[zSubStartSize] = {0x00, 0x00, 0x01};
    String NormalReceived = "GPS>Tracker?>000>" ;
};


#endif //COUPLE_LAMP_ZMQ_LIB_H

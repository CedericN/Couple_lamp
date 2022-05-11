//
// Created by Cederic on 4/02/2022.
//

#include "Zmq_lib.h"

bool Zmq_lib::connect() {
    if(!set){
        Serial.println("class not correctly set");
        return false;
    }

    bool exit = false;
    u8 counter = 0;
    u8 countUntil = 0;


    Serial.print("connecting to broker: ");
    Serial.print(host);
    Serial.print(":");
    Serial.println(port);

    if (!client.connect(host, port)) {
        Serial.println("connection with broker failed");
        return false;
    }
    Serial.println("connection succeeded start zmq headers");

    client.write(zGreetingSigVer, zGreetingSigVerSize);

    exit = false;
    counter = 0;
    countUntil = 11;
    while(!exit){
        if (client.available()){
            u8 tmpChar = client.read();
            Serial.write(tmpChar);
            counter ++;
        }
        if(counter == countUntil){
            exit = true;
        }
    }
    Serial.println();

    Serial.println("send greeting end");
    client.write(3);
    client.write(zGreetingEnd, zGreetingEndSize);

    exit = false;
    counter = 0;
    countUntil = 53;

    while(!exit){
        if (client.available()){
            u8 tmpChar = client.read();
            Serial.write(tmpChar);
            counter ++;
        }
        if(counter == countUntil){
            exit = true;
        }
    }
    Serial.println();

    counter = 0;

    switch (type) {
        case push:
            client.write(zReadyPUSH, zReadyPUSHSize);
            countUntil = 28;
            break;
        case sub:
            client.write(zReadySUB, zReadySUBSize);
            countUntil = 0;
            break;
        case none:
            break;
    }

    exit = false;
    while(!exit){
        if (client.available()){
            u8 tmpChar = client.read();
            Serial.write(tmpChar);
            counter ++;
        }
        if(counter == countUntil){
            exit = true;
        }
    }
    Serial.println();

    Serial.println("done sending headers");

    return true;
}

bool Zmq_lib::subscribe(const char *toppic) {
    if(type != sub){return false;}

    Serial.print("subscribing to topic: ");
    Serial.println(toppic);

    u16 stringLength = strlen(toppic);
    char sendString[MAXSENDSTRING + zSubStartSize];

    memset(sendString, 0, MAXSENDSTRING + zSubStartSize);
    memcpy(sendString,zSubStart,zSubStartSize);

    if(!MakeHeader(stringLength, sendString, false)){return false;};
    memcpy(sendString + zSubStartSize, toppic, stringLength);

    Serial.println(sendString[1]);
    client.write(zReadySUB, zReadySUBSize);
    client.write(sendString, stringLength + zSubStartSize);

//    for(unsigned int i = 0; i < (stringLength + zSubStartSize); i++){
//        Serial.print((int)sendString[i]);
//        Serial.print(" ");
//    }

    Serial.println("sub send");

    bool exit = false;
    u8 counter = 0;
    u8 countUntil = 24;
    while(!exit){
        if (client.available()){
            u8 tmpChar = client.read();
            Serial.write(tmpChar);
            counter ++;
        }
        if(counter == countUntil){
            exit = true;
        }
    }
    return true;
}

bool Zmq_lib::sendMessage(const char *message) {
    if(type != push){return false;}
    u16 stringLength = strlen(message);
    char sendString[MAXSENDSTRING + zSendHeaderSize];
    memset(sendString, 0, MAXSENDSTRING + zSendHeaderSize);
    if(!MakeHeader(stringLength, sendString, true)){return false;};
    memcpy(sendString + zSendHeaderSize, message, stringLength);
    client.write(sendString, stringLength + zSendHeaderSize);
    return true;
}

bool Zmq_lib::readMessage(String &returnString) {
    if(type != sub){
        return "";
    }

    u8 counter = 0;

    while(client.available()){
        char tmp = client.read();
        if(tmp > 31){
            Serial.print(tmp);
            returnString[counter] = tmp;
            counter ++;
        }
    }
    Serial.println();
    return true;
}

bool Zmq_lib::MakeHeader(uint16_t size, char *header, bool flag) {
    if(header != nullptr)
    {
        flag ? ((header[0] = 0),(header[1] = 0)) : ((header[1] = 0)); // flag = true : subscribe, flag = false: normal send
        int divisor = flag ?  32768 : 128;
        unsigned char mask = 0b10000000;
        unsigned char big = flag;
        while (divisor != 0)
        {
            if(size / divisor == 1)
            {
                size = size - divisor;
                big ? (header[0] = header[0] | mask) : (header[1] = header[1] | mask);

            }
            mask = mask >> 1;
            if (mask == 0 && big != 0)
            {
                big = 0;
                mask = 0b10000000;
            }
            divisor = divisor / 2;
        }
    }
    else{return false;}
    return true;
}
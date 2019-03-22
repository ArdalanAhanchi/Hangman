#include <string>
#include <regex>
#include <iostream>
#include <vector>

#include "constants.h"

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

#ifndef PACKET_H
#define PACKET_H

#define PACKET_DELIMITER '*'
#define PACKET_END -127
#define PACKET_BEGIN 127

using namespace std;

class Packet
{
    public:
        //Default Constructors.
        Packet();
        Packet(const Packet &other);

        //Constructor with opcode (Args can be added later).
        Packet(char opCode);

        //Constructor with a serialized char array (For arrived packets.)
        Packet(vector<char> &serialized);

        //Create a string For sending over network.
        void serialize(vector<char> &toSerialize);

        //Setter.
        void addArg(string arg);

        //Getters.
        char getOpCode();
        string getArg(int index);
        int getNumArgs();

    private:
        vector<string> _args;
        char _opCode;
};

#endif

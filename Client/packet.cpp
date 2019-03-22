#include "packet.h"

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

//Default constrctors.
Packet::Packet()
{
    _opCode = S_ERROR;
}

Packet::Packet(const Packet &other)
{
    this->_opCode = other._opCode;
    this->_args = other._args;
}

//Constructor with opcode (Args can be added later).
Packet::Packet(char opCode) { _opCode = opCode; }

//Constructor with a serialized string (For arrived packets.)
Packet::Packet(vector<char> &serialized)
{
    int currIndex = 0;

    while(serialized[currIndex] != PACKET_BEGIN && currIndex < serialized.size())
        currIndex++;

    _opCode = serialized[++currIndex];

    int currArg = -1;
    for(int i = currIndex + 1; i < serialized.size(); i++)
    {
        if(serialized[i] == PACKET_END)                                         //If we've reached the end of the file.
            break;

        if(serialized[i] == PACKET_DELIMITER)                                   //If we've reached a delimiter, skip it and go to next argument.
        {
            currArg++;
            _args.push_back("");
        }
        else
        {
            if(currArg == -1)                                                   //If its the first one, add an empty string to the array.
            {
                _args.push_back("");
                currArg++;
            }

            _args[currArg] += serialized[i];
        }
    }
}

//Create a string For sending over network.
void Packet::serialize(vector<char> &toSerialize)
{
    toSerialize.push_back(PACKET_BEGIN);
    cerr << "Serializing Pakcet, Opcode is: " << _opCode << endl;
    toSerialize.push_back(_opCode);
    for(size_t i = 0; i < _args.size(); i++)
    {
        for (int j = 0; j < _args[i].size(); j++)                               //Put all the characters in packet.
            toSerialize.push_back(_args[i][j]);

        toSerialize.push_back(PACKET_DELIMITER);                                //Add delimiter.
    }
    if(_args.size() != 0)
        toSerialize.pop_back();                                                 //Remove last delimiter.

    toSerialize.push_back(PACKET_END);                                          //Add the EOF.
}

//Setter.
void Packet::addArg(string arg)
{
    if (arg.find(PACKET_DELIMITER) != std::string::npos)                        //Check if the new argument has the delimiter in it.
        cerr << "Packet: Error, argument contains delimiter." << endl;
    else
        _args.push_back(arg);
}

//Getters.
char Packet::getOpCode() { return _opCode;}

string Packet::getArg(int index)
{
    if(index < 0 && index > _args.size())                                      //Check for invalid indexes.
    {
        cerr << "Packet: Error, Invalid index was given." << endl;
        return string();
    }
    else
        return _args[index];
}

int Packet::getNumArgs() { return _args.size(); }

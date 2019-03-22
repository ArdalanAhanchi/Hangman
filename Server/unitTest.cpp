#include <string>
#include <iostream>
#include <vector>

#include "packet.h"
#include "user.h"

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

using namespace std;

int main()
{
	Packet p('A');
	p.addArg("USERNAME:PASSCODE");
	p.addArg("ANOTHERONE:ANOTHER");


	//Test packet serializing.
	vector<char> serialized;
	p.serialize(serialized);

	cerr << "Printing serialized packet." << endl;
	for(int i = 0; i < serialized.size(); i++)
		cerr << serialized[i];

	cerr << endl;

	//Test deserializing.
	Packet p2(serialized);

	cerr << "Printing deserialized packet." << endl;
	cerr << "Op is: " << p2.getOpCode() << endl;
	for(int i = 0; i < p2.getNumArgs(); i++)
		cerr << p2.getArg(i) << endl;

	cerr << endl;

	//Test user serializing.
	User u1("TestUser", "TestPass");
	string serializedUser = u1.serialize();
	cerr << "Printing Serialized User." << endl;
	cerr << serializedUser << endl;

	User u2(serializedUser);	//Deserialize.
	cerr << "Printing deserialized user." << endl;
	cerr << u2.serialize() << endl;

	return 0;
}

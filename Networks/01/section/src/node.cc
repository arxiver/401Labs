//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "node.h"

Define_Module(Node);
int i;
void Node::initialize()
{
    i = 0;
    if (strcmp(getName(),"tic")==0){
        cMessage * msg = new cMessage("hello");
        send(msg,"out");
    }
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    if (strcmp(getName(),"toc") == 0 ){
        std::string str = "hi";
        if (i != 0){
            str += std::to_string(i);
        }
        cMessage * hiMsg = new cMessage(str.c_str());
        send(hiMsg,"out");
        i++;
    }
    else {
        std::string str = "hello" + std::to_string(i);
        cMessage * helloMsg = new cMessage(str.c_str());
        send(helloMsg,"out");
    }
    EV<<"Resending message"<<msg<<endl;
}

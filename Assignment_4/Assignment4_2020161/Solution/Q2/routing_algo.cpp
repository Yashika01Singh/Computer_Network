#include "node.h"
#include <iostream>

using namespace std;

void printRT(vector<RoutingNode*> nd){
/*Print routing table entries*/
	for (int i = 0; i < nd.size(); i++) {
	  nd[i]->printTable();
	}
}



void routingAlgo(vector<RoutingNode*> nd){
 
  bool saturation=false;
  for(int i=0 ; i<nd.size() ; i++){
    for(int j=0; j<nd.size() ; j++){
      if(i==j)
          continue;
      
      RoutingNode* sendnode = nd[i];
      RoutingNode* recvnode = nd[j];
      sendnode->sendMsg(recvnode);

    }
  }
  
 
  /*Print routing table entries after routing algo converges */
  printf("Printing the routing tables after the convergence \n");
  printRT(nd);
}

void RoutingNode::recvMsg(RouteMsg *msg) {
  //your code here
 
  // Traverse the routing table in the message.
  // Check if entries present in the message table is closer than already present 
  // entries.
  // Update entries.
  recieved_msg++;
  string sender = msg->from;
  LinkTable inter = msg->link;
  for(auto c : inter.tbl)
  memory.push_back(c);
 
 
  
  

  if(this->recieved_msg == this->no_of_routers -1){
    Dijkstra();
    //cout<<"YAAA";
  }
    
 
}




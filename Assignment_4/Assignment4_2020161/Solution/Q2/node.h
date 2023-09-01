#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <climits> 
using namespace std;

/*
  Each row in the table will have these fields
  dstip:	Destination IP address
  nexthop: 	Next hop on the path to reach dstip
  ip_interface: nexthop is reachable via this interface (a node can have multiple interfaces)
  cost: 	cost of reaching dstip (number of hops)
*/
class RoutingEntry{
 public:
  string dstip, nexthop;
  string ip_interface;
  int cost;
};

/*
 * Class for specifying the sort order of Routing Table Entries
 * while printing the routing tables
 * 
*/
class Comparator{
 public:
  bool operator()(const RoutingEntry &R1,const RoutingEntry &R2){
    if (R1.cost == R2.cost) {
      return R1.dstip.compare(R2.dstip)<0;
    }
    else if(R1.cost > R2.cost) {
      return false;
    }
    else {
      return true;
    }
  }
} ;

/*
  This is the routing table
*/
struct routingtbl {
  vector<RoutingEntry> tbl;
};

/*
  Message format to be sent by a sender
  from: 		Sender's ip
  mytbl: 		Senders routing table
  recvip:		Receiver's ip
*/


/*
  Emulation of network interface. Since we do not have a wire class, 
  we are showing the connection by the pair of IP's
  
  ip: 		Own ip
  connectedTo: 	An address to which above mentioned ip is connected via ethernet.
*/
class NetInterface {
 private:
  string ip;
  string connectedTo; 	//this node is connected to this ip
  int cost;
 public:
  string getip() {
    return this->ip;
  }
  int getcost(){
    return this->cost;
  }
  string getConnectedIp() {
    return this->connectedTo;
  }
  void setcost(int cost){
     this->cost=cost;
  }
  void setip(string ip) {
    this->ip = ip;
  }
  void setConnectedip(string ip) {
    this->connectedTo = ip;
  }
  
};

struct Link{
  NetInterface interface;
  int cost;
  string destNode;
  string sourceNode;
};
struct LinkTable{
  vector<Link> tbl;
  void push_back(Link c){
    this->tbl.push_back(c);
  }
};
/*
  Struct of each node
  name: 	It is just a label for a node
  interfaces: 	List of network interfaces a node have
  Node* is part of each interface, it easily allows to send message to another node
  mytbl: 		Node's routing table
*/
class Node {
 private:
  string name;
  
  

 protected:
  
  vector<pair<NetInterface, Node*> > interfaces;
  struct routingtbl mytbl;
  LinkTable mineLink;
  LinkTable memory;
  
  bool isMyInterface(string eth) {
    for (int i = 0; i < interfaces.size(); ++i) {
      if(interfaces[i].first.getip() == eth)
	return true;
    }
    return false;
  }
 public:
  int no_of_routers;
  void setRouters(int n){
    this->no_of_routers = n;
  }
  void setName(string name){
    this->name = name;
  }
  
  void addInterface(string ip, string connip, Node *nextHop,int cost) {
    //ip = mine , connip = neigh , nextHop = neigh
    
    NetInterface eth;
    eth.setip(ip);
    eth.setConnectedip(connip);
    eth.setcost(cost);
    interfaces.push_back({eth, nextHop});
    Link l;
    l.cost=cost;
    l.destNode=nextHop->getName();
    l.sourceNode=getName();
    l.interface = eth;
    mineLink.push_back(l);

    
  }
  
  void addTblEntry(string myip, int cost) {
    RoutingEntry entry;
    entry.dstip = myip;
    entry.nexthop = myip;
    entry.ip_interface = myip;
    entry.cost = cost;
    mytbl.tbl.push_back(entry);
  }

  void updateTblEntry(string dstip, int cost) {
    // to update the dstip hop count in the routing table (if dstip already exists)
    // new hop count will be equal to the cost 
    for (int i=0; i<mytbl.tbl.size(); i++){
      RoutingEntry entry = mytbl.tbl[i];

      if (entry.dstip == dstip) 
        mytbl.tbl[i].cost = cost;
      
    }

    // remove interfaces 
    for(int i=0; i<interfaces.size(); ++i){
      // if the interface ip is matching with dstip then remove
      // the interface from the list
      if (interfaces[i].first.getConnectedIp() == dstip) {
        interfaces.erase(interfaces.begin() + i);
      }
    }
  }
  
  string getName() {
    return this->name;
  }
  
  struct routingtbl getTable() {
    return mytbl;
  }
  
  void printTable() {
    Comparator myobject;
    sort(mytbl.tbl.begin(),mytbl.tbl.end(),myobject);
    cout<<this->getName()<<":"<<endl;
    for (int i = 0; i < mytbl.tbl.size(); ++i) {
      cout<<mytbl.tbl[i].dstip<<" | "<<mytbl.tbl[i].nexthop<<" | "<<mytbl.tbl[i].ip_interface<<" | "<<mytbl.tbl[i].cost <<endl;
    }
  }
 
  
  
};
class RouteMsg {
 public:
  //string from; // I am sending this message, so it must be me i.e. if A is sending mesg to B then it is A's ip address
  //struct Interfaec *mytbl;	// This is routing table of A
  string from;	
  LinkTable link;
   
  // B ip address that will receive this message
};
class RoutingNode: public Node {
 private:
  
  int recieved_msg = 0;
  public:
  
  void addentry(RoutingEntry entry){
    bool entryExists = false;
    for ( int i=0; i<mytbl.tbl.size(); ++i) {
      RoutingEntry myEntry = mytbl.tbl[i];
      //printf("i=%d, nodeRT.cost=%d, DV.cost=%d\n",i, myEntry.cost, entry.cost );
      if (myEntry.dstip==entry.dstip){
        entryExists=true;
        //update existing entry
        if (myEntry.cost>entry.cost) {
         
          mytbl.tbl[i]=entry;
        }
      }
    }
    if (!entryExists) {
      // add the new entry
      mytbl.tbl.push_back(entry);
    }
  }
int minDistance(int dist[], bool sptSet[])
{
 
    // Initialize min value
    int min = INT_MAX;
    int min_index;
 
    for (int v = 0; v < no_of_routers; v++)
        if (sptSet[v] == false && dist[v] <= min){
            min = dist[v];
            min_index = v;
 
        }
            
    return min_index;
}
int get_node_number(string A){
  for(auto &elem : A){
    return elem-'A';
  }
  return -1;
 
}




int find_min(LinkTable myips){
  int  answer=0;
  int Minimum = INT_MAX;
  int i=0;
  for(Link c : myips.tbl){
    if(c.cost<Minimum){
      answer=i;
      Minimum=c.cost;
    }
    i++;
  }
  return answer;
}
  void Dijkstra(){
  
    //we know whichip is connected to which ip
    //we know which ip is connected to this node
    //apply dijkstra
    //updating all
    /*
    cout<<"HERE ";
    cout<<getName();
    cout<<"\n";*/
    
    
    LinkTable myips;
    for(auto c : mineLink.tbl){
      myips.tbl.push_back(c);
    }
    
    bool arr[no_of_routers];
    for(int i=0; i<no_of_routers; i++)
      arr[i]=false;
    arr[get_node_number(getName())] = true;
    
    while(myips.tbl.size()>0){
      
      
      int min = find_min(myips);
      Link minimum = myips.tbl.at(min);
      //myips.tbl.erase(min);
      string name = minimum.destNode;
      
      arr[get_node_number(name)]=true;
      
      if(minimum.sourceNode==getName()){
          RoutingEntry entry;
          entry.dstip = minimum.interface.getConnectedIp();
          entry.nexthop = minimum.interface.getConnectedIp();
          entry.ip_interface = minimum.interface.getip();
          entry.cost = minimum.interface.getcost();
          addentry(entry);
          
      }
      for(auto c : memory.tbl){

        if(c.sourceNode==name){
          //c.interface.setip(minimum.interface.getip());
          RoutingEntry entry;
          entry.dstip = c.interface.getip();
          entry.nexthop = minimum.interface.getConnectedIp();
          entry.ip_interface = minimum.interface.getip();
          entry.cost = minimum.cost;
          addentry(entry);
          c.interface.setConnectedip(minimum.interface.getConnectedIp());
          c.interface.setip( minimum.interface.getip());
          c.cost=minimum.cost+c.interface.getcost();
          if(!arr[get_node_number(c.destNode)])

          myips.push_back(c);
          
        }

      }
      
      myips.tbl.erase(myips.tbl.begin()+min);

      
      

    }
    
    
  }
    
    
    
    

  
  
  Link getsmallest(LinkTable links){

    int cost = 1000000;
    Link min ;
    for( int i=0; i<links.tbl.size() ; i++){
      if(links.tbl[i].cost < cost){
          min = links.tbl[i];
          cost = links.tbl[i].cost;
      }
        
    }
    return min;
    
  }
  
  int getcostfromip(string ip)
  {
    for ( int i=0; i<mytbl.tbl.size(); ++i) {
      RoutingEntry myEntry = mytbl.tbl[i];
      if (myEntry.dstip==ip){
          return myEntry.cost;
      }
    }
    return -1;
  }
  
 
 
 
  void recvMsg(RouteMsg *msg);
  
  void sendMsg(RoutingNode *sendnode){
    
    /*
    struct routingtbl ntbl;
    for (int i = 0; i < mytbl.tbl.size(); ++i) {
      ntbl.tbl.push_back(mytbl.tbl[i]);
    }
    
    for (int i = 0; i < interfaces.size(); ++i) {
      RouteMsg msg;
      msg.from = interfaces[i].first.getip();
      //printf("i=%d, msg-from-interface=%s\n",i, msg.from.c_str());
      msg.neigh = &inter;
      msg.recvip = interfaces[i].first.getConnectedIp();		
      interfaces[i].second->recvMsg(&msg);
    }
    */
    LinkTable copy;
    
    for(auto c : mineLink.tbl){
      copy.push_back(c);
    }
    RouteMsg msg;
    msg.from = getName();
    //printf("i=%d, msg-from-interface=%s\n",i, msg.from.c_str());
    msg.link=copy;	
    sendnode->recvMsg(&msg);
    
    
      
  }

  
  
};

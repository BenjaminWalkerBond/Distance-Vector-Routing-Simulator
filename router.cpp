#pragma once
#include "TableEntry.h"
#include "packet.h"
#include <vector>
#include <climits>
#include <cstdio>
#include <iostream>
#include <set>
using namespace std;

class router {
    
    

    public:
        std::vector<TableEntry> shortestPathTable;
        set<int> added;
        int update_add_flag=0;
        vector<packet> dvpackets;
        vector<packet> packet_buffer;
        int total_packets_sent=0;
        int name;
        int numNeighbors;
        vector<std::pair<int,int>> neighbors;
        router();
        router(int, TableEntry);
        void reset_update_flag(){ update_add_flag = 0; }
        void add_packet(packet p){ packet_buffer.push_back(p);}
        int forward_packet(packet);
        
    
    void addEntry(TableEntry t) {
        shortestPathTable.push_back(t);
        added.insert(t.dest);
        update_add_flag = 1;
    }
    void updateEntry(TableEntry t){
        for(int x = 0; x < shortestPathTable.size();x++){
            if(shortestPathTable.at(x).dest == t.dest){
                if(t.cost < shortestPathTable.at(x).cost ){
                    shortestPathTable.at(x) = t;
                    update_add_flag = 1;
                }
            }
        }
        
    }
    
    void setNumNeighbors(){
        numNeighbors = shortestPathTable.size();
        for(int x = 0; x < shortestPathTable.size(); x++ ) {
            neighbors.push_back(pair<int,int> (shortestPathTable.at(x).dest, shortestPathTable.at(x).cost) );
        }
    }
    void prepareDVPackets(){

        // prepare node cost pair vector of pairs
        std::vector<std::pair<int,int>> node_cost_pairs;
        for(int y = 0; y < shortestPathTable.size(); y++){
            node_cost_pairs.push_back(std::pair<int,int> (shortestPathTable.at(y).dest, shortestPathTable.at(y).cost));
        }
        
        for(int x = 0; x < numNeighbors; x++){
            dvpackets.push_back(packet(name, shortestPathTable.at(x).dest, node_cost_pairs));
        }
    }

    router sendDVPacket(router rec_router){

        //single src router sends single packet to single recieving router
        this->total_packets_sent += 1;

        packet packet_rec;

        

        // get dvpacket from src router for rec_router
        for(int z = 0; z < dvpackets.size(); z++){
            if(dvpackets.at(z).dest == rec_router.name){
                packet_rec = dvpackets.at(z);
            }
        }
        

        int cost_to_reach_sender = 0;
        // must find the cost to reach the sender by finding the packet src in our routing table 
        // rec_router's point of view
        for(int x = 0; x < rec_router.neighbors.size(); x++){
            int current_neighbor = rec_router.neighbors.at(x).first;
            
            if(current_neighbor == packet_rec.src){
                cost_to_reach_sender = rec_router.neighbors.at(x).second;
            }
        }
        

        // we now have the packet and the cost to reach the sender

        // process the packet on the recieving router

        // iterate of recieving router's shortest path table
        // if we find match, update it
        // if we dont match, add it

        for(int i = 0; i < rec_router.shortestPathTable.size(); i++){
            vector<TableEntry> rec_router_table = rec_router.shortestPathTable;
            vector<pair<int,int>> packet_list = packet_rec.node_cost_pairs;
            
            for(int j = 0; j < packet_list.size(); j++){
                // we have a match, check cost
            
                if(rec_router_table.at(i).dest == packet_list.at(j).first){
                    int new_cost = packet_list.at(j).second + cost_to_reach_sender;
                    if(new_cost < rec_router_table.at(i).cost){
                            rec_router.updateEntry(TableEntry(rec_router_table.at(i).dest, new_cost, packet_rec.src ));
                    }else {
                        //dont update
                    }
                } else {

                    if(rec_router.added.count(packet_list.at(j).first) == 0){
                        
                        // no knowledge prior, shortestpathtable nextHop = src of packet
                        //( nextHop becomes src)
                        rec_router.addEntry(TableEntry(packet_list.at(j).first, cost_to_reach_sender + packet_list.at(j).second, packet_rec.src));
                    }
                    
                }

            } 
            
        }
     
        // rec_router this router's table against our routers table
        // if better paths exist, update rec_router
        // if not, return rec_router

        return rec_router;
        
    }
    void clearDVPackets(){
        dvpackets.clear();
    }
    
   
    void displayList(){
        for(int i = 0; i < shortestPathTable.size(); i++){
            cout << shortestPathTable.at(i);
        }
    }
    friend std::ostream & operator << (std::ostream &out, const router &te);

};

router::router(){}

router::router(int n, TableEntry t){ 
    name = n;
    shortestPathTable.push_back(t);
    added.insert(n); //doesn't need to go to itself
    added.insert(t.dest);
}
int router::forward_packet(packet p){ // returns the next hop of the packet
        int next_hop = 0;
        int cost_local = 0;

        for(int x = 0; x < shortestPathTable.size(); x++ ){
            if(p.dest = shortestPathTable.at(x).dest){
                next_hop = shortestPathTable.at(x).nextHop;
                cost_local = shortestPathTable.at(x).cost;
            }else{
                next_hop = -1;
            }
        }

        if(next_hop != -1){
            cout << "Current router: " << name << " nextHop: " << next_hop << " Cost: " << cost_local << "\n";
        }else {
            cout<< "\nArrived";
        }

        return next_hop;
    }

std::ostream & operator << (std::ostream& out, const router& r)
    {
        out << " \nVector Table for router: " << r.name << "\n";
        for(int i = 0; i < r.shortestPathTable.size(); i++){
            out << r.shortestPathTable.at(i);
        }
        return out;
    }


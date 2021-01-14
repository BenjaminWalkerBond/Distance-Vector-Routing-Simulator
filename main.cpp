#include "router.cpp"
#include <stdlib.h>
#include <cstdio>
#include <list>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <set>
#include <map>
using namespace std;

int main(int argc, char *argv[]){

    if(argc != 3){ fprintf(stderr, "USAGE: %s topology_file_name round_count\n", argv[0]); exit(-1);}
    int rounds = atoi(argv[2]);
    if( rounds > 1000 ){ fprintf(stderr, "ERROR: rounds must be below 1,000\n"); exit(-1);}

    map<int, router> topology;

    // Read in topology file
    FILE* fp;
    fp = fopen(argv[1],"r");
    
    if(fp == NULL){ fprintf(stderr, "ERROR: could not open file \n"); exit(-1); }
    
    int src, dest, cost, currentRouter;
    src=0;
    dest=0;
    cost=0;

    currentRouter = -1; // loop counter to track if we need to add a router object

    set<int> added; //keeps track of if the router is inserted into our topology. 


    // Need to keep a list of routers.

    while( fscanf(fp, "%i  %i  %i", &src,&dest,&cost) != EOF ){

        // must add properties to routers every time they are either a src or a dest

        // 4 cases. src and dest alreaddy in set. 1 or other is in set. neither is set

        // insert src router and dest router into topology if it is not already inside
        if(added.count(src) == 0 && added.count(dest) == 0){ 
            topology.insert(pair<int,router>(src, router(src, TableEntry(dest,cost,dest))));
            topology.insert(pair<int,router>(dest, router(dest, TableEntry(src,cost,src))));

            added.insert(src);
            added.insert(dest);
            std::printf("added.count(src) is: %i \n", added.count(src));
            std::printf("src is: %i \n", src);

        }else if(added.count(src) == 0){

            topology.insert(pair<int,router>(src, router(src, TableEntry(dest,cost,dest))));
            added.insert(src);

            router temp = topology.at(dest);
            topology.erase(dest);
            temp.addEntry(TableEntry(src,cost,src));
            topology.insert(pair<int,router> (dest, temp) );
            

        }else if( added.count(dest) == 0){
            topology.insert(pair<int,router>(dest, router(dest, TableEntry(src,cost,src))));
            added.insert(dest);

            router temp = topology.at(src);
            topology.erase(src);
            temp.addEntry(TableEntry(dest,cost,dest));
            topology.insert(pair<int,router> (src, temp) );
        }else { // both are in the set. do nothing
            router temp = topology.at(src);
            topology.erase(src);
            temp.addEntry(TableEntry(dest,cost,dest));
            topology.insert(pair<int,router> (src, temp) );

            router temp2 = topology.at(dest);
            topology.erase(dest);
            temp2.addEntry(TableEntry(src,cost,src));
            topology.insert(pair<int,router> (dest, temp2) );

        }
        
    } // end while
  
    std::printf("Topology size: %i", topology.size());
    std::printf("\nINITIAL ROUTING TABLES\n");
    for (int i = 0; i < topology.size(); i++){
        std::cout << topology.at(i);
    }
    
    fclose(fp);
    

    // Initialize nodes and their initial vector tables

    //Start rounds

    //set number of neighbors for each router ( dictates number of packets sent)
    for(int j = 0; j < topology.size(); j++){ 
        topology.at(j).setNumNeighbors();
    }
    int converged = 0;
    int roundCounter = 0;
    router last_converged_node;
    int total_dv_packets = 0;
    for(int i = 0; i < rounds; i++){


            //nodes prepare DV packets
        for(int j = 0; j < topology.size(); j++){ 

            //prepare indiv dv packets here.
            topology.at(j).prepareDVPackets();
        }


        // we want the packets to send in order, and we dont want packets to send to routers theyve recieved packets from
        for(int j = 0; j < topology.size(); j++){
            
            router current_router = topology.at(j);
            // access current router and get its dvpacket list size.
            for( int z = 0; z < current_router.dvpackets.size(); z++){
                
                //send out all dv packets in the list.
                router recieving_router = topology.at( topology.at(j).dvpackets.at(z).dest );
                router updated_router = topology.at(j).sendDVPacket(recieving_router);
                
                
                topology.erase(updated_router.name);
                topology.insert(pair<int, router> (updated_router.name, updated_router) );

            }
            
        }

        std::cout << "AFTER senddvpackets function\n";

        for(int j = 0; j < topology.size(); j++){ 

            // clear the dvpackets in each router for the next round
            topology.at(j).clearDVPackets();
            // top1 should prepare 14 first round
        }

        std::printf("Topology size: %i", topology.size());
        for (int i = 0; i < topology.size(); i++){
            std::cout << topology.at(i);
        }
        // check if we have converged
        converged = 1;
        for (int i = 0; i < topology.size(); i++){
            if(topology.at(i).update_add_flag)
            {   
                last_converged_node = topology.at(i);
                converged = 0;
            }
        }
        // reset flags
        for (int i = 0; i < topology.size(); i++){
            topology.at(i).reset_update_flag();
        }
    
        std::printf("Round Counter: %i \n", roundCounter);
        if(converged){break;}
        roundCounter += 1;
    
    }
    for (int i = 0; i < topology.size(); i++)
            total_dv_packets += topology.at(i).total_packets_sent;
    std::cout << "Total DVPackets sent: " << total_dv_packets << " \n";
    std::cout << "Last Converged Node: " << last_converged_node.name << "\n";
    std::cout << "CONVERGED\n";
    
    // Route packets base on topology file input

    // For routing testing

    string test = argv[1];
    
    if(test.compare("topology1.txt") == 0 ){

        packet p(0,3);
        int next = 0;
        router starting_router;
        for(int x = 0; x < topology.size(); x++){
            if(topology.at(x).name == p.src){
                    starting_router = topology.at(x);
            }
        }

        while(next != -1){
            next = starting_router.forward_packet(p);

            for(int x = 0; x < topology.size(); x++){
            if(topology.at(x).name == next){
                    starting_router = topology.at(x);
            }
        }

        }

    }else if(test.compare("topology2.txt") == 0){

        packet p(0,7);
        int next = 0;
        router starting_router;
        for(int x = 0; x < topology.size(); x++){
            if(topology.at(x).name == p.src){
                    starting_router = topology.at(x);
            }
        }

        while(next != -1){
            next = starting_router.forward_packet(p);

            for(int x = 0; x < topology.size(); x++){
            if(topology.at(x).name == next){
                    starting_router = topology.at(x);
                }
            }
        }

    }else if(test.compare("topology3.txt") == 0){

        packet p(0,23);
        int next = 0;
        router starting_router;
        for(int x = 0; x < topology.size(); x++){
            if(topology.at(x).name == p.src){
                    starting_router = topology.at(x);
            }
        }

        while(next != -1){
            next = starting_router.forward_packet(p);

            for(int x = 0; x < topology.size(); x++){
            if(topology.at(x).name == next){
                    starting_router = topology.at(x);
                }
            }
        }

    }else {


    }
    

return 0;
}
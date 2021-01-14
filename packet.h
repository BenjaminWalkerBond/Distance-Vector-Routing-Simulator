#include <vector>
#include <utility>
#include <string>

class packet{
    public:
        int src;
        int dest;
        std::vector<std::pair<int,int>> node_cost_pairs;
        packet(){};
        packet(int s, int d){ src = s; dest = d;}
        packet(int s, int d, std::vector<std::pair<int,int>> ncp){
            src = s;
            dest = d;
            node_cost_pairs = ncp;
        }
        friend std::ostream & operator << (std::ostream &out, const packet &p);
};

std::ostream & operator << (std::ostream& out, const packet& p)
    {
        out << to_string(p.src) << " " << to_string(p.dest) << "\n ";
        for(int x = 0; x < p.node_cost_pairs.size(); x++){
            out << " ("  << p.node_cost_pairs.at(x).first << ", " << p.node_cost_pairs.at(x).second << ")";
        } 
        out << "\n";
        return out;
    }


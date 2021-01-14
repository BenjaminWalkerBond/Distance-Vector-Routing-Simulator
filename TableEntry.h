#include <string>
#include <iostream>
using namespace std;
class TableEntry{
    public:
        int dest;
        int cost;
        int nextHop;
    // destination, src, next hop
    TableEntry(int d, int c, int nH) {
        dest = d;
        cost = c;
        nextHop = nH;
    }
    friend std::ostream & operator << (std::ostream &out, const TableEntry &te);
    
    std::string displayTableEntry() {
        return to_string(dest) + to_string(cost) + to_string(nextHop);
    }
    
};

    std::ostream & operator << (std::ostream& out, const TableEntry& te)
    {
        out << to_string(te.dest) << " " << to_string(te.cost) << " " << to_string(te.nextHop) << " \n";
        //out << " test \n";
        return out;
    }
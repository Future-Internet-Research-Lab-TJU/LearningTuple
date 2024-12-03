#include "tuple.h"

using namespace RLTupleUtils;
using namespace std;

#define HashBasis 5381
#define HashMult 33

namespace RLTupleUtils{

    void PreferedTuple(const Rule& r, TupleTable& tuple,int a,int b,const  vector<int>& siplist,const  vector<int>& diplist) {
    int c=0;
    int d=0;
    
    if((r.prefix_length[0] > a )&&(r.prefix_length[1] > b)){
            tuple.push_back(a);
            tuple.push_back(b);
    }

    else if((r.prefix_length[0] < a )&&(r.prefix_length[1] > b)){
        //int step=min{0,}; 
       for(int i=0;i<siplist.size();i++){
               if(siplist[i] < a) {
               c=max(c,siplist[i]);}
       }
        if(c=0)
        {
         tuple.push_back(0);
         }
        else{
            tuple.push_back(c);
        }
        tuple.push_back(b);
    }

    else if((r.prefix_length[0] > a )&&(r.prefix_length[1] < b)){
        //int step=min{0,}; 
       tuple.push_back(a);
       for(int i=0;i<diplist.size();i++){
               if(diplist[i] < b) {
               d=max(d,diplist[i]);}
       }
        if(d=0)
        {
         tuple.push_back(0);
         }
        else{
            tuple.push_back(d);
        }
    }

    else if((r.prefix_length[0] < a )&&(r.prefix_length[1] > b)){
        //int step=min{0,}; 
       for(int i=0;i<siplist.size();i++){
               if(siplist[i] < a) {
               c=max(c,siplist[i]);}
       }
        if(c=0)
        {
         tuple.push_back(0);
         }
        else{
            tuple.push_back(c);
        }
        for(int i=0;i<diplist.size();i++){
               if(diplist[i] < b) {
               d=max(d,diplist[i]);}
       }
        if(d=0)
        {
         tuple.push_back(0);
         }
        else{
            tuple.push_back(d);
        }
    }

    }

    uint32_t Mask(int bits) {
        // Shifting is undefined if bits > 32
        // We want to make sure that for bits = 32 we get the correct result
        //全1
        if (bits == 32) return 0xFFFFFFFFu;
        else return ~(0xFFFFFFFFu << bits);
    }



}


inline vector<int> Dimify(const TupleTable& t) {
    vector<int> sol;
    if (t[FieldSA] > 0) sol.push_back(FieldSA);
    if (t[FieldDA] > 0) sol.push_back(FieldDA);
    if (t[FieldSP] > 16) sol.push_back(FieldSP);
    if (t[FieldDP] > 16) sol.push_back(FieldDP);
    if (t[FieldProto] > 24) sol.push_back(FieldProto);
    return sol;
}

inline vector<unsigned int> Lengthify(const TupleTable& t, const vector<int> dims) {
    vector<unsigned int> sol;
    for (int i : dims) {
        sol.push_back(t[i]);
    }
    return sol;
}


Table::Table(const TupleTable& tuple)
        : dims(Dimify(tuple)), lengths(Lengthify(tuple, dims))
{
    cmap_init(&map_in_tuple);
}

void Table::Insertion(const Rule& r, bool& priority_change) {
    cmap_node * new_node = new cmap_node(r);
    cmap_insert(&map_in_tuple, new_node, HashRule(r));

    priority_container.insert(r.priority);
    if (r.priority > maxPriority) {
        maxPriority = r.priority;
        priority_change = true;
    }

}

vector<Rule> Table::Collisions(const Rule& r) const {
    vector<Rule> rules;
    cmap_node * node = cmap_find(&map_in_tuple, HashRule(r));
    while (node != nullptr) {
        rules.push_back(*node->rule_ptr);
        node = node->next;
    }
    return rules;
}

size_t Table::NumCollisions(const Rule& r) const {
    size_t collide = 0;
    cmap_node * node = cmap_find(&map_in_tuple, HashRule(r));
    while (node != nullptr) {
        collide++;
        node = node->next;
    }
    return collide;
}

uint32_t inline Table::HashRule(const Rule& r) const {
    uint32_t hash = HashBasis;
    for (size_t i = 0; i < dims.size(); i++) {
        hash *= HashMult;
        hash += r.range[dims[i]][LowDim] & RLTupleUtils::Mask(lengths[i]);
    }
    return hash;

}

vector<Rule> Table::GetRules() const {
    vector<Rule> rules;
    cmap_cursor cursor = cmap_cursor_start(&map_in_tuple);

    while (cursor.node != nullptr) {
        rules.push_back(*cursor.node->rule_ptr);
        cmap_cursor_advance(&cursor);
    }

    return rules;
}
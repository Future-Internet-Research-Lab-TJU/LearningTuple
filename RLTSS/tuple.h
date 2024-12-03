#pragma once

#include "../ElementaryClasses.h"

#include "../OVS/TupleSpaceSearch.h"

#include <unordered_set>

namespace RLTupleUtils {
    typedef std::vector<int> TupleTable;

    int Log2(unsigned int x);
    int Sum(const std::vector<int>& v);

    uint32_t Mask(int bits);
    bool CompatibilityCheck(const TupleTable& ruleTuple, const TupleTable& tableTuple);
    bool AreSame(const TupleTable& t1, const TupleTable& t2);
    void PreferedTuple(const Rule& r, TupleTable& tuple,int a,int b,const  std::vector<int>& siplist,const  std::vector<int>& diplist);
    void BestTuple(const std::vector<Rule>& rules, TupleTable& tuple);
    uint32_t Hash(const Rule& r, const TupleTable& tuple);
    uint32_t Hash(const Packet& r, const TupleTable& tuple);

    bool IsHashable(const std::vector<Rule>& rules, size_t collisionLimit);
    void PrintTuple(const TupleTable& tuple);

    struct TupleHasher {
        std::size_t operator()(const TupleTable& v) const {
            int hash = 0;
            for (int x : v) {
                hash *= 17;
                hash += x;
            }
            return hash;
        }
    };

    struct TupleEquals {
        bool operator()(const TupleTable& x, const TupleTable& y) const {
            if (x.size() != y.size()) return false;
            for (size_t i = 0; i < x.size(); i++) {
                if (x[i] != y[i]) return false;
            }
            return true;
        }
    };

    typedef std::unordered_set<TupleTable, TupleHasher, TupleEquals> TupleSet;

    template<class T>
    using TupleMap = std::unordered_map<TupleTable, T, TupleHasher, TupleEquals>;
}

struct Table {
public:
    Table(const std::vector<int>& dims, const std::vector<unsigned int>& lengths)
            : dims(dims), lengths(lengths), maxPriority(-1) {
        cmap_init(&map_in_tuple);
    }
//    typedef std::vector<int> TupleTable;
    Table(const RLTupleUtils::TupleTable& tuple);
    ~Table() { cmap_destroy(&map_in_tuple); }

    bool IsEmpty() { return NumRules() == 0; }

    int ClassifyAPacket(const Packet& p) const;
    void Insertion(const Rule& r, bool& priority_change);
    bool Deletion(const Rule& r, bool& priority_change);

    bool CanInsert(const RLTupleUtils::TupleTable& tuple) const {
        for (size_t i = 0; i < dims.size(); i++) {
            if (lengths[i] == tuple[dims[i]]) 
            return true;
        }
        return false;
    }
    bool IsThatTuple(const RLTupleUtils::TupleTable& tuple) const;
    bool CanTakeRulesFrom(const Table* table) const;
    bool HaveSameTuple(const Table* table) const;

    size_t NumCollisions(const Rule& r) const;
    std::vector<Rule> Collisions(const Rule& r) const;
    std::vector<Rule> GetRules() const;

    int WorstAccesses() const;
    int NumRules() const  {
        return cmap_count(&map_in_tuple);
    }
    Memory MemSizeBytes(Memory ruleSizeBytes) const {
        return 	cmap_count(&map_in_tuple)* ruleSizeBytes + cmap_array_size(&map_in_tuple) * POINTER_SIZE_BYTES;
    }

    int MaxPriority() const { return maxPriority; };

    void prints(){
        for(int i : lengths){
            std::cout<<i<<"\t";
        }
        std::cout<<std::endl;
    }

protected:
    uint32_t inline HashRule(const Rule& r) const;
    uint32_t inline HashPacket(const Packet& p) const;

    cmap map_in_tuple;

    std::vector<int> dims;
    std::vector<unsigned int> lengths;

    int maxPriority = -1;
    std::multiset<int> priority_container;
};
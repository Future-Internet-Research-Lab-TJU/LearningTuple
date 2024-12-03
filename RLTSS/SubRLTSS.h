
#ifndef RLTSSV1_2_SUBRLTSS_H
#define RLTSSV1_2_SUBRLTSS_H

#include "./../TupleMerge/TupleMergeOnline.h"
#include "tuple.h"

#define inflation 10

using namespace std;


enum Fun{stop,hashs};
class SubRLTSS {
public:
    SubRLTSS();
    explicit SubRLTSS(const vector<Rule> &r);
    SubRLTSS(const vector<Rule> &r, int s, SubRLTSS* p);
    SubRLTSS(const vector<Rule> &r, vector<int> offsetBit);
    vector<SubRLTSS*> ConstructClassifier(const vector<int> &op, const string& mode);
    int ClassifyAPacket(const Packet& packet);
    void DeleteRule(const Rule& rule);
    void InsertRule(const Rule& rule);
    Memory MemSizeBytes() const;
    int MemoryAccess() const;

    void addReward(int r);
    vector<vector<int> > getReward();
    uint32_t getRuleSize();
    int getState() const;
    int getAction() const;
    vector<Rule> getRules();

    void printInfo();

    void FindRule(const Rule &rule);
    void FindPacket(const Packet &p);
    void recurDelete();


    int nodeId;
    vector<int> bigOffset;
    vector<int> siplist;
    vector<int> diplist;
    int n;


private:
    // Next
    TupleMergeOnline *TMO;
    PriorityTupleSpaceSearch *pstss;
    vector<SubRLTSS*> children;
    SubRLTSS* bigClassifier, *par;
    int maxBigPriority;

    int state, action, reward;

    // info
    int fun;
    vector<Rule> rules;
    int nHashTable, nHashBit;
    int dim, bit, offset; // bit: use high bits, offset use for get hashKey
    vector<int> offsetBit; // [0, 32]
    int sip,dip;

    // Hash fun
//    int getRuleKey(const Rule &r, int dim, int offset);
    uint32_t getRulePrefixKey(const Rule &r);
    uint32_t getKey(const Rule &r) const;
    uint32_t getKey(const Packet &p) const;


//    int inflation;
    vector<int> threshold;

    std::vector<Table*> tables;
    std::unordered_map<int, Table*> assignments; // Priority -> Table
    void Resort() {
        sort(tables.begin(), tables.end(), [](Table*& tx, Table*& ty) { return tx->MaxPriority() > ty->MaxPriority(); });
    }

};

#endif //RLTSSV1_2_SUBRLTSS_H

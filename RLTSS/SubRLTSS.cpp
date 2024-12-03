#include "SubRLTSS.h"
#include <bitset>

using namespace RLTupleUtils;
using namespace std;
using std::bitset;


SubRLTSS::SubRLTSS() {
    reward = -1;
    bigClassifier = nullptr;
    state = 0;
    maxBigPriority = -1;

}
SubRLTSS::SubRLTSS(const vector<Rule> &r) {
    this->rules = r;
    state = 0;
    reward = 0;
    n=0;
    bigClassifier = nullptr;
    maxBigPriority = -1;
    par = nullptr;
    bigOffset.resize(4, 0);

}
SubRLTSS::SubRLTSS(const vector<Rule> &r, vector<int> offsetBit) {
    this->rules = r;
    reward = -1;
    this->offsetBit = std::move(offsetBit);
    bigClassifier = nullptr;
    maxBigPriority = -1;
}
SubRLTSS::SubRLTSS(const vector<Rule> &r, int s, SubRLTSS* p) {
    this->rules = r;
    this->state = s;
    bigClassifier = nullptr;
    maxBigPriority = -1;
    reward = 0;
    par = p;
}

vector<SubRLTSS *> SubRLTSS::ConstructClassifier(const vector<int> &op, const string& mode) {
    this->fun = op[0];
    action = 0;
    action |= (fun << 8);
    switch(fun) {
        case stop: {
            break;
        }
        case hashs: {
            sip = op[1];
            dip = op[2];

            action |= (sip << 6);
            action |= op[2];
            siplist.push_back(sip);
            diplist.push_back(dip);  
            int rules_num = rules.size();
            for (int i = 0; i < rules_num; ++i){
                InsertRule(rules[i]);
            }


            int i=0;
    for (auto table : tables){
        if(table->NumRules() > 50){
            int hashChildenState = 0;
            hashChildenState |= (op[1] << 6);
            hashChildenState |= (op[2] << 6);
            vector<Rule> rl = table->GetRules();
            if (!rl.empty()) {
                children[i] = new SubRLTSS(rl, hashChildenState, this);                    
                i++;
            }
        }
    }
    vector<SubRLTSS*> next = children;
    return next;

}
}
}


void SubRLTSS::InsertRule(const Rule &rule) {

    TupleTable tuple;
    PreferedTuple(rule, tuple,sip,dip, siplist,diplist);
    for (auto table : tables){
        if (table->CanInsert(tuple)){

        bool hasChanged = false;
        table->Insertion(rule, hasChanged);
        vector<Rule> collisions = table->Collisions(rule);

        n=n+table->NumCollisions(rule) ;

        SubRLTSS *p = this;
        int rew = -n;
        while (p) {
                p->addReward(rew);
                p = p->par;
        }

        if (hasChanged) {
                Resort();
         }
         return;
        }

  
    }
     
     // Could not insert
    // So create a new table
    {
        bool ignore;
        Table * table = new Table(tuple);
        table->Insertion(rule, ignore);
        tables.push_back(table);
        Resort();
    }


}

int SubRLTSS::MemoryAccess() const {
    return 0;
}


vector<vector<int> > SubRLTSS::getReward() {
    vector<vector<int> > res;
    vector<int> currReward = {state, action, reward};

    res.push_back(currReward);
    for (auto iter : children) {
        if (iter) {
            vector<vector<int> > tmp = iter->getReward();
            res.insert(res.end(), tmp.begin(), tmp.end());
        }
    }

    return res;
}

uint32_t SubRLTSS::getRuleSize() {
    return rules.size();
}



vector<Rule> SubRLTSS::getRules() {
    return rules;
}


int SubRLTSS::getState() const {
    return state;
}

int SubRLTSS::getAction() const {
    return action;
}

void SubRLTSS::addReward(int r) {
    this->reward += r;
}


string state2str(int state) {
    string res;
    vector<int> vec(4, -1);
    for (int i = 0; i < 4; i++) {
        if (state & 1) {
            state >>= 1;
            vec[i] = state & 15;
            state >>= 4;
        }
        res += to_string(vec[i]);
        res += "   ";
    }

    return res;

}

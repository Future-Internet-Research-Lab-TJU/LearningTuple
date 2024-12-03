
#include "RLTSS.h"

RLTSS::RLTSS() {
    binth = 8;
}

void RLTSS::ConstructClassifier(const vector<Rule> &rules) {

    train(rules);
    
    root = new SubRLTSS(rules);
    queue<SubRLTSS*> que;
    que.push(root);
    bool flag = true;
    int nNode = 0;
    while(!que.empty()) {
        SubRLTSS *node = que.front();
        que.pop();
        if (node) {
            node->nodeId = nNode ++;
        }
        vector<int> op = getAction(node, 99);
        cout<<"op1="<<op[1]<<"op2="<<op[2]<<endl;
        if (flag) {
            flag = false;
       //     cout << op[0] << "\t" << op[1] << "\t" << op[2] << endl;
        }
        vector<SubRLTSS*> children = node->ConstructClassifier(op, "build");
        for (auto iter : children) {
            if (iter) {
                que.push(iter);
            }
        }
    }
  vector<vector<int> > reward = root->getReward();

}

int RLTSS::ClassifyAPacket(const Packet &packet) {
//    return root->ClassifyAPacket(packet);
    return 0;
}

void RLTSS::DeleteRule(const Rule &rule) {
 //   root->DeleteRule(rule);
 return;
}

void RLTSS::InsertRule(const Rule &rule) {
    root->InsertRule(rule);

}

Memory RLTSS::MemSizeBytes() const {
 //   return root->MemSizeBytes();
 return 0;
}

int RLTSS::MemoryAccess() const {
    return 0;
}

size_t RLTSS::NumTables() const {
    return 0;
}

size_t RLTSS::RulesInTable(size_t tableIndex) const {
    return 0;
}

vector<int> RLTSS::getAction(SubRLTSS *state, int epsilion = 100) {
    if (!state) {
        cout << "state node exist" << endl;
        exit(-1);
    }

    int s = state->getState();
    vector<Rule> nodeRules = state->getRules();
    if (nodeRules.size() < binth) {
        return {stop, -1, -1};
    }
    int num = rand() % 100;
    vector<vector<int> > Actions;
    vector<double> rews;

    for (int j = 0; j < 32; j++) {
        for (int k = 0; k < 32; k++) {
            Actions.push_back({hashs, j, k});
            int act = (j << 4) | k;
            rews.push_back(QTable[s][act]);
        }    
        }
            
    if (rews.empty()) {
        return {stop, -1, -1};
    }
    if (num <= epsilion) {
        // E-greedy
        vector<int> op;
        double maxReward = rews[0];
        op = Actions[0];
        for (int i = 0; i < rews.size(); i++) {
            if (rews[i] > maxReward) {
                maxReward = rews[i];
                op = Actions[i];
            }
        }
        return op;
    } else {
        int N = rand() % rews.size();
        return Actions[N];
    }

    return {stop, -1, -1};

}

void RLTSS::train(const vector<Rule> &rules) {
    int stateSize = 1 << 20, actionSize = 1 << 8;
    QTable.resize(stateSize, vector<double>(actionSize, 0.0));
    uint32_t loopNum = 10000;
    int trainRate = 10;
    for (int i = 0; i < loopNum; i++) {
       // cout << "next loop" << endl;
        if(i >= loopNum / 10 && i % (loopNum / 10) == 0){
            std::cout<<"Training finish "<<trainRate<<"% ...............Remaining: "<<100 - trainRate<<"%"<<std::endl;
            trainRate += 10;
        }
        auto *tmpRoot = new SubRLTSS(rules);
   
        queue<SubRLTSS*> que;
        que.push(tmpRoot);
        // cout<<"b"<<endl;
        while (!que.empty()) {
            SubRLTSS* node = que.front();
            que.pop();
            vector<int> op = getAction(node, 50);
                   
            vector<SubRLTSS*> children = node->ConstructClassifier(op, "train");
            for (auto iter : children) {
                if (iter) {
                    que.push(iter);
                }
            }
        }
        vector<vector<int> > reward;
        reward = tmpRoot->getReward();

        for (auto iter : reward) {
            if ((iter[1] >> 6) != 3) {
                continue;
            }
            int s = iter[0], a = iter[1] & ((1 << 9) - 1), r = iter[2];
            double lr = 0.1;
            
            if (QTable[s][a] == 0) {
                QTable[s][a] = r;
            } else {
                
                QTable[s][a] += lr * (r - QTable[s][a]);

            }
        }
        int act = reward[0][1] & ((1 << 6) - 1);

    }
}



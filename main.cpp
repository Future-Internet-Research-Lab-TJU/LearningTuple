#include <iostream>
#include <fstream>
#include <string>
#include "ElementaryClasses.h"
#include "./RLTSS/RLTSS.h"
#include "./CutTSS/CutTSS.h"
using namespace std;

string ruleFile, packetFile;
FILE *fpr = fopen("./ack_1k", "r"), *fpt=fopen("./acl_1k_trace", "r");
vector<Rule> rules;
vector<Packet> packets;
std::chrono::time_point<std::chrono::steady_clock> Start, End;
std::chrono::duration<double> elapsed_seconds{};
std::chrono::duration<double,std::milli> elapsed_milliseconds{};
vector<int> randUpdate;
int nInsert, nDelete;
ofstream fError("ErrorLog.csv", ios::app);

void testPerformance(PacketClassifier *p) {
    cout << p->funName() << ":" << endl;
    cout<<"a"<<endl;
    Start = std::chrono::steady_clock::now();
    p->ConstructClassifier(rules);
    End = std::chrono::steady_clock::now();
    elapsed_milliseconds = End - Start;
    cout << "\tConstruction time: " << elapsed_milliseconds.count() <<" ms " << endl;
      printf("\nClassify Performance:\n");
        std::chrono::duration<double> sumTime(0);
        int matchPri = -1, matchMiss = 0;
        int nPacket = int(packets.size());
        int nRules = int(rules.size());
        vector<int> results(nPacket, -1);
        const int trials = 10;
        for (int i = 0; i < trials; i++) {
            Start = std::chrono::steady_clock::now();
            for (int j = 0; j < nPacket; j++) {
                matchPri = p->ClassifyAPacket(packets[j]);
                results[j] = nRules - 1 - matchPri;
            }
            End = std::chrono::steady_clock::now();
            sumTime += End - Start;
            for (int j = 0; j < nPacket; j++) {
                if (results[j] == nRules || packets[j][5] < results[j]) {

                    matchMiss ++;
                }
            }
        }

        printf("\t%d packets are classified, %d of them are misclassified\n", nPacket * trials, matchMiss);
        printf("\tTotal classification time: %f s\n", sumTime.count() / trials);
        printf("\tAverage classification time: %f us\n", sumTime.count() * 1e6 / (trials * nPacket));
        printf("\tThroughput: %f Mpps\n", 1 / (sumTime.count() * 1e6 / (trials * nPacket)));

}

int main(int argc, char* argv[]) {
    rules = loadrule(fpr);
    packets = loadpacket(fpt);
    cout << rules.size() << endl;
    cout << packets.size() << endl;

    // ---RLTSS---Construction---
    PacketClassifier *RL = new RLTSS();
    testPerformance(RL);
    return 0;
}

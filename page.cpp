#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

using namespace std;

int fifo(const vector<int>& refs, int frameSize) {
    unordered_set<int> frames;
    queue<int> q;
    int faults = 0;

    for (int page : refs) {
        if (frames.find(page) == frames.end()) {
            faults++;
            if (frames.size() == frameSize) {
                int old = q.front(); q.pop();
                frames.erase(old);
            }
            frames.insert(page);
            q.push(page);
        }
    }
    return faults;
}

int lru(const vector<int>& refs, int frameSize) {
    unordered_set<int> frames;
    unordered_map<int, int> lastUsed;
    int faults = 0;

    for (int i = 0; i < refs.size(); ++i) {
        int page = refs[i];
        if (frames.find(page) == frames.end()) {
            faults++;
            if (frames.size() == frameSize) {
                // find LRU page
                int lruPage = -1, lruTime = i;
                for (int p : frames) {
                    if (lastUsed[p] < lruTime) {
                        lruTime = lastUsed[p];
                        lruPage = p;
                    }
                }
                frames.erase(lruPage);
            }
            frames.insert(page);
        }
        lastUsed[page] = i;
    }
    return faults;
}

int opt(const vector<int>& refs, int frameSize) {
    unordered_set<int> frames;
    int faults = 0;

    for (int i = 0; i < refs.size(); ++i) {
        int page = refs[i];
        if (frames.find(page) == frames.end()) {
            faults++;
            if (frames.size() == frameSize) {
                // find the page to replace
                int farthest = i, victim = -1;
                for (int p : frames) {
                    int nextUse = -1;
                    for (int j = i + 1; j < refs.size(); ++j) {
                        if (refs[j] == p) {
                            nextUse = j;
                            break;
                        }
                    }
                    if (nextUse == -1) { // never used again
                        victim = p;
                        break;
                    }
                    if (nextUse > farthest) {
                        farthest = nextUse;
                        victim = p;
                    }
                }
                frames.erase(victim);
            }
            frames.insert(page);
        }
    }
    return faults;
}

int main() {
    ifstream inFile("page.inp");
    ofstream outFile("page.out");

    int frameSize;
    inFile >> frameSize;

    vector<int> refs;
    int ref;
    while (inFile >> ref && ref != -1) {
        refs.push_back(ref);
    }

    int fifoFaults = fifo(refs, frameSize);
    int lruFaults = lru(refs, frameSize);
    int optFaults = opt(refs, frameSize);

    outFile << "FIFO: " << fifoFaults << endl;
    outFile << "LRU: " << lruFaults << endl;
    outFile << "OPT: " << optFaults << endl;

    inFile.close();
    outFile.close();

    return 0;
}

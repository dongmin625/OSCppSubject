//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <queue>
//#include <algorithm>
//
//using namespace std;
//
//struct Process {
//    int pid, requestTime, duration, size;
//    int allocTime = -1;
//    int finishTime = -1;
//    int allocAddr = -1;
//};
//
//struct Block {
//    int address, pid, size; // pid == -1이면 hole
//};
//
//class MemorySimulator {
//    int time = 0;
//    const int MEM_SIZE = 1000;
//    vector<Block> memory;
//    vector<Process> active;
//    vector<Process> waiting;
//    vector<Process> allProcesses;
//
//public:
//    enum FitType { FIRST, BEST, WORST };
//
//public:
//    MemorySimulator(vector<Process> procs) : allProcesses(procs) {}
//
//    int run(FitType fitType) {
//        memory = { {0, -1, MEM_SIZE} };
//        active.clear();
//        waiting.clear();
//        time = 0;
//
//        auto procList = allProcesses;
//        int total = procList.size();
//        int completed = 0;
//
//        while (completed < total) {
//            // 1. 종료
//            for (auto it = active.begin(); it != active.end();) {
//                if (it->finishTime == time) {
//                    deallocate(it->pid);
//                    it = active.erase(it);
//                    completed++;
//                }
//                else ++it;
//            }
//
//            mergeHoles();
//
//            // 2. 요청
//            for (auto it = procList.begin(); it != procList.end();) {
//                if (it->requestTime == time) {
//                    waiting.push_back(*it);
//                    it = procList.erase(it);
//                }
//                else break; // 요청 시각은 비감소
//            }
//
//            // 3. 할당
//            for (auto it = waiting.begin(); it != waiting.end();) {
//                int index = findHole(*it, fitType);
//                if (index != -1) {
//                    Block& b = memory[index];
//                    int leftover = b.size - it->size;
//                    int addr = b.address;
//
//                    // 메모리 갱신
//                    memory[index] = { addr, it->pid, it->size };
//                    if (leftover > 0)
//                        memory.insert(memory.begin() + index + 1, { addr + it->size, -1, leftover });
//
//                    it->allocTime = time;
//                    it->finishTime = time + it->duration;
//                    it->allocAddr = addr;
//                    active.push_back(*it);
//
//                    // ?? 여기서 allProcesses도 반드시 업데이트해야 결과에 반영됨
//                    allProcesses[it->pid].allocAddr = addr;
//
//                    it = waiting.erase(it);
//                }
//                else ++it;
//            }
//
//            ++time;
//        }
//
//        return allProcesses.back().allocAddr; // 마지막 PID(n-1)의 시작 주소 반환
//    }
//
//private:
//    int findHole(const Process& p, int fitType) {
//        vector<pair<int, int>> candidates; // {index, size}
//
//        for (int i = 0; i < memory.size(); ++i) {
//            const Block& b = memory[i];
//            if (b.pid == -1 && b.size >= p.size) {
//                candidates.push_back({ i, b.size });
//                if (fitType == FIRST) return i;
//            }
//        }
//
//        if (fitType == BEST) {
//            if (candidates.empty()) return -1;
//            return min_element(candidates.begin(), candidates.end(),
//                [](const pair<int, int>& a, const pair<int, int>& b) {
//                    return a.second < b.second;
//                })->first;
//        }
//
//        if (fitType == WORST) {
//            if (candidates.empty()) return -1;
//            return max_element(candidates.begin(), candidates.end(),
//                [](const pair<int, int>& a, const pair<int, int>& b) {
//                    return a.second < b.second;
//                })->first;
//        }
//
//        return -1;
//    }
//
//
//    void deallocate(int pid) {
//        for (auto& b : memory) {
//            if (b.pid == pid) b.pid = -1;
//        }
//    }
//
//    void mergeHoles() {
//        vector<Block> merged;
//        for (const auto& b : memory) {
//            if (!merged.empty() && merged.back().pid == -1 && b.pid == -1)
//                merged.back().size += b.size;
//            else
//                merged.push_back(b);
//        }
//        memory = merged;
//    }
//};
//
//int main() {
//    ifstream fin("allocation.inp");
//    ofstream fout("allocation.out");
//
//    int n;
//    fin >> n;
//    vector<Process> procs(n);
//    for (int i = 0; i < n; ++i) {
//        fin >> procs[i].requestTime >> procs[i].duration >> procs[i].size;
//        procs[i].pid = i;
//    }
//
//    MemorySimulator simulator(procs);
//    fout << simulator.run(MemorySimulator::FIRST) << '\n';
//    fout << simulator.run(MemorySimulator::BEST) << '\n';
//    fout << simulator.run(MemorySimulator::WORST) << '\n';
//
//    fin.close();
//    fout.close();
//    return 0;
//}

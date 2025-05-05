//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <queue>
//#include <climits>
//
//using namespace std;
//
//enum State { READY, RUNNING, WAITING, FINISHED };
//
//struct Process {
//    int iPID = 0; // 프로세스 ID 입력순
//    int arrivalTime = 0; // 도착 시간
//    vector<int> burstSequence; // 시간 순서 저장
//    int brustIndex = 0; // 인덱스
//    State state = READY; // 초기상태
//    int readyQueueArrivalTime = 0;
//    int remainingIoTime = 0;
//};
//
//struct CompareProcess { // 우선순위 조건
//    bool operator()(const Process& a, const Process& b) {
//        int nextCpuA = INT_MAX;
//        if (a.state == READY && a.brustIndex < a.burstSequence.size() && a.brustIndex % 2 == 1) {
//            nextCpuA = a.burstSequence[a.brustIndex];
//        }
//        int nextCpuB = INT_MAX;
//        if (b.state == READY && b.brustIndex < b.burstSequence.size() && b.brustIndex % 2 == 1) {
//            nextCpuB = b.burstSequence[b.brustIndex];
//        }
//
//        if (nextCpuA != nextCpuB) return nextCpuA > nextCpuB;
//        if (a.readyQueueArrivalTime != b.readyQueueArrivalTime) return a.readyQueueArrivalTime > b.readyQueueArrivalTime;
//        return a.iPID > b.iPID;
//    }
//};
//
//int main() {
//    ifstream inputFile("sjf.inp");
//    if (!inputFile) {
//        cerr << "입력 파일을 열 수 없습니다." << endl;
//        return -1;
//    }
//
//    int N;
//    inputFile >> N;
//    inputFile.ignore();
//
//    if (N < 3 || N > 1000) {
//        cerr << "프로세스 개수 N은 3 이상 1000 이하여야 합니다." << endl;
//        return -1;
//    }
//
//    vector<Process> processes(N); // N 개의 프로세스
//    for (int i = 0; i < N; ++i) {
//        processes[i].iPID = i + 1;
//        int time;
//        inputFile >> processes[i].arrivalTime; // 첫 번째 값은 도착 시간
//        while (inputFile >> time) {
//            if (time == -1) break;
//            processes[i].burstSequence.push_back(time); // CPU와 IO 버스트 시간 저장
//        }
//        processes[i].readyQueueArrivalTime = processes[i].arrivalTime; // 도착시간 설정
//    }
//    inputFile.close();
//
//    int iCurrentTime = 0; // 클록
//    int iCPUIdleTime = 0; // cpu idletime
//    int completedProcesses = 0; // 작업 완료된 프로세스
//
//    vector<int> iProcessEndTime(N); // 각 프로세스들의 종료시간을 저장할 vector
//
//    priority_queue<Process, vector<Process>, CompareProcess> readyQueue; // 순서가 있는 queue (위의 우선순위 조건)
//
//    vector<Process> waitingQueue; // I/O 를 실행중인 Waiting 상태의 프로세스들
//
//    for (auto& process : processes) {
//        readyQueue.push(process);
//    }
//
//    bool cpuBusy = false; // cpu가 사용중인지 저장할 변수
//    Process currentProcess; // 현재 실행중인 프로세스
//
//    while (completedProcesses < N) {
//        // I/O 병렬 처리
//        for (auto it = waitingQueue.begin(); it != waitingQueue.end(); ) {
//            if (it->brustIndex >= it->burstSequence.size()) {
//                it = waitingQueue.erase(it);
//                continue;
//            }
//
//            it->remainingIoTime--;
//
//            if (it->remainingIoTime <= 0) {
//                it->brustIndex++; // I/O 완료 -> 다음 CPU
//
//                if (it->brustIndex >= it->burstSequence.size()) {
//                    // 모든 burst 완료
//                    it->state = FINISHED;
//                    iProcessEndTime[it->iPID - 1] = iCurrentTime;
//                    completedProcesses++;
//                    it = waitingQueue.erase(it);
//                    continue;
//                }
//
//                // 다음이 CPU burst라면 준비 큐로
//                if (it->brustIndex % 2 == 0) {
//                    it->state = READY;
//                    it->readyQueueArrivalTime = iCurrentTime;
//                    readyQueue.push(*it);
//                }
//
//                it = waitingQueue.erase(it);
//            }
//            else {
//                ++it;
//            }
//        }
//
//        // CPU가 비어 있고 readyQueue에 준비상태 프로세스가 있을때 readyQueue에서 SJF로 선택
//        if (!cpuBusy && !readyQueue.empty()) {
//            currentProcess = readyQueue.top(); readyQueue.pop();
//
//            // 출력: 현재 실행 중인 프로세스와 idleTime
//            cout << "Time= " << iCurrentTime << " selectedProcess= " << currentProcess.iPID << " idleTime= " << iCPUIdleTime << endl;
//
//            // CPU burst가 0이면 시간을 변하지 않고 바로 다음 프로세스로 넘어가도록 처리
//            //if (currentProcess.burstSequence[currentProcess.brustIndex] == 0) {
//            //    // 현재 프로세스가 CPU burst 0을 가질 경우, 현재 시간을 변경하지 않음
//            //    // 바로 I/O 로 넘김
//            //    currentProcess.brustIndex++;
//            //    if (currentProcess.brustIndex < currentProcess.burstSequence.size() && currentProcess.brustIndex % 2 == 1) {
//            //        currentProcess.state = WAITING; // I/O 상태로 변경
//            //        currentProcess.remainingIoTime = currentProcess.burstSequence[currentProcess.brustIndex]; // I/O 시간이 설정됨
//            //        waitingQueue.push_back(currentProcess); // I/O 대기 큐에 추가
//            //    }
//            //    continue;
//            //}
//            currentProcess.state = RUNNING;
//            cpuBusy = true;
//        }
//
//        // CPU 실행
//        if (cpuBusy) {
//            currentProcess.burstSequence[currentProcess.brustIndex]--;
//
//            if (currentProcess.burstSequence[currentProcess.brustIndex] == 0) {
//                currentProcess.brustIndex++;
//
//                if (currentProcess.brustIndex >= currentProcess.burstSequence.size()) {
//                    currentProcess.state = FINISHED;
//                    iProcessEndTime[currentProcess.iPID - 1] = iCurrentTime + 1;
//                    completedProcesses++;
//                }
//                else {
//                    currentProcess.state = WAITING;
//                    currentProcess.remainingIoTime = currentProcess.burstSequence[currentProcess.brustIndex];
//                    waitingQueue.push_back(currentProcess);
//                }
//
//                cpuBusy = false;
//            }
//        }
//        else {
//            iCPUIdleTime++;
//        }
//
//        iCurrentTime++;
//    }
//
//    ofstream outputFile("sjf.out");
//    if (!outputFile) {
//        cerr << "출력 파일을 열 수 없습니다." << endl;
//        return -1;
//    }
//
//    outputFile << iCPUIdleTime << endl;
//    for (int i = 0; i < iProcessEndTime.size(); ++i) {
//        outputFile << iProcessEndTime[i] << endl;
//    }
//
//    outputFile.close();
//
//    return 0;
//}

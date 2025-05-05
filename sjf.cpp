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
//    int iPID = 0; // ���μ��� ID �Է¼�
//    int arrivalTime = 0; // ���� �ð�
//    vector<int> burstSequence; // �ð� ���� ����
//    int brustIndex = 0; // �ε���
//    State state = READY; // �ʱ����
//    int readyQueueArrivalTime = 0;
//    int remainingIoTime = 0;
//};
//
//struct CompareProcess { // �켱���� ����
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
//        cerr << "�Է� ������ �� �� �����ϴ�." << endl;
//        return -1;
//    }
//
//    int N;
//    inputFile >> N;
//    inputFile.ignore();
//
//    if (N < 3 || N > 1000) {
//        cerr << "���μ��� ���� N�� 3 �̻� 1000 ���Ͽ��� �մϴ�." << endl;
//        return -1;
//    }
//
//    vector<Process> processes(N); // N ���� ���μ���
//    for (int i = 0; i < N; ++i) {
//        processes[i].iPID = i + 1;
//        int time;
//        inputFile >> processes[i].arrivalTime; // ù ��° ���� ���� �ð�
//        while (inputFile >> time) {
//            if (time == -1) break;
//            processes[i].burstSequence.push_back(time); // CPU�� IO ����Ʈ �ð� ����
//        }
//        processes[i].readyQueueArrivalTime = processes[i].arrivalTime; // �����ð� ����
//    }
//    inputFile.close();
//
//    int iCurrentTime = 0; // Ŭ��
//    int iCPUIdleTime = 0; // cpu idletime
//    int completedProcesses = 0; // �۾� �Ϸ�� ���μ���
//
//    vector<int> iProcessEndTime(N); // �� ���μ������� ����ð��� ������ vector
//
//    priority_queue<Process, vector<Process>, CompareProcess> readyQueue; // ������ �ִ� queue (���� �켱���� ����)
//
//    vector<Process> waitingQueue; // I/O �� �������� Waiting ������ ���μ�����
//
//    for (auto& process : processes) {
//        readyQueue.push(process);
//    }
//
//    bool cpuBusy = false; // cpu�� ��������� ������ ����
//    Process currentProcess; // ���� �������� ���μ���
//
//    while (completedProcesses < N) {
//        // I/O ���� ó��
//        for (auto it = waitingQueue.begin(); it != waitingQueue.end(); ) {
//            if (it->brustIndex >= it->burstSequence.size()) {
//                it = waitingQueue.erase(it);
//                continue;
//            }
//
//            it->remainingIoTime--;
//
//            if (it->remainingIoTime <= 0) {
//                it->brustIndex++; // I/O �Ϸ� -> ���� CPU
//
//                if (it->brustIndex >= it->burstSequence.size()) {
//                    // ��� burst �Ϸ�
//                    it->state = FINISHED;
//                    iProcessEndTime[it->iPID - 1] = iCurrentTime;
//                    completedProcesses++;
//                    it = waitingQueue.erase(it);
//                    continue;
//                }
//
//                // ������ CPU burst��� �غ� ť��
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
//        // CPU�� ��� �ְ� readyQueue�� �غ���� ���μ����� ������ readyQueue���� SJF�� ����
//        if (!cpuBusy && !readyQueue.empty()) {
//            currentProcess = readyQueue.top(); readyQueue.pop();
//
//            // ���: ���� ���� ���� ���μ����� idleTime
//            cout << "Time= " << iCurrentTime << " selectedProcess= " << currentProcess.iPID << " idleTime= " << iCPUIdleTime << endl;
//
//            // CPU burst�� 0�̸� �ð��� ������ �ʰ� �ٷ� ���� ���μ����� �Ѿ���� ó��
//            //if (currentProcess.burstSequence[currentProcess.brustIndex] == 0) {
//            //    // ���� ���μ����� CPU burst 0�� ���� ���, ���� �ð��� �������� ����
//            //    // �ٷ� I/O �� �ѱ�
//            //    currentProcess.brustIndex++;
//            //    if (currentProcess.brustIndex < currentProcess.burstSequence.size() && currentProcess.brustIndex % 2 == 1) {
//            //        currentProcess.state = WAITING; // I/O ���·� ����
//            //        currentProcess.remainingIoTime = currentProcess.burstSequence[currentProcess.brustIndex]; // I/O �ð��� ������
//            //        waitingQueue.push_back(currentProcess); // I/O ��� ť�� �߰�
//            //    }
//            //    continue;
//            //}
//            currentProcess.state = RUNNING;
//            cpuBusy = true;
//        }
//
//        // CPU ����
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
//        cerr << "��� ������ �� �� �����ϴ�." << endl;
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

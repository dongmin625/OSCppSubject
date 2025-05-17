//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include <queue>
//using namespace std;
//
//// 전역 변수 선언
//int n, m; // n: 프로세스 수, m: 자원 종류 수
//vector<int> totalResources;  // 전체 자원 수
//vector<int> available;       // 현재 사용 가능한 자원 수
//vector<vector<int>> maxMatrix, allocation, need; // MAX, 할당, 필요 자원 매트릭스
//
//ofstream fout("banker.out"); // 결과를 출력할 파일
//
//// 현재 사용 가능한 자원을 출력
//void printAvailable() {
//    for (int i = 0; i < m; i++) {
//        fout << available[i];
//        if (i != m - 1) fout << " ";
//    }
//    fout << "\n";
//}
//
//// 현재 상태가 안전한지 확인하는 함수 (안전성 검사)
//bool isSafeState() {
//    vector<int> work = available; // 현재 자원을 work에 복사
//    vector<bool> finish(n, false); // 각 프로세스의 완료 여부
//
//    while (true) {
//        bool progress = false;
//        for (int i = 0; i < n; i++) {
//            if (!finish[i]) {
//                // 현재 프로세스가 필요한 자원을 work가 모두 충족할 수 있는지 확인
//                bool canFinish = true;
//                for (int j = 0; j < m; j++) {
//                    if (need[i][j] > work[j]) {
//                        canFinish = false;
//                        break;
//                    }
//                }
//
//                // 해당 프로세스가 자원 요구 없이 종료할 수 있다면
//                if (canFinish) {
//                    // 자원을 반납받음 (할당된 자원을 다시 work에 추가)
//                    for (int j = 0; j < m; j++)
//                        work[j] += allocation[i][j];
//                    finish[i] = true;
//                    progress = true;
//                }
//            }
//        }
//        if (!progress) break; // 어떤 프로세스도 더 이상 종료할 수 없으면 반복 종료
//    }
//
//    // 모든 프로세스가 종료 가능한 상태이면 안전한 상태
//    for (bool done : finish)
//        if (!done) return false;
//    return true;
//}
//
//// 자원 요청 처리 함수
//bool requestResources(int pid, const vector<int>& req) {
//    for (int i = 0; i < m; i++) {
//        if (req[i] > need[pid][i]) return false;     // 필요량 초과 요청
//        if (req[i] > available[i]) return false;     // 자원이 부족한 경우
//    }
//
//    // 자원을 임시로 할당 (시뮬레이션)
//    for (int i = 0; i < m; i++) {
//        available[i] -= req[i];
//        allocation[pid][i] += req[i];
//        need[pid][i] -= req[i];
//    }
//
//    // 이 상태가 안전한지 확인
//    if (!isSafeState()) {
//        // 안전하지 않다면 원래 상태로 되돌림
//        for (int i = 0; i < m; i++) {
//            available[i] += req[i];
//            allocation[pid][i] -= req[i];
//            need[pid][i] += req[i];
//        }
//        return false;
//    }
//
//    return true; // 요청 성공
//}
//
//// 자원 반납 함수
//void releaseResources(int pid, const vector<int>& rel) {
//    for (int i = 0; i < m; i++) {
//        allocation[pid][i] -= rel[i]; // 할당된 자원 감소
//        available[i] += rel[i];       // 사용 가능한 자원 증가
//        need[pid][i] += rel[i];       // 필요 자원 증가
//    }
//}
//
//int main() {
//    ifstream fin("banker.inp");
//    if (!fin) {
//        cerr << "banker.inp 파일을 열 수 없습니다.\n";
//        return 1;
//    }
//
//    string line;
//    getline(fin, line);
//    stringstream ss(line);
//    ss >> n >> m; // 프로세스 수, 자원 종류 수 읽기
//
//    // 전체 자원 수 입력
//    totalResources.resize(m);
//    available.resize(m);
//    getline(fin, line);
//    ss.clear(); ss.str(line);
//    for (int i = 0; i < m; i++) {
//        ss >> totalResources[i];
//        available[i] = totalResources[i]; // 처음에는 모두 사용 가능
//    }
//
//    // MAX, ALLOCATION, NEED 초기화
//    maxMatrix.assign(n, vector<int>(m));
//    allocation.assign(n, vector<int>(m));
//    need.assign(n, vector<int>(m));
//
//    // MAX matrix 입력
//    int count = 0;
//    while (getline(fin, line)) {
//        if (line.empty()) continue;
//        ss.clear(); ss.str(line);
//        for (int j = 0; j < m; j++) ss >> maxMatrix[count][j];
//        count++;
//        if (count == n) break;
//    }
//
//    // ALLOCATION matrix 입력
//    count = 0;
//    while (getline(fin, line)) {
//        if (line.empty()) continue;
//        ss.clear(); ss.str(line);
//        for (int j = 0; j < m; j++) ss >> allocation[count][j];
//        count++;
//        if (count == n) break;
//    }
//
//    // NEED 계산 및 available 재계산
//    for (int i = 0; i < n; i++)
//        for (int j = 0; j < m; j++) {
//            need[i][j] = maxMatrix[i][j] - allocation[i][j];
//            available[j] -= allocation[i][j]; // 사용 중인 자원만큼 제외
//        }
//
//    queue<pair<int, vector<int>>> waitQueue; // 요청 실패 시 기다리는 큐
//
//    // 명령어 처리 (request, release, quit)
//    while (getline(fin, line)) {
//        if (line.empty()) continue;
//        ss.clear(); ss.str(line);
//        string cmd;
//        ss >> cmd;
//
//        if (cmd == "quit") break; // 종료 명령
//
//        int pid;
//        ss >> pid;
//        vector<int> vec(m);
//        for (int i = 0; i < m; i++) ss >> vec[i];
//
//        if (cmd == "request") {
//            // 요청이 실패하면 대기 큐에 넣음
//            if (!requestResources(pid, vec)) {
//                waitQueue.push({ pid, vec });
//            }
//        }
//        else if (cmd == "release") {
//            releaseResources(pid, vec); // 자원 반납
//
//            // 자원 반납 후, 대기 큐 확인 → 다시 요청 시도
//            int qsize = waitQueue.size();
//            for (int i = 0; i < qsize; i++) {
//                pair<int, vector<int>> front = waitQueue.front();
//                int wpid = front.first;
//                vector<int> wres = front.second;
//                waitQueue.pop();
//
//                // 다시 시도해서 실패하면 다시 큐에 넣음
//                if (!requestResources(wpid, wres)) {
//                    waitQueue.push({ wpid, wres });
//                }
//            }
//        }
//
//        // 현재 자원 상태 출력
//        printAvailable();
//    }
//
//    fin.close();
//    fout.close();
//    return 0;
//}

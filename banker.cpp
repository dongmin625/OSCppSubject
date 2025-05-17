//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include <queue>
//using namespace std;
//
//// ���� ���� ����
//int n, m; // n: ���μ��� ��, m: �ڿ� ���� ��
//vector<int> totalResources;  // ��ü �ڿ� ��
//vector<int> available;       // ���� ��� ������ �ڿ� ��
//vector<vector<int>> maxMatrix, allocation, need; // MAX, �Ҵ�, �ʿ� �ڿ� ��Ʈ����
//
//ofstream fout("banker.out"); // ����� ����� ����
//
//// ���� ��� ������ �ڿ��� ���
//void printAvailable() {
//    for (int i = 0; i < m; i++) {
//        fout << available[i];
//        if (i != m - 1) fout << " ";
//    }
//    fout << "\n";
//}
//
//// ���� ���°� �������� Ȯ���ϴ� �Լ� (������ �˻�)
//bool isSafeState() {
//    vector<int> work = available; // ���� �ڿ��� work�� ����
//    vector<bool> finish(n, false); // �� ���μ����� �Ϸ� ����
//
//    while (true) {
//        bool progress = false;
//        for (int i = 0; i < n; i++) {
//            if (!finish[i]) {
//                // ���� ���μ����� �ʿ��� �ڿ��� work�� ��� ������ �� �ִ��� Ȯ��
//                bool canFinish = true;
//                for (int j = 0; j < m; j++) {
//                    if (need[i][j] > work[j]) {
//                        canFinish = false;
//                        break;
//                    }
//                }
//
//                // �ش� ���μ����� �ڿ� �䱸 ���� ������ �� �ִٸ�
//                if (canFinish) {
//                    // �ڿ��� �ݳ����� (�Ҵ�� �ڿ��� �ٽ� work�� �߰�)
//                    for (int j = 0; j < m; j++)
//                        work[j] += allocation[i][j];
//                    finish[i] = true;
//                    progress = true;
//                }
//            }
//        }
//        if (!progress) break; // � ���μ����� �� �̻� ������ �� ������ �ݺ� ����
//    }
//
//    // ��� ���μ����� ���� ������ �����̸� ������ ����
//    for (bool done : finish)
//        if (!done) return false;
//    return true;
//}
//
//// �ڿ� ��û ó�� �Լ�
//bool requestResources(int pid, const vector<int>& req) {
//    for (int i = 0; i < m; i++) {
//        if (req[i] > need[pid][i]) return false;     // �ʿ䷮ �ʰ� ��û
//        if (req[i] > available[i]) return false;     // �ڿ��� ������ ���
//    }
//
//    // �ڿ��� �ӽ÷� �Ҵ� (�ùķ��̼�)
//    for (int i = 0; i < m; i++) {
//        available[i] -= req[i];
//        allocation[pid][i] += req[i];
//        need[pid][i] -= req[i];
//    }
//
//    // �� ���°� �������� Ȯ��
//    if (!isSafeState()) {
//        // �������� �ʴٸ� ���� ���·� �ǵ���
//        for (int i = 0; i < m; i++) {
//            available[i] += req[i];
//            allocation[pid][i] -= req[i];
//            need[pid][i] += req[i];
//        }
//        return false;
//    }
//
//    return true; // ��û ����
//}
//
//// �ڿ� �ݳ� �Լ�
//void releaseResources(int pid, const vector<int>& rel) {
//    for (int i = 0; i < m; i++) {
//        allocation[pid][i] -= rel[i]; // �Ҵ�� �ڿ� ����
//        available[i] += rel[i];       // ��� ������ �ڿ� ����
//        need[pid][i] += rel[i];       // �ʿ� �ڿ� ����
//    }
//}
//
//int main() {
//    ifstream fin("banker.inp");
//    if (!fin) {
//        cerr << "banker.inp ������ �� �� �����ϴ�.\n";
//        return 1;
//    }
//
//    string line;
//    getline(fin, line);
//    stringstream ss(line);
//    ss >> n >> m; // ���μ��� ��, �ڿ� ���� �� �б�
//
//    // ��ü �ڿ� �� �Է�
//    totalResources.resize(m);
//    available.resize(m);
//    getline(fin, line);
//    ss.clear(); ss.str(line);
//    for (int i = 0; i < m; i++) {
//        ss >> totalResources[i];
//        available[i] = totalResources[i]; // ó������ ��� ��� ����
//    }
//
//    // MAX, ALLOCATION, NEED �ʱ�ȭ
//    maxMatrix.assign(n, vector<int>(m));
//    allocation.assign(n, vector<int>(m));
//    need.assign(n, vector<int>(m));
//
//    // MAX matrix �Է�
//    int count = 0;
//    while (getline(fin, line)) {
//        if (line.empty()) continue;
//        ss.clear(); ss.str(line);
//        for (int j = 0; j < m; j++) ss >> maxMatrix[count][j];
//        count++;
//        if (count == n) break;
//    }
//
//    // ALLOCATION matrix �Է�
//    count = 0;
//    while (getline(fin, line)) {
//        if (line.empty()) continue;
//        ss.clear(); ss.str(line);
//        for (int j = 0; j < m; j++) ss >> allocation[count][j];
//        count++;
//        if (count == n) break;
//    }
//
//    // NEED ��� �� available ����
//    for (int i = 0; i < n; i++)
//        for (int j = 0; j < m; j++) {
//            need[i][j] = maxMatrix[i][j] - allocation[i][j];
//            available[j] -= allocation[i][j]; // ��� ���� �ڿ���ŭ ����
//        }
//
//    queue<pair<int, vector<int>>> waitQueue; // ��û ���� �� ��ٸ��� ť
//
//    // ��ɾ� ó�� (request, release, quit)
//    while (getline(fin, line)) {
//        if (line.empty()) continue;
//        ss.clear(); ss.str(line);
//        string cmd;
//        ss >> cmd;
//
//        if (cmd == "quit") break; // ���� ���
//
//        int pid;
//        ss >> pid;
//        vector<int> vec(m);
//        for (int i = 0; i < m; i++) ss >> vec[i];
//
//        if (cmd == "request") {
//            // ��û�� �����ϸ� ��� ť�� ����
//            if (!requestResources(pid, vec)) {
//                waitQueue.push({ pid, vec });
//            }
//        }
//        else if (cmd == "release") {
//            releaseResources(pid, vec); // �ڿ� �ݳ�
//
//            // �ڿ� �ݳ� ��, ��� ť Ȯ�� �� �ٽ� ��û �õ�
//            int qsize = waitQueue.size();
//            for (int i = 0; i < qsize; i++) {
//                pair<int, vector<int>> front = waitQueue.front();
//                int wpid = front.first;
//                vector<int> wres = front.second;
//                waitQueue.pop();
//
//                // �ٽ� �õ��ؼ� �����ϸ� �ٽ� ť�� ����
//                if (!requestResources(wpid, wres)) {
//                    waitQueue.push({ wpid, wres });
//                }
//            }
//        }
//
//        // ���� �ڿ� ���� ���
//        printAvailable();
//    }
//
//    fin.close();
//    fout.close();
//    return 0;
//}

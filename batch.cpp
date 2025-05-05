//#include <iostream>
//#include <fstream>
//#include <string>
//
//using namespace std;
//
//
//int main() {
//    
//    ifstream inputFile("batch.inp"); // �Է� ���� ����
//    if (!inputFile) {
//        cerr << "������ �� �� �����ϴ�." << endl;
//        return -1;
//    }
//
//    inputFile.clear();  // EOF �÷��� �ʱ�ȭ (�ʼ�)
//    inputFile.seekg(0, std::ios::beg);  // ������ ó��(0)���� �̵�
//
//    int N = 0; // ���α׷� ����
//
//    //string sLineData;
//
//    //getline(inputFile, sLineData);  // ��ɾ� �� �� �б�
//
//    //N = atoi(sLineData.c_str());
//
//    inputFile >> N;  // ù ��° �ٿ��� ���α׷��� ���� �б�
//    inputFile.ignore();
//    //cerr << N << endl;
//
//    if(!(N >= 3 && N <= 1000)) {
//        cerr << "���α׷� ���� N�� 3 �̻� 1000 ���Ͽ��� �մϴ�." << endl;
//        return -1;
//    }
//
//    int iCPUIdleTime = 0;
//    int iTotTime = 0;
//    int iIndex = 1;
//    int iValue = 0;
//    int iPGCount = 0;
//
//    while(true) {
//        inputFile >> iValue;
//        if (iValue == -1)
//        {
//            inputFile.ignore();
//            iPGCount++;
//            if (iPGCount == N) break;
//            iIndex = 1;
//        }
//        else {
//            if (iIndex++ % 2 == 0) iCPUIdleTime += iValue;
//            iTotTime += iValue;
//        }
//    }
//
//    inputFile.close(); // �Է� ���� �ݱ�
//
//    ofstream outputFile("batch.out"); // ��� ���� ����
//    if (!outputFile) {
//        //cerr << "��� ������ �� �� �����ϴ�." << endl;
//        return -1;
//    }
//
//    // ��� ���Ͽ� ���� �ð��� ���� �ð��� ���
//    outputFile << iCPUIdleTime << " " << iTotTime << endl;
//
//    outputFile.close(); // ��� ���� �ݱ�
//
//    //cerr << "[CPU ��ȿ�ð�] " << iCPUIdleTime << " [��ü�ð�] " << iTotTime << "\r\n" << endl;
//
//    return 0;
//}
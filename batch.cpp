//#include <iostream>
//#include <fstream>
//#include <string>
//
//using namespace std;
//
//
//int main() {
//    
//    ifstream inputFile("batch.inp"); // 입력 파일 열기
//    if (!inputFile) {
//        cerr << "파일을 열 수 없습니다." << endl;
//        return -1;
//    }
//
//    inputFile.clear();  // EOF 플래그 초기화 (필수)
//    inputFile.seekg(0, std::ios::beg);  // 파일의 처음(0)으로 이동
//
//    int N = 0; // 프로그램 개수
//
//    //string sLineData;
//
//    //getline(inputFile, sLineData);  // 명령어 한 줄 읽기
//
//    //N = atoi(sLineData.c_str());
//
//    inputFile >> N;  // 첫 번째 줄에서 프로그램의 개수 읽기
//    inputFile.ignore();
//    //cerr << N << endl;
//
//    if(!(N >= 3 && N <= 1000)) {
//        cerr << "프로그램 개수 N은 3 이상 1000 이하여야 합니다." << endl;
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
//    inputFile.close(); // 입력 파일 닫기
//
//    ofstream outputFile("batch.out"); // 출력 파일 열기
//    if (!outputFile) {
//        //cerr << "출력 파일을 열 수 없습니다." << endl;
//        return -1;
//    }
//
//    // 결과 파일에 유휴 시간과 종료 시간을 출력
//    outputFile << iCPUIdleTime << " " << iTotTime << endl;
//
//    outputFile.close(); // 출력 파일 닫기
//
//    //cerr << "[CPU 유효시간] " << iCPUIdleTime << " [전체시간] " << iTotTime << "\r\n" << endl;
//
//    return 0;
//}
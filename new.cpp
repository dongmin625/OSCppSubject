//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <queue>
//#include <climits>
//#include <Windows.h>
//
//using namespace std;
//
//enum State {TstartTime,Tcpu,Tio };
//
//struct ProcessData {
//	int iType = 0;
//	int iValue =0;
//};
//
//
//vector<vector<ProcessData>> myVector;
//
//int main() {
//
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
//    ProcessData stpData, stpData2;
//    int iMaxCpu = 0;
//
//    int processesEnd[1000] = { 0, }; // N ���� ���μ���
//    int processesRunEnableTime[1000] = { 0, }; // N ���� ���μ���
//    int processesPushtime[1000] = { 0, };
//    myVector.resize(N+1);
//    for (int i = 0; i < N; ++i) {
//        // ���۽ð�
//        stpData.iType = TstartTime;
//        inputFile >> stpData.iValue; // ù ��° ���� ���� ��
//        myVector[i].push_back(stpData);
//        while (true) {
//            // ���۽ð�
//            stpData.iType = Tcpu;
//            inputFile >> stpData.iValue; // ù ��° ���� ���� ��
//            if (stpData.iValue > iMaxCpu) iMaxCpu = stpData.iValue;
//            if (stpData.iValue == -1)
//            {
//                break;
//            }
//            myVector[i].push_back(stpData);
//            stpData.iType = Tio;
//            inputFile >> stpData.iValue;
//            if (stpData.iValue == -1)
//            {
//                break;
//            }
//            myVector[i].push_back(stpData);
//        }
//    }
//    inputFile.close();
//
//    int iStartTime = 0;
//    int iRunTime = 0;
//    int iRunProcess = 0;
//    int iRunCpuTime = 0;
//    int i = 0;
//    bool bFind = false;
//    int iIdleTotTime = 0;
//    int iPreRunTime = 0;
//    int iIdleTime = 0;
//    char cImsi[1000];
//    int iCurrTime = 1;
//    bool bRunCpu = false;
//    while (true) {
//        iRunCpuTime = iMaxCpu + 1;
//        bFind = false;
//
//        for (i = 0; i < N; ++i) {
//            //ù��° ���� �ð� ���
//            if (myVector[i].size() == 0) continue;
//
//            stpData = myVector[i].front();
//            switch (stpData.iType)
//            {
//            case TstartTime:
//                if (iCurrTime >= stpData.iValue)
//                {
//                    if(!bRunCpu) processesEnd[i] = stpData.iValue;
//                    myVector[i].erase(myVector[i].begin());
//                }
//                break;
//            case Tcpu:
//                if (stpData.iValue <= iRunCpuTime &&
//                    processesEnd[i] <= iCurrTime)
//                {
//                    if (bRunCpu && stpData.iValue == myVector[iRunProcess].front().iValue)
//                    {
//                        if(processesEnd[iRunProcess] > processesEnd[i]) iRunProcess = i;
//                    }else iRunProcess = i;
//                    bRunCpu = true;
//                }
//                break;
//            case Tio:
//                processesEnd[i] += stpData.iValue;
//                myVector[i].erase(myVector[i].begin());
//                break;
//            default:
//                break;
//            }
//        }
//
//
//        if (bRunCpu)
//        {
//            iIdleTime = iRunTime - iPreRunTime;
//            iCurrTime += myVector[iRunProcess].front().iValue;
//             // ���: ���� ���� ���� ���μ����� idleTime
//           
//             cerr << "Time= " << iRunTime << " selectedProcess= " << iRunProcess << " idleTime= " << iIdleTime << endl;
//      
//             sprintf(cImsi, "Time = %d  selectedProcess =  %d  cpu = %d \n", iRunTime, iRunProcess, myVector[iRunProcess].front().iValue);
//
//             OutputDebugStringA(cImsi);
//
//            bRunCpu = false;
//           
//            processesEnd[iRunProcess] += myVector[iRunProcess].front().iValue;
//            iIdleTotTime += iIdleTime;
//            myVector[iRunProcess].erase(myVector[iRunProcess].begin());
//        }else   iCurrTime++;
//
//
//        bool bExit = true;
//        for (i = 0; i < N; i++)
//        {
//            if (myVector[i].size() > 0)
//            {
//                bExit = false;
//                break;
//            }
//        }
//        if (bExit) 
//            break;
//    }
//
//}
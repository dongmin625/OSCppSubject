#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

using namespace std;

int main() {
    // DOS → Unix 매핑
    unordered_map<string, string> dosToUnix;
    dosToUnix["dir"] = "ls";        // DOS: dir -> Unix: ls
    dosToUnix["md"] = "mkdir";      // DOS: md -> Unix: mkdir
    dosToUnix["rd"] = "rmdir";      // DOS: rd -> Unix: rmdir
    dosToUnix["del"] = "rm";        // DOS: del -> Unix: rm
    dosToUnix["copy"] = "cp";       // DOS: copy -> Unix: cp
    dosToUnix["rename"] = "mv";     // DOS: rename -> Unix: mv
    dosToUnix["cls"] = "clear";     // DOS: cls -> Unix: clear
    dosToUnix["cd"] = "pwd";        // DOS: cd -> Unix: pwd
    dosToUnix["type"] = "cat";      // DOS: type -> Unix: cat
    dosToUnix["help"] = "man";      // DOS: help -> Unix: man
    dosToUnix["time"] = "date";     // DOS: time -> Unix: date
    dosToUnix["find"] = "find";     // DOS: find -> Unix: find
    dosToUnix["findstr"] = "grep"; // DOS: findstr -> Unix: grep
    dosToUnix["more"] = "more";     // DOS: more -> Unix: more
    dosToUnix["comp"] = "diff";     // DOS: comp -> Unix: diff
    dosToUnix["edlin"] = "ed";     // DOS: edlin -> Unix: ed
    dosToUnix["sort"] = "sort";     // DOS: sort -> Unix: sort
    dosToUnix["attrib"] = "lsattr"; // DOS: attrib -> Unix: lsattr
    dosToUnix["pushd"] = "pushd";  // DOS: pushd -> Unix: pushd
    dosToUnix["popd"] = "popd";    // DOS: popd -> Unix: popd
    dosToUnix["taskmgr"] = "ps";   // DOS: taskmgr -> Unix: ps
    dosToUnix["tskill"] = "kill";  // DOS: tskill -> Unix: kill
    dosToUnix["shutdown"] = "halt";// DOS: shutdown -> Unix: halt
    dosToUnix["ipconfig"] = "ifconfig";  // DOS: ipconfig -> Unix: ifconfig
    dosToUnix["chkdsk"] = "fsck";  // DOS: chkdsk -> Unix: fsck
    dosToUnix["mem"] = "free";     // DOS: mem -> Unix: free
    dosToUnix["scandisk"] = "debugfs"; // DOS: scandisk -> Unix: debugfs
    dosToUnix["print"] = "lpr";    // DOS: print -> Unix: lpr


    // Unix → DOS 매핑
    unordered_map<string, string> unixToDos;
    unixToDos["ls"] = "dir";
    unixToDos["mkdir"] = "md";
    unixToDos["rmdir"] = "rd";
    unixToDos["rm"] = "del";
    unixToDos["cp"] = "copy";
    unixToDos["mv"] = "rename";
    unixToDos["clear"] = "cls";
    unixToDos["pwd"] = "cd";
    unixToDos["cat"] = "type";
    unixToDos["man"] = "help";
    unixToDos["date"] = "time";
    unixToDos["find"] = "find";
    unixToDos["grep"] = "findstr";
    unixToDos["more"] = "more";
    unixToDos["diff"] = "comp";
    unixToDos["ed"] = "edlin";
    unixToDos["sort"] = "sort";
    unixToDos["lsattr"] = "attrib";
    unixToDos["pushd"] = "pushd";
    unixToDos["popd"] = "popd";
    unixToDos["ps"] = "taskmgr";
    unixToDos["kill"] = "tskill";
    unixToDos["halt"] = "shutdown";
    unixToDos["ifconfig"] = "ipconfig";
    unixToDos["fsck"] = "chkdsk";
    unixToDos["free"] = "mem";
    unixToDos["debugfs"] = "scandisk";
    unixToDos["lpr"] = "print";

    // 입력 파일 열기
    ifstream inputFile("command.inp");
    if (!inputFile) {
        cerr << "Error opening input file!" << endl;
        return 1;
    }

    // 출력 파일 열기
    ofstream outputFile("command.out");
    if (!outputFile) {
        cerr << "Error opening output file!" << endl;
        return 1;
    }

    int n;
    inputFile >> n; // 첫 번째 줄에서 명령어의 개수 읽기
    inputFile.ignore(); // 첫 번째 줄의 개행 문자 무시

    string command;
    // 주어진 개수만큼 명령어 처리
    for (int i = 0; i < n; ++i) {
        getline(inputFile, command);  // 명령어 한 줄 읽기

        // DOS 명령어 → Unix 명령어
        if (dosToUnix.find(command) != dosToUnix.end()) {
            outputFile << command << " -> " << dosToUnix[command] << endl;
        }
        // Unix 명령어 → DOS 명령어
        else if (unixToDos.find(command) != unixToDos.end()) {
            outputFile << command << " -> " << unixToDos[command] << endl;
        }
        else {
            outputFile << "Command not found!" << endl;
        }
    }

    // 파일 닫기
    inputFile.close();
    outputFile.close();

    return 0;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

class Person {
public:
    int id;
    string firstName;
    string lastName;

    Person(int id, string firstName, string lastName)
        : id(id), firstName(firstName), lastName(lastName) {}
};

int main() {
    vector<Person> people;
    map<string, int> lastNameCount;
    int maxFirstNameLength = 0;
    int maxLastNameLength = 0;

    ifstream inputFile("test.inp");
    if (!inputFile) {
        cerr << "파일을 읽는 중 오류 발생" << endl;
        return 1;
    }

    int n;
    inputFile >> n;
    inputFile.ignore();  // 개행 문자 제거

    for (int i = 0; i < n; i++) {
        int id;
        string firstName, lastName;
        inputFile >> id >> firstName >> lastName;

        if (firstName.length() >= 30 || lastName.length() >= 30) {
            cerr << "이름의 길이가 맞지 않음" << endl;
            return 1;
        }

        people.emplace_back(id, firstName, lastName);
        maxFirstNameLength = max(maxFirstNameLength, (int)firstName.length());
        maxLastNameLength = max(maxLastNameLength, (int)lastName.length());
        lastNameCount[lastName]++;
    }
    inputFile.close();

    // 고유번호 오름차순 정렬
    sort(people.begin(), people.end(), [](const Person& a, const Person& b) {
        return a.id < b.id;
        });

    // 동일한 Last name이 없는 경우 처리
    bool allLastNamesUnique = true;
    for (const auto& entry : lastNameCount) {
        if (entry.second > 1) {
            allLastNamesUnique = false;
            break;
        }
    }

    // 모든 Last name이 서로 다르면 바로 종료
    if (allLastNamesUnique) {
        ofstream outputFile("test.out");
        if (!outputFile) {
            cerr << "test.out 파일을 생성할 수 없습니다!" << endl;
            return 1;
        }

        for (const auto& p : people) {
            // ID를 5자리로 변환하여 문자열로 저장
            ostringstream idStream;
            idStream << setw(5) << setfill('0') << p.id;

            outputFile << idStream.str() << " ";  // 변환된 ID 출력

            // First name과 Last name 출력
            outputFile << left << setw(maxFirstNameLength) << setfill(' ') << p.firstName << " ";
            outputFile << left << setw(maxLastNameLength) << setfill(' ') << p.lastName << endl;
        }


    // 동일한 Last name이 있는 경우 처리
    ofstream outputFile("test.out");
    if (!outputFile) {
        cerr << "test.out 파일을 생성할 수 없습니다!" << endl;
        return 1;
    }

    for (const auto& p : people) {
        // ID를 5자리로 변환하여 문자열로 저장
        ostringstream idStream;
        idStream << setw(5) << setfill('0') << p.id;

        outputFile << idStream.str() << " ";  // 변환된 ID 출력

        // First name과 Last name 출력
        outputFile << left << setw(maxFirstNameLength) << setfill(' ') << p.firstName << " ";
        outputFile << left << setw(maxLastNameLength) << setfill(' ') << p.lastName << endl;
    }

    outputFile << endl;  // 빈 줄 추가

    // 동일한 Last name이 있는 사람들의 정보를 출력
    for (const auto& entry : lastNameCount) {
        if (entry.second > 1) {
            outputFile << entry.first << " " << entry.second << endl;
        }
    }

    outputFile.close();
    return 0;
}

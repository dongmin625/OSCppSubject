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
        cerr << "������ �д� �� ���� �߻�" << endl;
        return 1;
    }

    int n;
    inputFile >> n;
    inputFile.ignore();  // ���� ���� ����

    for (int i = 0; i < n; i++) {
        int id;
        string firstName, lastName;
        inputFile >> id >> firstName >> lastName;

        if (firstName.length() >= 30 || lastName.length() >= 30) {
            cerr << "�̸��� ���̰� ���� ����" << endl;
            return 1;
        }

        people.emplace_back(id, firstName, lastName);
        maxFirstNameLength = max(maxFirstNameLength, (int)firstName.length());
        maxLastNameLength = max(maxLastNameLength, (int)lastName.length());
        lastNameCount[lastName]++;
    }
    inputFile.close();

    // ������ȣ �������� ����
    sort(people.begin(), people.end(), [](const Person& a, const Person& b) {
        return a.id < b.id;
        });

    // ������ Last name�� ���� ��� ó��
    bool allLastNamesUnique = true;
    for (const auto& entry : lastNameCount) {
        if (entry.second > 1) {
            allLastNamesUnique = false;
            break;
        }
    }

    // ��� Last name�� ���� �ٸ��� �ٷ� ����
    if (allLastNamesUnique) {
        ofstream outputFile("test.out");
        if (!outputFile) {
            cerr << "test.out ������ ������ �� �����ϴ�!" << endl;
            return 1;
        }

        for (const auto& p : people) {
            // ID�� 5�ڸ��� ��ȯ�Ͽ� ���ڿ��� ����
            ostringstream idStream;
            idStream << setw(5) << setfill('0') << p.id;

            outputFile << idStream.str() << " ";  // ��ȯ�� ID ���

            // First name�� Last name ���
            outputFile << left << setw(maxFirstNameLength) << setfill(' ') << p.firstName << " ";
            outputFile << left << setw(maxLastNameLength) << setfill(' ') << p.lastName << endl;
        }


    // ������ Last name�� �ִ� ��� ó��
    ofstream outputFile("test.out");
    if (!outputFile) {
        cerr << "test.out ������ ������ �� �����ϴ�!" << endl;
        return 1;
    }

    for (const auto& p : people) {
        // ID�� 5�ڸ��� ��ȯ�Ͽ� ���ڿ��� ����
        ostringstream idStream;
        idStream << setw(5) << setfill('0') << p.id;

        outputFile << idStream.str() << " ";  // ��ȯ�� ID ���

        // First name�� Last name ���
        outputFile << left << setw(maxFirstNameLength) << setfill(' ') << p.firstName << " ";
        outputFile << left << setw(maxLastNameLength) << setfill(' ') << p.lastName << endl;
    }

    outputFile << endl;  // �� �� �߰�

    // ������ Last name�� �ִ� ������� ������ ���
    for (const auto& entry : lastNameCount) {
        if (entry.second > 1) {
            outputFile << entry.first << " " << entry.second << endl;
        }
    }

    outputFile.close();
    return 0;
}

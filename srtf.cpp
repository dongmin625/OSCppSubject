#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <utility> // pair ����� ���� ����
#include <algorithm> // sort, min ����� ���� ����
#include <limits> // numeric_limits ����� ���� ����

using namespace std;

// Task ����ü (��κ� ���� ����)
struct Task {
    int pid; // ���μ��� ID
    int arrival_time; // ���� �ð�
    vector<int> bursts; // ����Ʈ �ð� �迭 (���� �ð�, CPU ����Ʈ, I/O ����Ʈ, ...)
    int burst_index = 1; // ���� ó���ؾ� �� ����Ʈ �ε��� (bursts[0]�� ���� �ð��̹Ƿ� ù CPU ����Ʈ���� ����)
    int remaining = 0;   // ���� CPU ����Ʈ�� ���� �ð�
    int last_ready_time = 0; // ���������� �غ� ť�� �� �ð�
    int end_time = 0;      // �½�ũ�� ������ �Ϸ��� �ð�

    // ù ��° ����Ʈ�� ���� �ð� �ʱ�ȭ
    void initialize_first_burst() {
        if (bursts.size() > 1) { // CPU ����Ʈ�� �ϳ� �̻� �ִ��� Ȯ��
            remaining = bursts[1]; // ù ��° CPU ����Ʈ �ð����� remaining ����
        }
        else {
            remaining = 0; // ��ȿ�� �Է�(-1�� ����)������ �߻����� �ʾƾ� ��
        }
    }
};

// �غ� ť ���� (���� �ð� -> ������ �غ� �ð� -> PID ������ �ּ� ��)
struct CompareReady {
    bool operator()(Task* a, Task* b) {
        if (a->remaining != b->remaining)
            return a->remaining > b->remaining; // ���� �ð��� ª�� �� �켱
        if (a->last_ready_time != b->last_ready_time)
            return a->last_ready_time > b->last_ready_time; // ���� �غ�� �� �켱 (FIFO Ÿ�� �극��ũ)
        return a->pid > b->pid; // PID�� ���� �� �켱 (���� Ÿ�� �극��ũ)
    }
};

// I/O ��� ť ���� (I/O �Ϸ� �ð� ���� �ּ� ��)
struct CompareIO {
    // Pair: {io_completion_time, task_pointer} (I/O �Ϸ� �ð�, �½�ũ ������)
    bool operator()(const pair<int, Task*>& a, const pair<int, Task*>& b) {
        return a.first > b.first; // �Ϸ� �ð��� ���� �� �켱
    }
};

int main() {
    // KR: �λ걤���� �ð��� ��� (KST, UTC+9)
    // ����: C++ ǥ�� ���̺귯�������δ� �ý��� �ð��븦 ���� ����/����ϱ� ����쳪, ���� ��ü�� �ð���� �����մϴ�.
    //       ���� �ð�(2025�� 5�� 5�� ���� 9:31:57 KST) ������ �ڵ� ���� ������ ������ ���� �ʽ��ϴ�.
    ifstream fin("srtf.inp");
    ofstream fout("srtf.out");

    if (!fin.is_open()) {
        cerr << "�Է� ���� srtf.inp ���� ����" << endl;
        return 1;
    }
    if (!fout.is_open()) {
        cerr << "��� ���� srtf.out ���� ����" << endl;
        return 1;
    }

    int n; // ���μ��� ����
    fin >> n;
    vector<Task> processes(n); // ���μ��� ��ü ���� ����
    vector<Task*> process_ptrs(n); // ���� ������ ���� ������ ����

    for (int i = 0; i < n; ++i) {
        processes[i].pid = i; // PID ����
        process_ptrs[i] = &processes[i]; // ������ ����
        int t;
        // ���� �ð� ���� �б�
        if (!(fin >> t)) break; // ������ �б� ���� ó��
        processes[i].bursts.push_back(t); // ����Ʈ �迭�� �߰�
        processes[i].arrival_time = t; // ���� �ð� ����

        // -1�� ���� ������ ������ ����Ʈ �б�
        while (fin >> t && t != -1) {
            processes[i].bursts.push_back(t);
        }
        processes[i].initialize_first_burst(); // ù CPU ����Ʈ�� ���� �ð� ����
    }

    // ���� ���� ���μ����� ���� ã�� ���� ���� �ð� ������ ����
    sort(process_ptrs.begin(), process_ptrs.end(), [](Task* a, Task* b) {
        return a->arrival_time < b->arrival_time;
        });

    int current_time = 0; // ���� �ùķ��̼� �ð�
    int completed_count = 0; // �Ϸ�� ���μ��� ����
    long long total_idle_time = 0; // ���������� ū ���� �ð��� ���� long long ���
    Task* current_running_task = nullptr; // ���� CPU���� ���� ���� �½�ũ
    int next_arrival_idx = 0; // ���� ������ ���μ����� �ε��� (���ĵ� ������ �迭 ����)

    // �غ� ť: CPU���� ���� �غ� �� �½�ũ��
    priority_queue<Task*, vector<Task*>, CompareReady> ready_queue;

    // I/O ��� ť: ���� I/O �۾��� ���� ���� �½�ũ��
    // {io_completion_time, task_pointer} ���� ����
    priority_queue<pair<int, Task*>, vector<pair<int, Task*>>, CompareIO> io_wait_queue;

    // �ùķ��̼��� ������ ���� ���� ���� �ð� ã��
    if (n > 0 && next_arrival_idx < n) {
        // ���� �ð��� ù ��° ���� �ð��̾�� ��
        // *** �߿�: �ʱ� ���� �ð� ��� ������. ù ���� �ð��� 0���� ũ�� �� ���̸�ŭ ���� �ð� �߻�. ***
        // ���� �亯���� total_idle_time ��� �߰� ���ȵ�. ���⼭�� ���� ����.
        current_time = process_ptrs[next_arrival_idx]->arrival_time;
    }
    else {
        fout << 0 << "\n"; // ���μ����� ������ ���� �ð��� 0
        fout.close();      // ��ȯ ���� ���� �ݱ�
        fin.close();
        return 0; // ���μ����� ������ ����
    }


    while (completed_count < n) { // ��� ���μ����� �Ϸ�� ������ �ݺ�
        // 1. ���� ������ ���μ��� (current_time ������ ����)�� �غ� ť�� �߰�
        while (next_arrival_idx < n && process_ptrs[next_arrival_idx]->arrival_time <= current_time) {
            if (process_ptrs[next_arrival_idx]->bursts.size() > 1) { // CPU ����Ʈ�� �ϳ� �̻� �ִ��� Ȯ��
                // ��� �����ߴٸ� �غ� �ð��� ���� �ð����� ����
                process_ptrs[next_arrival_idx]->last_ready_time = process_ptrs[next_arrival_idx]->arrival_time;
                ready_queue.push(process_ptrs[next_arrival_idx]); // �غ� ť�� �߰�
            }
            else {
                // ���μ����� �������ڸ��� ��� ���� (���� �ð� ����Ʈ�� �ְ� ���� ����Ʈ�� ����)
                process_ptrs[next_arrival_idx]->end_time = process_ptrs[next_arrival_idx]->arrival_time; // ���� �� �Ϸ�
                completed_count++;
            }
            next_arrival_idx++; // ���� ���� ���� ���μ��� �ε��� ����
        }

        // 2. ���� �ð�(current_time) �Ǵ� �� ������ I/O �Ϸ�� �½�ũ Ȯ��
        //    �Ϸ� �ð��� current_time ������ ��� �̺�Ʈ�� ó��
        // ================== ������ ��� ���� ==================
        while (!io_wait_queue.empty() && io_wait_queue.top().first <= current_time) {
            Task* task_finished_io = io_wait_queue.top().second; // I/O �Ϸ�� �½�ũ ������
            // ��Ȯ���� ���� ť���� ���� �Ϸ� �ð� ���
            int io_actual_finish_time = io_wait_queue.top().first; // ���� I/O �Ϸ� �ð�
            io_wait_queue.pop(); // I/O ��� ť���� ����

            task_finished_io->burst_index++; // �Ϸ�� I/O ���� ����Ʈ�� ����Ű���� �ε��� ����

            if (task_finished_io->burst_index < task_finished_io->bursts.size()) {
                // ���� ����Ʈ�� �� ����. ��ȿ�� �Է��̶�� ������ CPU ����Ʈ.
                task_finished_io->remaining = task_finished_io->bursts[task_finished_io->burst_index]; // ���� CPU ����Ʈ �ð����� remaining ����
                // �½�ũ�� I/O�� �Ϸ�� ����*�� �غ� ���°� ��
                task_finished_io->last_ready_time = io_actual_finish_time; // �غ� �ð� ����
                ready_queue.push(task_finished_io); // �غ� ť�� �߰�
            }
            else {
                // ��� �Ϸ�� I/O ����Ʈ�� �� �½�ũ�� *������* ����Ʈ����.
                // �½�ũ�� I/O�� �Ϸ�� ����*�� �Ϸ�� ������ ǥ��.
                task_finished_io->end_time = io_actual_finish_time; // ���� �ð� ���
                completed_count++; // �Ϸ�� �½�ũ �� ����
                // �̰��� ��ȿ�� �Ϸ� �ó������̹Ƿ� ���� �޽��� �ʿ� ����.
            }
        }
        // =================== ������ ��� �� ===================


        // 3. ������ ������ �½�ũ ���� (SRTF ���� Ȯ��)
        if (current_running_task) { // ���� ���� ���� �½�ũ�� �ִٸ�
            // �� ���� �켱������ �½�ũ(���� �ð� ª�ų�, ������ �غ� �ð� �����ų�/PID ����)�� �غ� ť�� �ִٸ� ����
            if (!ready_queue.empty() && CompareReady()(current_running_task, ready_queue.top())) {
                // ���� �½�ũ�� �غ� ť�� �ٽ� ����
                // **�߿�**: last_ready_time�� ������ �ٽ� �غ� ���°� �� ���� �����ؾ� ��.
                // ���⼭�� �����Ǵ� ������, ����Ʈ�� �Ϸ��ϴ� ���� �ƴ�. last_ready_time��
                // ���������� �غ� ���°� �Ǿ��� �ð�(���� �Ǵ� I/O �Ϸ� ����)�� �����ؾ� ��.
                // ���⼭ current_running_task->last_ready_time ���� �ʿ� ����.
                ready_queue.push(current_running_task);
                // ���ο� �ְ� �켱���� �½�ũ�� ������
                current_running_task = ready_queue.top();
                ready_queue.pop(); // �غ� ť���� ����
            }
        }
        // ���� ���� �½�ũ�� ���ų� ���� �½�ũ�� ����/�Ϸ�� ���:
        if (!current_running_task) {
            // CPU�� ���� �����̸� �غ� ť���� ���� ���� �켱������ �½�ũ�� ����
            if (!ready_queue.empty()) {
                current_running_task = ready_queue.top(); // ������ �½�ũ ����
                ready_queue.pop(); // �غ� ť���� ����
            }
        }

        // 4. ���� �̺�Ʈ������ �ð� ���
        int time_to_next_event = numeric_limits<int>::max(); // ���� �̺�Ʈ���� ���� �ð� (�ִ밪���� �ʱ�ȭ)
        bool is_idle_period = false; // ���� �Ⱓ�� ���� �ð����� �����ϴ� �÷���

        // �̺�Ʈ: ���� ���μ��� ����
        if (next_arrival_idx < n) { // ���� ������ ���μ����� ���Ҵٸ�
            // �̷��� ������ ����ϵ��� ��
            if (process_ptrs[next_arrival_idx]->arrival_time > current_time) {
                time_to_next_event = min(time_to_next_event, process_ptrs[next_arrival_idx]->arrival_time - current_time); // ���� �ð� ���
            }
            else {
                // �̹� ó���Ǿ���� ������, ��Ÿ ���� �� 0�� �� �� ����
                // ������ ���� �Ǵ� �����̸�, �� �̺�Ʈ�� ���� �ð� ��Ÿ�� ��ǻ� 0
                time_to_next_event = min(time_to_next_event, 0);
            }
        }

        // �̺�Ʈ: ���� CPU ����Ʈ �Ϸ�
        if (current_running_task) { // ���� ���� �½�ũ�� �ִٸ�
            if (current_running_task->remaining <= 0) {
                // �ð� ���� ���� �½�ũ�� ��Ȯ�� current_time�� ������ ��� �߻� ����
                time_to_next_event = min(time_to_next_event, 0);
            }
            else {
                time_to_next_event = min(time_to_next_event, current_running_task->remaining); // ���� CPU ����Ʈ �ð�
            }
        }

        // �̺�Ʈ: ���� I/O �Ϸ�
        if (!io_wait_queue.empty()) { // I/O ��� ���� �½�ũ�� �ִٸ�
            // �̷��� �ϷḸ ����ϵ��� ��
            if (io_wait_queue.top().first > current_time) {
                time_to_next_event = min(time_to_next_event, io_wait_queue.top().first - current_time); // ���� �ð� ���
            }
            else {
                // IO �Ϸᰡ ���� �Ǵ� �����̸�, �̺�Ʈ �ð� ��Ÿ�� ��ǻ� 0
                time_to_next_event = min(time_to_next_event, 0);
            }
        }

        // ��� ���� �̺�Ʈ�� ���� ����/����� ���� ���ٸ�, �Ϸ�Ǿ��ų� ���� �߸��� ��.
        if (time_to_next_event == numeric_limits<int>::max() && !current_running_task && ready_queue.empty() && io_wait_queue.empty() && next_arrival_idx == n) {
            if (completed_count < n) {
                // �ùٸ� �ùķ��̼� �� ��ȿ�� �Է¿����� �߻����� �ʾƾ� ��
                cerr << "���: �ùķ��̼��� ���⿡ ����Ǿ����ϴ�. ������ Ȯ���ϼ���." << endl;
            }
            break; // �ùķ��̼� ���� ����
        }

        // time_to_next_event�� 0�̸� �̺�Ʈ�� ��Ȯ�� ���� �߻��Ѵٴ� �ǹ�.
        // ���� �̺�Ʈ�� ó���ؾ� �� (���� I/O �Ϸ� ���� ��)
        // �׸��� �ٸ� �̷� �̺�Ʈ�� �� ���� �ܰ踦 �������� �ʴ´ٸ� ���������� �ð��� 1��ŭ �����ؾ� ��.
        // �׷��� ����/IO ������ current_time �̺�Ʈ�� ó����.
        // �ֵ� ������ *�̷�* �̺�Ʈ�� ������ �ý����� ������ ���� ��� �߻�.
        // ���� �ð� ���� �ð� ������ ������ ���ô�.

        // ��Ÿ�� �ִ밪�̸�, ã�� �̷� �̺�Ʈ�� ���ٴ� �ǹ�. ����.
        if (time_to_next_event == numeric_limits<int>::max()) {
            break;
        }

        // �ð��� ����ǵ��� �ϰ�, 0 ��Ÿ ���̽��� �����ϰ� ó��.
        // ��Ÿ�� 0�̸� �̺�Ʈ�� ���� �߻�. ������ ó���ϵ��� ��. ó�� *�Ŀ�* �ð��� �����ؾ� ���� �����ؾ� ��.
        // ���� 0 ��Ÿ�� ó�� �������� �� �̺�Ʈ�� �߰��ǰų� ���°� ����Ǿ� ���� �ݺ������� 0�� �ƴ� ��Ÿ�� ���� �� ����.
        // ���� ��Ÿ ����� ��� 0���� �̾����ٸ�, 1��ŭ ���� ����? �̺�Ʈ�� �ǳʶ� �� ����.
        // �� ���� ���: ��Ÿ�� 0�̸� ó��; ó�� �Ŀ��� ������ 0�̸� �ʿ� �� 1��ŭ ����?.
        // ������ ���� �ð� �ǳʶٱ�� ��Ÿ > 0�� ���� ����. 0 ��Ÿ�� ������ �ٽ� ���鼭 �Ϲ������� ó��.

        if (time_to_next_event < 0) { // ���� ��Ÿ ���� Ȯ��
            // ���� ������ ��Ÿ��, current_time�� �ʹ� ���� ����Ǿ��� �� ����?
            cerr << "����: ���� �ð� ��Ÿ�� ���Ǿ����ϴ�." << endl;
            time_to_next_event = 1; // ���� ��ġ (�����δ� ���� ���� ���� �ʿ�)
        }

        // �ٰ����� �Ⱓ�� ���� �������� ����
        is_idle_period = (!current_running_task && ready_queue.empty()); // ���� ���� ���� �½�ũ ����, �غ� ť�� ��������� ����

        // �� �ܰ迡 ���� ��� �ð� ���
        int time_elapsed = time_to_next_event; // ��� �ð� = ���� �̺�Ʈ������ �ð�

        // time_elapsed�� 0�� �� ó�� (���� ������ �ּ� �� ������ �ѱ�ȭ �� ����)
        if (time_elapsed == 0) {
            // �̺�Ʈ�� ���� �ð��� �߻����� �ǹ�.
            // ������ ����/I/O ó�� ������ �� �̺�Ʈ���� ó���� ����.
            // ���⼭ 'current_time'�� �����Ű�� ����.
            // ���� �ð��� �߰����� ���� (�ð��� �帣�� �ʾ����Ƿ�).
            // ���� ����(��: �۾� �Ϸ�)�� �ش� �̺�Ʈ ó�� ��Ͽ��� �߻�.
            // ������ ��ӵǾ� ���¸� ������.
        }
        else { // time_elapsed > 0�� ��
            if (is_idle_period) { // CPU�� ���� �����̰� �ð��� ������ ����� ����
                total_idle_time += time_elapsed; // ���� �ð� ����
            }
            if (current_running_task) { // ���� ���� �½�ũ�� �ִٸ�
                current_running_task->remaining -= time_elapsed; // ���� �ð� ����
                // ���� �ð��� 0 ���ϰ� �� ����� ó���� �Ʒ� 5�� �ܰ迡�� ����
            }
            current_time += time_elapsed; // �ùķ��̼� �ð� ����
        }


        // 5. CPU ����Ʈ �Ϸ� ó�� (**���ο� current_time�� ��Ȯ�� �߻��� ���**)
        if (current_running_task && current_running_task->remaining <= 0) {
            // ����: �ð��� �̹� �Ϸ� ���� (�Ǵ� �ٸ� �̺�Ʈ ����)���� ����Ǿ���.
            // ���� �ð� Ȯ���� �Ϸᰡ 'current_time'�� �߻������� Ȯ��.

            current_running_task->burst_index++; // ���� ����Ʈ(I/O �Ǵ� ����)�� �̵�

            if (current_running_task->burst_index < current_running_task->bursts.size()) { // ���� ����Ʈ�� �ִٸ�
                // ���� ����Ʈ�� I/O (�ε����� ¦��: 2, 4, 6...)
                int io_time = current_running_task->bursts[current_running_task->burst_index]; // I/O �ð�
                if (io_time > 0) { // I/O �ð��� ����� ���� ť�� �߰�
                    // I/O�� *����* (current_time) ����
                    io_wait_queue.push({ current_time + io_time, current_running_task }); // I/O �Ϸ� �ð��� �Բ� ť�� �߰�
                }
                else { // I/O �ð��� 0�̸� ��� ���� CPU �غ� / ���� ���°� �Ǿ�� ��
                    current_running_task->burst_index++; // ���� ������ CPU ����Ʈ �ε����� �̵�
                    if (current_running_task->burst_index < current_running_task->bursts.size()) { // ���� CPU ����Ʈ�� �ִٸ�
                        current_running_task->remaining = current_running_task->bursts[current_running_task->burst_index]; // ���� �ð� ����
                        current_running_task->last_ready_time = current_time; // ���� �غ� ���°� ��
                        ready_queue.push(current_running_task); // �غ� ť�� �߰�
                    }
                    else { // 0 �ð� I/O �Ŀ� ���� (CPU ����Ʈ ����)
                        current_running_task->end_time = current_time; // ���� �ð��� CPU ����Ʈ (�� 0-IO) �Ϸ� ����
                        completed_count++; // �Ϸ� ī��Ʈ ����
                    }
                }
            }
            else { // ��� ����Ʈ �Ϸ� (CPU�� �������̾���)
                current_running_task->end_time = current_time; // ���� �ð��� CPU ����Ʈ �Ϸ� ����
                completed_count++; // �Ϸ� ī��Ʈ ����
            }
            current_running_task = nullptr; // CPU�� ��� ��
        }
        // ������ �Ϸ� ó�� ��, CPU�� ����ٸ� �غ� ť�� �ٽ� Ȯ���� �ʿ�� ����
        // �� Ȯ���� �̺�Ʈ ó�� �� ���� ���� �κ��� 3�ܰ�� �̵��Ǿ���.

    } // while(completed_count < n) ���� ��

    // ��� ���
    fout << total_idle_time << "\n"; // �� ���� �ð� ���

    // ���� PID ������� ���� �ð� ���
    vector<int> end_times(n);
    for (int i = 0; i < n; ++i) {
        // pid 'i'�� ���� ���� ���μ����� ã�� ���� �ð��� �ùٸ��� ����
        for (int j = 0; j < n; ++j) {
            if (processes[j].pid == i) {
                end_times[i] = processes[j].end_time;
                break; // ã������ ���� ���� ����
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        fout << end_times[i] << (i == n - 1 ? "" : "\n"); // ���������� �ٹٲ� ���� ���
    }
    // �������� �������� �׻� �ٹٲ��� �䱸�ϴ��� Ȯ��
    if (n > 0) fout << "\n"; // ���μ����� �־��ٸ� ������ �ٹٲ� �߰� (���� �䱸���׿� ���� ����)

    fin.close(); // �Է� ���� �ݱ�
    fout.close(); // ��� ���� �ݱ�

    return 0; // ���α׷� ���� ����
}
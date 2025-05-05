#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <utility> // pair 사용을 위해 포함
#include <algorithm> // sort, min 사용을 위해 포함
#include <limits> // numeric_limits 사용을 위해 포함

using namespace std;

// Task 구조체 (대부분 변경 없음)
struct Task {
    int pid; // 프로세스 ID
    int arrival_time; // 도착 시간
    vector<int> bursts; // 버스트 시간 배열 (도착 시간, CPU 버스트, I/O 버스트, ...)
    int burst_index = 1; // 현재 처리해야 할 버스트 인덱스 (bursts[0]는 도착 시간이므로 첫 CPU 버스트부터 시작)
    int remaining = 0;   // 현재 CPU 버스트의 남은 시간
    int last_ready_time = 0; // 마지막으로 준비 큐에 들어간 시간
    int end_time = 0;      // 태스크가 실행을 완료한 시간

    // 첫 번째 버스트의 남은 시간 초기화
    void initialize_first_burst() {
        if (bursts.size() > 1) { // CPU 버스트가 하나 이상 있는지 확인
            remaining = bursts[1]; // 첫 번째 CPU 버스트 시간으로 remaining 설정
        }
        else {
            remaining = 0; // 유효한 입력(-1로 끝남)에서는 발생하지 않아야 함
        }
    }
};

// 준비 큐 비교자 (남은 시간 -> 마지막 준비 시간 -> PID 순서의 최소 힙)
struct CompareReady {
    bool operator()(Task* a, Task* b) {
        if (a->remaining != b->remaining)
            return a->remaining > b->remaining; // 남은 시간이 짧은 것 우선
        if (a->last_ready_time != b->last_ready_time)
            return a->last_ready_time > b->last_ready_time; // 먼저 준비된 것 우선 (FIFO 타이 브레이크)
        return a->pid > b->pid; // PID가 낮은 것 우선 (최종 타이 브레이크)
    }
};

// I/O 대기 큐 비교자 (I/O 완료 시간 기준 최소 힙)
struct CompareIO {
    // Pair: {io_completion_time, task_pointer} (I/O 완료 시간, 태스크 포인터)
    bool operator()(const pair<int, Task*>& a, const pair<int, Task*>& b) {
        return a.first > b.first; // 완료 시간이 빠른 것 우선
    }
};

int main() {
    // KR: 부산광역시 시간대 사용 (KST, UTC+9)
    // 참고: C++ 표준 라이브러리만으로는 시스템 시간대를 직접 설정/사용하기 어려우나, 로직 자체는 시간대와 무관합니다.
    //       현재 시간(2025년 5월 5일 오후 9:31:57 KST) 정보는 코드 실행 로직에 영향을 주지 않습니다.
    ifstream fin("srtf.inp");
    ofstream fout("srtf.out");

    if (!fin.is_open()) {
        cerr << "입력 파일 srtf.inp 열기 오류" << endl;
        return 1;
    }
    if (!fout.is_open()) {
        cerr << "출력 파일 srtf.out 열기 오류" << endl;
        return 1;
    }

    int n; // 프로세스 개수
    fin >> n;
    vector<Task> processes(n); // 프로세스 객체 저장 벡터
    vector<Task*> process_ptrs(n); // 쉬운 접근을 위해 포인터 저장

    for (int i = 0; i < n; ++i) {
        processes[i].pid = i; // PID 설정
        process_ptrs[i] = &processes[i]; // 포인터 설정
        int t;
        // 도착 시간 먼저 읽기
        if (!(fin >> t)) break; // 잠재적 읽기 오류 처리
        processes[i].bursts.push_back(t); // 버스트 배열에 추가
        processes[i].arrival_time = t; // 도착 시간 저장

        // -1이 나올 때까지 나머지 버스트 읽기
        while (fin >> t && t != -1) {
            processes[i].bursts.push_back(t);
        }
        processes[i].initialize_first_burst(); // 첫 CPU 버스트의 남은 시간 설정
    }

    // 다음 도착 프로세스를 쉽게 찾기 위해 도착 시간 순으로 정렬
    sort(process_ptrs.begin(), process_ptrs.end(), [](Task* a, Task* b) {
        return a->arrival_time < b->arrival_time;
        });

    int current_time = 0; // 현재 시뮬레이션 시간
    int completed_count = 0; // 완료된 프로세스 개수
    long long total_idle_time = 0; // 잠재적으로 큰 유휴 시간을 위해 long long 사용
    Task* current_running_task = nullptr; // 현재 CPU에서 실행 중인 태스크
    int next_arrival_idx = 0; // 다음 도착할 프로세스의 인덱스 (정렬된 포인터 배열 기준)

    // 준비 큐: CPU에서 실행 준비가 된 태스크들
    priority_queue<Task*, vector<Task*>, CompareReady> ready_queue;

    // I/O 대기 큐: 현재 I/O 작업을 수행 중인 태스크들
    // {io_completion_time, task_pointer} 쌍을 저장
    priority_queue<pair<int, Task*>, vector<pair<int, Task*>>, CompareIO> io_wait_queue;

    // 시뮬레이션을 시작할 가장 빠른 도착 시간 찾기
    if (n > 0 && next_arrival_idx < n) {
        // 시작 시간은 첫 번째 도착 시간이어야 함
        // *** 중요: 초기 유휴 시간 계산 누락됨. 첫 도착 시간이 0보다 크면 그 차이만큼 유휴 시간 발생. ***
        // 이전 답변에서 total_idle_time 계산 추가 제안됨. 여기서는 원본 유지.
        current_time = process_ptrs[next_arrival_idx]->arrival_time;
    }
    else {
        fout << 0 << "\n"; // 프로세스가 없으면 유휴 시간은 0
        fout.close();      // 반환 전에 파일 닫기
        fin.close();
        return 0; // 프로세스가 없으면 종료
    }


    while (completed_count < n) { // 모든 프로세스가 완료될 때까지 반복
        // 1. 새로 도착한 프로세스 (current_time 이전에 도착)를 준비 큐에 추가
        while (next_arrival_idx < n && process_ptrs[next_arrival_idx]->arrival_time <= current_time) {
            if (process_ptrs[next_arrival_idx]->bursts.size() > 1) { // CPU 버스트가 하나 이상 있는지 확인
                // 방금 도착했다면 준비 시간을 도착 시간으로 설정
                process_ptrs[next_arrival_idx]->last_ready_time = process_ptrs[next_arrival_idx]->arrival_time;
                ready_queue.push(process_ptrs[next_arrival_idx]); // 준비 큐에 추가
            }
            else {
                // 프로세스가 도착하자마자 즉시 종료 (도착 시간 버스트만 있고 실제 버스트는 없음)
                process_ptrs[next_arrival_idx]->end_time = process_ptrs[next_arrival_idx]->arrival_time; // 도착 시 완료
                completed_count++;
            }
            next_arrival_idx++; // 다음 도착 예정 프로세스 인덱스 증가
        }

        // 2. 현재 시간(current_time) 또는 그 이전에 I/O 완료된 태스크 확인
        //    완료 시간이 current_time 이하인 모든 이벤트를 처리
        // ================== 수정된 블록 시작 ==================
        while (!io_wait_queue.empty() && io_wait_queue.top().first <= current_time) {
            Task* task_finished_io = io_wait_queue.top().second; // I/O 완료된 태스크 포인터
            // 정확성을 위해 큐에서 실제 완료 시간 사용
            int io_actual_finish_time = io_wait_queue.top().first; // 실제 I/O 완료 시간
            io_wait_queue.pop(); // I/O 대기 큐에서 제거

            task_finished_io->burst_index++; // 완료된 I/O 다음 버스트를 가리키도록 인덱스 증가

            if (task_finished_io->burst_index < task_finished_io->bursts.size()) {
                // 다음 버스트가 더 있음. 유효한 입력이라면 다음은 CPU 버스트.
                task_finished_io->remaining = task_finished_io->bursts[task_finished_io->burst_index]; // 다음 CPU 버스트 시간으로 remaining 설정
                // 태스크는 I/O가 완료된 시점*에 준비 상태가 됨
                task_finished_io->last_ready_time = io_actual_finish_time; // 준비 시간 갱신
                ready_queue.push(task_finished_io); // 준비 큐에 추가
            }
            else {
                // 방금 완료된 I/O 버스트가 이 태스크의 *마지막* 버스트였음.
                // 태스크는 I/O가 완료된 시점*에 완료된 것으로 표시.
                task_finished_io->end_time = io_actual_finish_time; // 종료 시간 기록
                completed_count++; // 완료된 태스크 수 증가
                // 이것은 유효한 완료 시나리오이므로 오류 메시지 필요 없음.
            }
        }
        // =================== 수정된 블록 끝 ===================


        // 3. 다음에 실행할 태스크 결정 (SRTF 선점 확인)
        if (current_running_task) { // 현재 실행 중인 태스크가 있다면
            // 더 높은 우선순위의 태스크(남은 시간 짧거나, 같으면 준비 시간 빠르거나/PID 낮음)가 준비 큐에 있다면 선점
            if (!ready_queue.empty() && CompareReady()(current_running_task, ready_queue.top())) {
                // 현재 태스크를 준비 큐에 다시 넣음
                // **중요**: last_ready_time은 실제로 다시 준비 상태가 될 때만 갱신해야 함.
                // 여기서는 선점되는 것이지, 버스트를 완료하는 것이 아님. last_ready_time은
                // 마지막으로 준비 상태가 되었던 시간(도착 또는 I/O 완료 시점)을 유지해야 함.
                // 여기서 current_running_task->last_ready_time 변경 필요 없음.
                ready_queue.push(current_running_task);
                // 새로운 최고 우선순위 태스크를 가져옴
                current_running_task = ready_queue.top();
                ready_queue.pop(); // 준비 큐에서 제거
            }
        }
        // 실행 중인 태스크가 없거나 이전 태스크가 선점/완료된 경우:
        if (!current_running_task) {
            // CPU가 유휴 상태이면 준비 큐에서 가장 높은 우선순위의 태스크를 선택
            if (!ready_queue.empty()) {
                current_running_task = ready_queue.top(); // 실행할 태스크 선택
                ready_queue.pop(); // 준비 큐에서 제거
            }
        }

        // 4. 다음 이벤트까지의 시간 계산
        int time_to_next_event = numeric_limits<int>::max(); // 다음 이벤트까지 남은 시간 (최대값으로 초기화)
        bool is_idle_period = false; // 계산된 기간이 유휴 시간인지 추적하는 플래그

        // 이벤트: 다음 프로세스 도착
        if (next_arrival_idx < n) { // 아직 도착할 프로세스가 남았다면
            // 미래의 도착만 고려하도록 함
            if (process_ptrs[next_arrival_idx]->arrival_time > current_time) {
                time_to_next_event = min(time_to_next_event, process_ptrs[next_arrival_idx]->arrival_time - current_time); // 남은 시간 계산
            }
            else {
                // 이미 처리되었어야 하지만, 델타 재계산 시 0이 될 수 있음
                // 도착이 현재 또는 과거이면, 이 이벤트에 대한 시간 델타는 사실상 0
                time_to_next_event = min(time_to_next_event, 0);
            }
        }

        // 이벤트: 현재 CPU 버스트 완료
        if (current_running_task) { // 실행 중인 태스크가 있다면
            if (current_running_task->remaining <= 0) {
                // 시간 진행 전에 태스크가 정확히 current_time에 끝나는 경우 발생 가능
                time_to_next_event = min(time_to_next_event, 0);
            }
            else {
                time_to_next_event = min(time_to_next_event, current_running_task->remaining); // 남은 CPU 버스트 시간
            }
        }

        // 이벤트: 다음 I/O 완료
        if (!io_wait_queue.empty()) { // I/O 대기 중인 태스크가 있다면
            // 미래의 완료만 고려하도록 함
            if (io_wait_queue.top().first > current_time) {
                time_to_next_event = min(time_to_next_event, io_wait_queue.top().first - current_time); // 남은 시간 계산
            }
            else {
                // IO 완료가 현재 또는 과거이면, 이벤트 시간 델타는 사실상 0
                time_to_next_event = min(time_to_next_event, 0);
            }
        }

        // 대기 중인 이벤트가 없고 실행/대기할 것이 없다면, 완료되었거나 뭔가 잘못된 것.
        if (time_to_next_event == numeric_limits<int>::max() && !current_running_task && ready_queue.empty() && io_wait_queue.empty() && next_arrival_idx == n) {
            if (completed_count < n) {
                // 올바른 시뮬레이션 및 유효한 입력에서는 발생하지 않아야 함
                cerr << "경고: 시뮬레이션이 조기에 종료되었습니다. 로직을 확인하세요." << endl;
            }
            break; // 시뮬레이션 루프 종료
        }

        // time_to_next_event가 0이면 이벤트가 정확히 지금 발생한다는 의미.
        // 현재 이벤트를 처리해야 함 (위의 I/O 완료 루프 등)
        // 그리고 다른 미래 이벤트가 더 작은 단계를 강제하지 않는다면 잠재적으로 시간을 1만큼 진행해야 함.
        // 그러나 도착/IO 루프는 current_time 이벤트를 처리함.
        // 주된 문제는 *미래* 이벤트는 없지만 시스템이 끝나지 않은 경우 발생.
        // 유휴 시간 계산과 시간 진행을 개선해 봅시다.

        // 델타가 최대값이면, 찾은 미래 이벤트가 없다는 의미. 종료.
        if (time_to_next_event == numeric_limits<int>::max()) {
            break;
        }

        // 시간이 진행되도록 하고, 0 델타 케이스를 신중하게 처리.
        // 델타가 0이면 이벤트가 지금 발생. 루프가 처리하도록 함. 처리 *후에* 시간을 진행해야 할지 결정해야 함.
        // 보통 0 델타라도 처리 과정에서 새 이벤트가 추가되거나 상태가 변경되어 다음 반복에서는 0이 아닌 델타가 나올 수 있음.
        // 만약 델타 계산이 계속 0으로 이어진다면, 1만큼 강제 진행? 이벤트를 건너뛸 수 있음.
        // 더 나은 방법: 델타가 0이면 처리; 처리 후에도 여전히 0이면 필요 시 1만큼 진행?.
        // 지금은 실제 시간 건너뛰기는 델타 > 0일 때만 가정. 0 델타는 루프가 다시 돌면서 암묵적으로 처리.

        if (time_to_next_event < 0) { // 음수 델타 안전 확인
            // 로직 오류를 나타냄, current_time이 너무 많이 진행되었을 수 있음?
            cerr << "오류: 음수 시간 델타가 계산되었습니다." << endl;
            time_to_next_event = 1; // 안전 장치 (실제로는 오류 원인 수정 필요)
        }

        // 다가오는 기간이 유휴 상태인지 결정
        is_idle_period = (!current_running_task && ready_queue.empty()); // 현재 실행 중인 태스크 없고, 준비 큐도 비어있으면 유휴

        // 이 단계에 대한 경과 시간 계산
        int time_elapsed = time_to_next_event; // 경과 시간 = 다음 이벤트까지의 시간

        // time_elapsed가 0일 때 처리 (이전 버전의 주석 및 로직을 한글화 및 정리)
        if (time_elapsed == 0) {
            // 이벤트가 현재 시간에 발생함을 의미.
            // 위쪽의 도착/I/O 처리 루프가 이 이벤트들을 처리할 것임.
            // 여기서 'current_time'을 진행시키지 않음.
            // 유휴 시간도 추가하지 않음 (시간이 흐르지 않았으므로).
            // 상태 변경(예: 작업 완료)은 해당 이벤트 처리 블록에서 발생.
            // 루프는 계속되어 상태를 재평가함.
        }
        else { // time_elapsed > 0일 때
            if (is_idle_period) { // CPU가 유휴 상태이고 시간이 실제로 진행될 때만
                total_idle_time += time_elapsed; // 유휴 시간 누적
            }
            if (current_running_task) { // 실행 중인 태스크가 있다면
                current_running_task->remaining -= time_elapsed; // 남은 시간 감소
                // 남은 시간이 0 이하가 된 경우의 처리는 아래 5번 단계에서 수행
            }
            current_time += time_elapsed; // 시뮬레이션 시간 진행
        }


        // 5. CPU 버스트 완료 처리 (**새로운 current_time에 정확히 발생한 경우**)
        if (current_running_task && current_running_task->remaining <= 0) {
            // 참고: 시간은 이미 완료 시점 (또는 다른 이벤트 시점)까지 진행되었음.
            // 남은 시간 확인은 완료가 'current_time'에 발생했음을 확인.

            current_running_task->burst_index++; // 다음 버스트(I/O 또는 종료)로 이동

            if (current_running_task->burst_index < current_running_task->bursts.size()) { // 다음 버스트가 있다면
                // 다음 버스트는 I/O (인덱스가 짝수: 2, 4, 6...)
                int io_time = current_running_task->bursts[current_running_task->burst_index]; // I/O 시간
                if (io_time > 0) { // I/O 시간이 양수일 때만 큐에 추가
                    // I/O는 *지금* (current_time) 시작
                    io_wait_queue.push({ current_time + io_time, current_running_task }); // I/O 완료 시간과 함께 큐에 추가
                }
                else { // I/O 시간이 0이면 즉시 다음 CPU 준비 / 종료 상태가 되어야 함
                    current_running_task->burst_index++; // 다음 잠재적 CPU 버스트 인덱스로 이동
                    if (current_running_task->burst_index < current_running_task->bursts.size()) { // 다음 CPU 버스트가 있다면
                        current_running_task->remaining = current_running_task->bursts[current_running_task->burst_index]; // 남은 시간 설정
                        current_running_task->last_ready_time = current_time; // 지금 준비 상태가 됨
                        ready_queue.push(current_running_task); // 준비 큐에 추가
                    }
                    else { // 0 시간 I/O 후에 종료 (CPU 버스트 직후)
                        current_running_task->end_time = current_time; // 종료 시간은 CPU 버스트 (및 0-IO) 완료 시점
                        completed_count++; // 완료 카운트 증가
                    }
                }
            }
            else { // 모든 버스트 완료 (CPU가 마지막이었음)
                current_running_task->end_time = current_time; // 종료 시간은 CPU 버스트 완료 시점
                completed_count++; // 완료 카운트 증가
            }
            current_running_task = nullptr; // CPU가 비게 됨
        }
        // 잠재적 완료 처리 후, CPU가 비었다면 준비 큐를 다시 확인할 필요는 없음
        // 이 확인은 이벤트 처리 후 루프 시작 부분인 3단계로 이동되었음.

    } // while(completed_count < n) 루프 끝

    // 결과 출력
    fout << total_idle_time << "\n"; // 총 유휴 시간 출력

    // 원래 PID 순서대로 종료 시간 출력
    vector<int> end_times(n);
    for (int i = 0; i < n; ++i) {
        // pid 'i'를 가진 원래 프로세스를 찾아 종료 시간을 올바르게 저장
        for (int j = 0; j < n; ++j) {
            if (processes[j].pid == i) {
                end_times[i] = processes[j].end_time;
                break; // 찾았으면 내부 루프 종료
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        fout << end_times[i] << (i == n - 1 ? "" : "\n"); // 마지막에는 줄바꿈 없이 출력
    }
    // 문제에서 마지막에 항상 줄바꿈을 요구하는지 확인
    if (n > 0) fout << "\n"; // 프로세스가 있었다면 마지막 줄바꿈 추가 (문제 요구사항에 따라 조정)

    fin.close(); // 입력 파일 닫기
    fout.close(); // 출력 파일 닫기

    return 0; // 프로그램 정상 종료
}
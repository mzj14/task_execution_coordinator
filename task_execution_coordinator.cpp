#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <set>
#include <unordered_map>

using namespace std;

queue<int> input_q; // queue of incoming tasks, task is denoted using tag
queue<pair<int, int>> output_q; // queue of completed tasks, task is denoted using tag
unordered_map<int, set<int>*> compare_map; // map for tasks under processing, with key(tag) and value(a set of task id)
int cnt = -1; // a global counter to give each task unique id
mutex input_mtx; // mutex for fetching task from input queue
mutex output_mtx; // mutex for pushing task to output queue
int max_duration_millis = 1000; // upper bound of the task processing time of a task
condition_variable cv;

void consume_task(int id) {
  int tag;
  int cnt_copy;
  while (true) {
    // lock for input queue
    input_mtx.lock();
    if (input_q.empty()) {
      input_mtx.unlock();
      break;
    }
    // fetch the incoming task
    tag = input_q.front();
    input_q.pop();
    // increase the global id
    cnt++;
    cnt_copy = cnt;
    // add this task as a task under process to the map
    compare_map[tag]->insert(cnt);
    input_mtx.unlock();

    cout << "Proc " << id << " start processing task_tag: " << tag << ", task_id: " << cnt << endl;
    this_thread::sleep_for(chrono::milliseconds(rand() % max_duration_millis));
    cout << "Proc " << id << " finish processing task_tag: " << tag << ", task_id: " << cnt << endl;

    // unique_lock is used together with condition variable
    unique_lock<mutex> output_lock(output_mtx);
    while (cnt_copy != *(compare_map[tag]->begin())) {
      // continue wait if current task do not have the miminum id among task with same tag
      cv.wait(output_lock);
    }
    compare_map[tag]->erase(cnt_copy);
    output_q.push(make_pair(tag, cnt_copy));
    cv.notify_all();
    output_lock.unlock();
  }
}

int main(int argc, char* argv[]) {
  int tag_num = 8; // assume there are 8 kinds of task, with tag 0..7
  int task_num = 100; // assume we have 100 incoming task originally

  // initialize all tasks with random tag
  srand(time(NULL));
  for (int i = 0; i < task_num; i++) {
    input_q.push(rand() % tag_num);
  }

  // initially, under each tag, there is no tasks under process
  for (int i = 0; i < tag_num; i++) {
    compare_map[i] = new set<int>;
  }

  thread worker0(consume_task, 0);
  thread worker1(consume_task, 1);
  thread worker2(consume_task, 2);
  thread worker3(consume_task, 3);

  worker0.join();
  worker1.join();
  worker2.join();
  worker3.join();

  cout << "---------------- Complete all tasks! ------------------------" << endl;
  while (!output_q.empty()) {
    cout << "(" << output_q.front().first << "," << output_q.front().second << ")" << endl;
    output_q.pop();
  }

  // delete all the set<int> allocated on heap
  for (auto& kv: compare_map) {
    delete kv.second;
  }

  return 0;
}

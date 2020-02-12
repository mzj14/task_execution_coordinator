# task_execution_coordinator
Task execution coordinator among workers. Use multithreading, mutex and conditional variable.

## Question
Given an input queue with a certain number of tasks to be processed (these tasks belong to several categories/tags), 4 workers are going to fetch tasks from this input queue and execute them, the completed tasks will be further pushed into an output queue. Design a coordinator so that the tasks in the output queue under a same tag maintains the order as in the input queue.

## Solution
1) Model a worker as a thread. Mutual exclusion should be achieved when a worker get a task from input queue or push a completed task to an output queue.

2) We use a global counter to help identify the id of each task.

3) We maintain a collection of tasks under process. Each worker will not push its completed task into the output queue until the id of the completed task becomes the smallest among the tasks with the same tag in the collection.

## Compile and Run
Check the [C++11 solution](task_execution_coordinator.cpp).
```
g++ --std=c++11 -pthread -o main task_execution_coordinator.cpp
./main
```

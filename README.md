# Alarm Clock System

## Operating Systems and Systems Programming Project

A Linux-based alarm clock system implemented in C using Operating System concepts such as process creation, signals, timers, process suspension, and process management.

---

## Project Overview

The Alarm Clock System allows users to create and manage multiple alarms using a specified delay.

Each alarm is handled by a separate child process. The child process sets a timer and waits for the timer to expire. When the timer expires, the `SIGALRM` signal is generated and a signal handler displays the alarm notification.

The system also allows users to view active alarms, cancel alarms, and safely terminate the application.

---

## Objectives

* Implement process creation using `fork()`.
* Implement alarms using Linux timers and signals.
* Use `SIGALRM` and signal handlers.
* Suspend processes efficiently using `pause()`.
* Avoid unnecessary CPU usage while waiting.
* Provide an option to cancel active alarms.
* Support multiple alarm requests.
* Track and manage child processes.
* Demonstrate Linux process management.
* Use C and Linux system calls to implement the alarm system.

---

## Features

* Create an alarm using a delay in seconds.
* Support multiple alarms.
* Separate child process for each alarm.
* View active alarms.
* Display Alarm ID and Process ID.
* Cancel an active alarm.
* Generate `SIGALRM` when the timer expires.
* Display alarm notification.
* Monitor completed alarm processes.
* Cleanly terminate alarm processes when exiting.

---

## Technologies Used

* **Programming Language:** C
* **Operating System:** Linux / Ubuntu
* **Compiler:** GCC
* **Environment:** Linux Terminal / Ubuntu / WSL
* **System APIs:** Linux/POSIX system calls and signals

---

## Operating System Concepts Used

| Concept / API      | Purpose                                         |
| ------------------ | ----------------------------------------------- |
| `fork()`           | Creates a child process for each alarm          |
| `alarm()`          | Sets a timer for the alarm                      |
| `SIGALRM`          | Generates the alarm event                       |
| `signal()`         | Registers the signal handler                    |
| `pause()`          | Suspends the alarm process while waiting        |
| `kill()`           | Cancels an active alarm process                 |
| `waitpid()`        | Monitors and manages child processes            |
| `PID`              | Identifies individual alarm processes           |
| Multiple Processes | Allows multiple alarms to operate independently |

---

## System Flow

```text
User
  |
  v
Main Process
  |
  | fork()
  v
Alarm Process
  |
  | alarm()
  v
pause()
  |
  | Timer Expires
  v
SIGALRM
  |
  v
Signal Handler
  |
  v
Alarm Notification
```

---

## Project Structure

```text
Alarm-Clock-System/
│
├── alarm_clock.c
│
└── README.md
```

---

## Installation

### Clone the Repository

```bash
git clone https://github.com/Venkatesh3653/Alarm-Clock-System-OSSP-Project.git
```

### Enter the Project Directory

```bash
cd Alarm-Clock-System-OSSP-Project
```

### Compile the Program

```bash
gcc alarm_clock.c -o alarm_clock
```

### Run the Program

```bash
./alarm_clock
```

---

## Menu

When the program starts, the following menu is displayed:

```text
=====================================
          ALARM CLOCK SYSTEM
=====================================

Current Time: 14:32:10

1. Create Alarm
2. View Active Alarms
3. Cancel Alarm
4. Exit

Enter your choice:
```

---

## 1. Create Alarm

Select option `1` and enter the alarm delay in seconds.

Example:

```text
Enter alarm delay in seconds: 10

Alarm created successfully!
Alarm ID : 1
Process ID: 2456
Delay    : 10 seconds
```

A new child process is created using `fork()` to manage the alarm.

---

## 2. View Active Alarms

Select option `2` to display currently active alarms.

Example:

```text
========== ACTIVE ALARMS ==========

Alarm ID : 1
PID      : 2456
Delay    : 10 seconds
-----------------------------------
```

---

## 3. Cancel Alarm

Select option `3` and enter the Alarm ID.

```text
Enter Alarm ID to cancel: 1

Alarm 1 cancelled successfully.
```

The `kill()` system call is used to terminate the corresponding alarm process.

---

## 4. Alarm Notification

When the timer expires, the `SIGALRM` signal is generated.

Example:

```text
=================================
          ALARM RINGING!
=================================
Wake Up! Your alarm has expired.
=================================
```

---

## Multiple Alarms

Multiple alarms can be created at the same time.

Example:

```text
Alarm 1 → 10 seconds
Alarm 2 → 20 seconds
Alarm 3 → 30 seconds
```

Each alarm is managed by a separate child process.

```text
              Parent Process
               /     |     \
              /      |      \
             v       v       v
          Alarm 1  Alarm 2  Alarm 3
           PID 1    PID 2    PID 3
             |        |        |
          alarm()  alarm()  alarm()
             |        |        |
           pause()  pause()  pause()
```

---

## Process Management

### Parent Process

The parent process:

* Creates alarm processes.
* Tracks process IDs.
* Displays active alarms.
* Cancels alarms.
* Monitors completed processes.
* Terminates remaining alarm processes during exit.

### Child Process

The child process:

* Sets the alarm timer.
* Registers the `SIGALRM` signal handler.
* Waits using `pause()`.
* Displays the alarm notification.

---

## CPU Efficiency

The alarm process does not continuously check the time.

It uses:

```c
alarm(seconds);
pause();
```

The `pause()` system call suspends the process until a signal is received. This prevents unnecessary CPU usage while waiting for the alarm.

---

## Example Output

```text
=====================================
          ALARM CLOCK SYSTEM
=====================================

Current Time: 14:32:10

1. Create Alarm
2. View Active Alarms
3. Cancel Alarm
4. Exit

Enter your choice: 1

Enter alarm delay in seconds: 5

Alarm created successfully!
Alarm ID : 1
Process ID: 2456
Delay    : 5 seconds
```

After the timer expires:

```text
=================================
          ALARM RINGING!
=================================
Wake Up! Your alarm has expired.
=================================
```

---

## Learning Outcomes

This project demonstrates:

* Process creation
* Parent-child processes
* Process IDs
* Signals
* Signal handlers
* Timers
* Process suspension
* Process termination
* Process synchronization
* Multiple process management
* Linux system calls

---

## Future Enhancements

* Schedule alarms using a specific clock time.
* Add snooze functionality.
* Add custom alarm sounds.
* Store alarms permanently.
* Add recurring alarms.
* Support date-base


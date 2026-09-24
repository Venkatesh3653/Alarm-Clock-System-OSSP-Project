#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

struct Alarm {
    int id;
    pid_t pid;
    int active;
    int seconds;
};

struct Alarm alarms[10];
int alarm_count = 0;

void alarm_handler(int sig)
{
    printf("\n\n=================================\n");
    printf("          ALARM RINGING!\n");
    printf("=================================\n");
    printf("Wake Up! Your alarm has expired.\n");
    printf("=================================\n\n");
    fflush(stdout);
}

void alarm_process(int seconds)
{
    signal(SIGALRM, alarm_handler);

    alarm(seconds);

    pause();

    exit(0);
}

void create_alarm()
{
    int seconds;

    if (alarm_count >= 10) {
        printf("Maximum of 10 alarms allowed.\n");
        return;
    }

    printf("\nEnter alarm delay in seconds: ");
    scanf("%d", &seconds);

    if (seconds <= 0) {
        printf("Invalid delay.\n");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        alarm_process(seconds);
    }

    alarms[alarm_count].id = alarm_count + 1;
    alarms[alarm_count].pid = pid;
    alarms[alarm_count].active = 1;
    alarms[alarm_count].seconds = seconds;

    alarm_count++;

    printf("\nAlarm created successfully!\n");
    printf("Alarm ID : %d\n", alarm_count);
    printf("Process ID: %d\n", pid);
    printf("Delay    : %d seconds\n", seconds);
}

void show_alarms()
{
    int found = 0;

    printf("\n========== ACTIVE ALARMS ==========\n");

    for (int i = 0; i < alarm_count; i++) {
        if (alarms[i].active) {
            printf("Alarm ID : %d\n", alarms[i].id);
            printf("PID      : %d\n", alarms[i].pid);
            printf("Delay    : %d seconds\n", alarms[i].seconds);
            printf("-----------------------------------\n");
            found = 1;
        }
    }

    if (!found) {
        printf("No active alarms.\n");
    }
}

void cancel_alarm()
{
    int id;

    show_alarms();

    printf("\nEnter Alarm ID to cancel: ");
    scanf("%d", &id);

    if (id < 1 || id > alarm_count) {
        printf("Invalid Alarm ID.\n");
        return;
    }

    if (!alarms[id - 1].active) {
        printf("Alarm is already inactive.\n");
        return;
    }

    if (kill(alarms[id - 1].pid, SIGTERM) == 0) {
        alarms[id - 1].active = 0;

        waitpid(alarms[id - 1].pid, NULL, 0);

        printf("Alarm %d cancelled successfully.\n", id);
    } else {
        perror("kill");
    }
}

void check_completed_alarms()
{
    for (int i = 0; i < alarm_count; i++) {

        if (alarms[i].active) {

            int status;

            pid_t result = waitpid(
                alarms[i].pid,
                &status,
                WNOHANG
            );

            if (result == alarms[i].pid) {
                alarms[i].active = 0;
            }
        }
    }
}

void display_current_time()
{
    time_t current_time;
    struct tm *time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    printf("\nCurrent Time: %02d:%02d:%02d\n",
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec);
}

int main()
{
    int choice;

    while (1) {

        check_completed_alarms();

        printf("\n\n");
        printf("=====================================\n");
        printf("          ALARM CLOCK SYSTEM\n");
        printf("=====================================\n");

        display_current_time();

        printf("\n1. Create Alarm\n");
        printf("2. View Active Alarms\n");
        printf("3. Cancel Alarm\n");
        printf("4. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                create_alarm();
                break;

            case 2:
                show_alarms();
                break;

            case 3:
                cancel_alarm();
                break;

            case 4:

                for (int i = 0; i < alarm_count; i++) {
                    if (alarms[i].active) {
                        kill(alarms[i].pid, SIGTERM);
                    }
                }

                for (int i = 0; i < alarm_count; i++) {
                    if (alarms[i].active) {
                        waitpid(alarms[i].pid, NULL, 0);
                    }
                }

                printf("\nExiting Alarm Clock System...\n");
                exit(0);

            default:
                printf("\nInvalid choice.\n");
        }
    }

    return 0;
}

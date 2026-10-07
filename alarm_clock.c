#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

struct Alarm {
    int id;
    pid_t pid;
    int active;
    int recurring;
    int snooze;
    int delay;
    int priority;
    time_t target;
    time_t created;
    char label[50];
};

struct Alarm *alarms = NULL;
int alarm_count = 0;

void clear_screen()
{
    printf("\033[2J\033[H");
}

void alarm_handler(int sig)
{
    const char message[] =
        "\n============================================================\n"
        "                     ALARM RINGING!\n"
        "============================================================\n"
        "                       WAKE UP!\n"
        "============================================================\n";

    write(STDOUT_FILENO, message, sizeof(message) - 1);
    printf("\a\a\a");
    fflush(stdout);

    exit(0);
}

void alarm_process(time_t target)
{
    signal(SIGALRM, alarm_handler);

    int seconds = (int)difftime(target, time(NULL));

    if (seconds <= 0)
        seconds = 1;

    alarm(seconds);
    pause();

    exit(0);
}

void save_alarms()
{
    FILE *file = fopen("alarms.dat", "w");

    if (file == NULL)
        return;

    for (int i = 0; i < alarm_count; i++) {
        if (alarms[i].active) {
            fprintf(file, "%d|%d|%d|%d|%d|%d|%ld|%ld|%s\n",
                    alarms[i].id,
                    alarms[i].active,
                    alarms[i].recurring,
                    alarms[i].snooze,
                    alarms[i].delay,
                    alarms[i].priority,
                    (long)alarms[i].target,
                    (long)alarms[i].created,
                    alarms[i].label);
        }
    }

    fclose(file);
}

void start_alarm(int i, time_t target)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0)
        alarm_process(target);

    alarms[i].pid = pid;
    alarms[i].target = target;
    alarms[i].active = 1;
}

void update_alarms()
{
    int status;

    for (int i = 0; i < alarm_count; i++) {

        if (!alarms[i].active)
            continue;

        pid_t result = waitpid(
            alarms[i].pid,
            &status,
            WNOHANG
        );

        if (result == alarms[i].pid) {

            if (alarms[i].recurring) {

                alarms[i].target += 24 * 60 * 60;

                start_alarm(i, alarms[i].target);

            } else {

                alarms[i].active = 0;
            }
        }
    }

    save_alarms();
}

const char *priority_name(int priority)
{
    if (priority == 3)
        return "HIGH";

    if (priority == 2)
        return "MEDIUM";

    return "LOW";
}

void display_clock()
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    printf("                    CURRENT TIME\n");
    printf("                    %02d:%02d:%02d\n",
           t->tm_hour,
           t->tm_min,
           t->tm_sec);
}

void display_dashboard()
{
    int active = 0;
    int completed = 0;

    for (int i = 0; i < alarm_count; i++) {
        if (alarms[i].active)
            active++;
        else
            completed++;
    }

    clear_screen();

    printf("============================================================\n");
    printf("                 ALARM CLOCK SYSTEM\n");
    printf("============================================================\n\n");

    display_clock();

    printf("\n------------------------------------------------------------\n");
    printf("                     ALARM STATISTICS\n");
    printf("------------------------------------------------------------\n");

    printf("  Total Alarms       : %d\n", alarm_count);
    printf("  Active Alarms      : %d\n", active);
    printf("  Completed/Stopped  : %d\n", completed);

    printf("------------------------------------------------------------\n");
    printf("                     ACTIVE ALARMS\n");
    printf("------------------------------------------------------------\n");

    if (active == 0) {

        printf("\n                    No Active Alarms\n\n");

    } else {

        printf("\n");
        printf("%-4s %-15s %-12s %-9s %-9s %-10s\n",
               "ID", "LABEL", "TIME", "TYPE",
               "PRIORITY", "PID");

        printf("------------------------------------------------------------\n");

        for (int i = 0; i < alarm_count; i++) {

            if (alarms[i].active) {

                struct tm *t =
                    localtime(&alarms[i].target);

                char time_string[30];

                strftime(
                    time_string,
                    sizeof(time_string),
                    "%d-%m %H:%M",
                    t
                );

                printf("%-4d %-15s %-12s %-9s %-9s %-10d\n",
                       alarms[i].id,
                       alarms[i].label,
                       time_string,
                       alarms[i].recurring ?
                       "DAILY" : "ONCE",
                       priority_name(
                           alarms[i].priority),
                       alarms[i].pid);
            }
        }

        printf("\n");
    }

    printf("============================================================\n");
    printf("                         OPTIONS\n");
    printf("============================================================\n\n");

    printf("   [1] Create Delay Alarm\n");
    printf("   [2] Create Date/Time Alarm\n");
    printf("   [3] View Alarms\n");
    printf("   [4] Snooze Alarm\n");
    printf("   [5] Cancel Alarm\n");
    printf("   [6] Refresh Dashboard\n");
    printf("   [7] Process Information\n");
    printf("   [8] Alarm History\n");
    printf("   [9] Exit\n");

    printf("\n============================================================\n");
}

void get_priority(int *priority)
{
    printf("\nSelect Priority:\n");
    printf("1. Low\n");
    printf("2. Medium\n");
    printf("3. High\n");
    printf("Enter priority: ");

    scanf("%d", priority);

    if (*priority < 1 || *priority > 3)
        *priority = 1;
}

void create_delay_alarm()
{
    int seconds;

    alarms = realloc(
        alarms,
        (alarm_count + 1) * sizeof(struct Alarm)
    );

    if (alarms == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    int i = alarm_count;

    printf("\nEnter alarm label: ");
    scanf(" %[^\n]", alarms[i].label);

    printf("Enter delay in seconds: ");
    scanf("%d", &seconds);

    if (seconds <= 0) {
        printf("Invalid delay.\n");
        sleep(2);
        return;
    }

    get_priority(&alarms[i].priority);

    alarms[i].id = i + 1;
    alarms[i].recurring = 0;
    alarms[i].snooze = 0;
    alarms[i].delay = seconds;
    alarms[i].created = time(NULL);

    start_alarm(
        i,
        time(NULL) + seconds
    );

    alarm_count++;

    save_alarms();

    printf("\n============================================================\n");
    printf("                    ALARM CREATED\n");
    printf("============================================================\n");

    printf("  Alarm ID     : %d\n", alarms[i].id);
    printf("  Label        : %s\n", alarms[i].label);
    printf("  Process ID   : %d\n", alarms[i].pid);
    printf("  Parent PID   : %d\n", getpid());
    printf("  Delay        : %d seconds\n", seconds);
    printf("  Priority     : %s\n",
           priority_name(alarms[i].priority));

    printf("============================================================\n");

    sleep(2);
}

void create_datetime_alarm()
{
    struct tm t = {0};

    char repeat;
    char label[50];

    int day;
    int month;
    int year;
    int hour;
    int minute;

    printf("\nEnter alarm label: ");
    scanf(" %[^\n]", label);

    printf("\nEnter date and time\n");

    printf("Day    : ");
    scanf("%d", &day);

    printf("Month  : ");
    scanf("%d", &month);

    printf("Year   : ");
    scanf("%d", &year);

    printf("Hour   : ");
    scanf("%d", &hour);

    printf("Minute : ");
    scanf("%d", &minute);

    t.tm_mday = day;
    t.tm_mon = month - 1;
    t.tm_year = year - 1900;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = 0;

    time_t target = mktime(&t);

    if (target == (time_t)-1) {
        printf("Invalid date/time.\n");
        sleep(2);
        return;
    }

    if (target <= time(NULL)) {
        printf("\nThe specified time has already passed.\n");
        sleep(2);
        return;
    }

    printf("Repeat daily? (y/n): ");
    scanf(" %c", &repeat);

    int recurring =
        (repeat == 'y' || repeat == 'Y');

    alarms = realloc(
        alarms,
        (alarm_count + 1) * sizeof(struct Alarm)
    );

    if (alarms == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    int i = alarm_count;

    strcpy(alarms[i].label, label);

    alarms[i].id = i + 1;
    alarms[i].recurring = recurring;
    alarms[i].snooze = 0;
    alarms[i].delay = 0;
    alarms[i].created = time(NULL);

    get_priority(&alarms[i].priority);

    start_alarm(i, target);

    alarm_count++;

    save_alarms();

    printf("\n============================================================\n");
    printf("                  DATE/TIME ALARM CREATED\n");
    printf("============================================================\n");

    printf("  Alarm ID     : %d\n", alarms[i].id);
    printf("  Label        : %s\n", alarms[i].label);
    printf("  Process ID   : %d\n", alarms[i].pid);
    printf("  Parent PID   : %d\n", getpid());
    printf("  Repeat       : %s\n",
           recurring ? "DAILY" : "ONCE");
    printf("  Priority     : %s\n",
           priority_name(alarms[i].priority));

    printf("============================================================\n");

    sleep(2);
}

void view_alarms()
{
    display_dashboard();

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void snooze_alarm()
{
    int id;
    int minutes;

    display_dashboard();

    printf("\nEnter Alarm ID to snooze: ");
    scanf("%d", &id);

    if (id < 1 || id > alarm_count ||
        !alarms[id - 1].active) {

        printf("Invalid or inactive alarm.\n");
        sleep(2);
        return;
    }

    printf("Enter snooze duration in minutes: ");
    scanf("%d", &minutes);

    if (minutes <= 0) {
        printf("Invalid snooze duration.\n");
        sleep(2);
        return;
    }

    int i = id - 1;

    kill(alarms[i].pid, SIGTERM);

    waitpid(
        alarms[i].pid,
        NULL,
        0
    );

    alarms[i].snooze = 1;

    start_alarm(
        i,
        time(NULL) + minutes * 60
    );

    save_alarms();

    printf("\n============================================================\n");
    printf("                     ALARM SNOOZED\n");
    printf("============================================================\n");

    printf("  Alarm ID     : %d\n", id);
    printf("  Snooze       : %d minutes\n", minutes);
    printf("  New PID      : %d\n", alarms[i].pid);

    printf("============================================================\n");

    sleep(2);
}

void cancel_alarm()
{
    int id;

    display_dashboard();

    printf("\nEnter Alarm ID to cancel: ");
    scanf("%d", &id);

    if (id < 1 || id > alarm_count ||
        !alarms[id - 1].active) {

        printf("Invalid or inactive alarm.\n");
        sleep(2);
        return;
    }

    int i = id - 1;

    kill(alarms[i].pid, SIGTERM);

    waitpid(
        alarms[i].pid,
        NULL,
        0
    );

    alarms[i].active = 0;

    save_alarms();

    printf("\n============================================================\n");
    printf("                    ALARM CANCELLED\n");
    printf("============================================================\n");

    printf("  Alarm ID     : %d\n", id);
    printf("  PID          : %d\n", alarms[i].pid);
    printf("  Status       : TERMINATED\n");

    printf("============================================================\n");

    sleep(2);
}

void process_information()
{
    display_dashboard();

    printf("\n============================================================\n");
    printf("                   PROCESS INFORMATION\n");
    printf("============================================================\n");

    printf("\nMain Alarm Manager\n");
    printf("PID  : %d\n", getpid());
    printf("PPID : %d\n", getppid());

    printf("\n------------------------------------------------------------\n");

    int found = 0;

    for (int i = 0; i < alarm_count; i++) {

        if (alarms[i].active) {

            found = 1;

            printf("\nAlarm ID      : %d\n",
                   alarms[i].id);

            printf("Label         : %s\n",
                   alarms[i].label);

            printf("Child PID     : %d\n",
                   alarms[i].pid);

            printf("Parent PID    : %d\n",
                   getpid());

            printf("Process State : RUNNING\n");

            printf("Priority      : %s\n",
                   priority_name(
                       alarms[i].priority));

            printf("------------------------------------------------------------\n");
        }
    }

    if (!found)
        printf("\nNo active child processes.\n");

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void alarm_history()
{
    clear_screen();

    printf("============================================================\n");
    printf("                       ALARM HISTORY\n");
    printf("============================================================\n\n");

    if (alarm_count == 0) {

        printf("No alarm history available.\n");

    } else {

        printf("%-4s %-15s %-10s %-10s %-10s\n",
               "ID",
               "LABEL",
               "PID",
               "PRIORITY",
               "STATUS");

        printf("------------------------------------------------------------\n");

        for (int i = 0; i < alarm_count; i++) {

            printf("%-4d %-15s %-10d %-10s %-10s\n",
                   alarms[i].id,
                   alarms[i].label,
                   alarms[i].pid,
                   priority_name(
                       alarms[i].priority),
                   alarms[i].active ?
                   "ACTIVE" : "STOPPED");
        }
    }

    printf("\n============================================================\n");
    printf("Press Enter to continue...");

    getchar();
    getchar();
}

void exit_program()
{
    printf("\nStopping alarm processes...\n");

    for (int i = 0; i < alarm_count; i++) {

        if (alarms[i].active) {

            kill(
                alarms[i].pid,
                SIGTERM
            );

            waitpid(
                alarms[i].pid,
                NULL,
                0
            );
        }
    }

    free(alarms);

    clear_screen();

    printf("\n============================================================\n");
    printf("                 ALARM CLOCK SYSTEM\n");
    printf("============================================================\n");

    printf("\n                  All processes stopped.\n");
    printf("                  Memory released.\n");
    printf("                  Exiting program...\n");

    printf("\n============================================================\n");

    exit(0);
}

int main()
{
    int choice;

    while (1) {

        update_alarms();

        display_dashboard();

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                create_delay_alarm();
                break;

            case 2:
                create_datetime_alarm();
                break;

            case 3:
                view_alarms();
                break;

            case 4:
                snooze_alarm();
                break;

            case 5:
                cancel_alarm();
                break;

            case 6:
                break;

            case 7:
                process_information();
                break;

            case 8:
                alarm_history();
                break;

            case 9:
                exit_program();
                break;

            default:
                printf("\nInvalid choice.\n");
                sleep(2);
        }
    }

    return 0;
}

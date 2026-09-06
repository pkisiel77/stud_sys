#include "blank/moje.h"

#include <stdio.h>
#include <string.h>

extern struct agenda *SysA[AG_SIZE];
extern struct agenda *SysQ[Q_SIZE];
extern int AgendaMax;
extern int NoServisMax;
extern char select_mode;

void check_agenda(time_t dt);
struct agenda *check_queue(void);

static int failures = 0;

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__,           \
                    #condition);                                                \
            failures++;                                                        \
        }                                                                       \
    } while (0)

static void reset_scheduler(void)
{
    memset(SysA, 0, sizeof(struct agenda *) * AG_SIZE);
    memset(SysQ, 0, sizeof(struct agenda *) * Q_SIZE);
    AgendaMax = -1;
    NoServisMax = -1;
}

static struct agenda ready_task(int priority)
{
    struct agenda task;
    memset(&task, 0, sizeof(task));
    task.mode = 'p';
    task.state = 1;
    task.prior = priority;
    task.cur_prior = priority;
    task.number_of_calls = -1;
    return task;
}

static void test_max_priority_scans_whole_queue(void)
{
    struct agenda low = ready_task(10);
    struct agenda high = ready_task(200);

    reset_scheduler();
    SysQ[0] = &low;
    SysQ[2] = &high;
    NoServisMax = 2;
    select_mode = MAX_PRIOR;

    CHECK(check_queue() == &high);
    CHECK(high.state == -1);
    CHECK(low.state == 1);
}

static void test_no_priority_marks_selected_task_running(void)
{
    struct agenda first = ready_task(1);
    struct agenda second = ready_task(100);

    reset_scheduler();
    SysQ[1] = &first;
    SysQ[2] = &second;
    NoServisMax = 2;
    select_mode = NO_PRIOR;

    CHECK(check_queue() == &first);
    CHECK(first.state == -1);
    CHECK(second.state == 1);
}

static void test_random_mode_uses_real_queue_index(void)
{
    struct agenda only = ready_task(10);

    reset_scheduler();
    SysQ[7] = &only;
    NoServisMax = 7;
    select_mode = RAND_PRIOR;

    CHECK(check_queue() == &only);
    CHECK(only.state == -1);
}

static void test_random_mode_accepts_zero_weight(void)
{
    struct agenda only = ready_task(0);

    reset_scheduler();
    SysQ[7] = &only;
    NoServisMax = 7;
    select_mode = RAND_PRIOR;

    CHECK(check_queue() == &only);
    CHECK(only.state == -1);
}

static void test_completed_entry_is_removed_before_selection(void)
{
    struct agenda completed = ready_task(100);
    struct agenda ready = ready_task(10);

    reset_scheduler();
    completed.state = 0;
    SysQ[0] = &completed;
    SysQ[1] = &ready;
    NoServisMax = 1;
    select_mode = MAX_PRIOR;

    CHECK(check_queue() == &ready);
    CHECK(SysQ[0] == NULL);
}

static void test_agenda_releases_ready_task_to_queue(void)
{
    struct agenda task = ready_task(30);

    reset_scheduler();
    task.mode = 's';
    task.delay = 0.0f;
    task.Interval = 5;
    task.number_of_calls = 2;
    task.state = 0;
    SysA[0] = &task;
    AgendaMax = 0;

    check_agenda(1);

    CHECK(task.number_of_calls == 1);
    CHECK(task.delay == 4.0f);
    CHECK(task.state == 1);
    CHECK(SysQ[0] == &task);
}

int main(void)
{
    test_max_priority_scans_whole_queue();
    test_no_priority_marks_selected_task_running();
    test_random_mode_uses_real_queue_index();
    test_random_mode_accepts_zero_weight();
    test_completed_entry_is_removed_before_selection();
    test_agenda_releases_ready_task_to_queue();

    if (failures != 0) {
        fprintf(stderr, "%d scheduler test(s) failed\n", failures);
        return 1;
    }
    puts("scheduler tests: OK");
    return 0;
}

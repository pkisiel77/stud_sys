#include "blank/moje.h"
#include "sensor_sim.h"
#include "sys_rep.h"
#include "scheduler.h"
#include "zlecenia.h"

#include <stdio.h>
#include <string.h>


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

static void test_status_snapshot_explains_scheduler_state(void)
{
    struct Service service;
    struct agenda waiting = ready_task(20);
    struct agenda queued = ready_task(30);
    struct agenda disabled = ready_task(40);
    struct sys_status_row rows[AG_SIZE];
    struct sys_status_summary summary;
    int count;

    memset(&service, 0, sizeof(service));
    service.name = "sensor";
    waiting.S = &service;
    waiting.name = "temperature";
    waiting.delay = 5.0f;
    waiting.Interval = 10;
    waiting.rt.value = 21.5f;
    queued.S = &service;
    queued.name = NULL;
    queued.rt.alarm = RT_ALARM_RANGE;
    disabled.S = &service;
    disabled.number_of_calls = 0;

    reset_scheduler();
    SysA[0] = &waiting;
    SysA[3] = &queued;
    SysA[4] = &disabled;
    SysQ[2] = &queued;

    count = sys_status_snapshot(rows, AG_SIZE, &summary);

    CHECK(count == 3);
    CHECK(summary.active == 2);
    CHECK(summary.queued == 1);
    CHECK(summary.alarms == 1);
    CHECK(rows[0].agenda_index == 0);
    CHECK(strcmp(rows[0].name, "temperature") == 0);
    CHECK(rows[0].state == SYS_STATUS_WAITING);
    CHECK(rows[1].agenda_index == 3);
    CHECK(strcmp(rows[1].name, "sensor") == 0);
    CHECK(rows[1].state == SYS_STATUS_READY);
    CHECK(rows[2].state == SYS_STATUS_DISABLED);
    CHECK(sys_status_snapshot(rows, 1, NULL) == 3);
}

static void test_sensor_sample_finishes_queue_execution(void)
{
    struct agenda task = ready_task(100);
    struct sensor_sim sensor;

    memset(&sensor, 0, sizeof(sensor));
    sensor.waveform = 's';
    sensor.amplitude = 10.0f;
    sensor.frequency = 0.1f;
    sensor.offset = 20.0f;
    sensor.thr_min = 12.0f;
    sensor.thr_max = 28.0f;
    task.data = &sensor;
    task.rt.elapsed = 0.0f;

    CHECK(sensor_sim_main(&task) == 0);
    CHECK(task.state == 0);
    CHECK(task.rt.value == 20.0f);
    CHECK(task.rt.elapsed == 1.0f);
    CHECK(task.rt.alarm == RT_ALARM_OK);
}

static void test_system_date_uses_calendar_month(void)
{
    int dzien = 0, miesiac = 0, rok = 0;
    time_t now = time(NULL);
    struct tm expected = *localtime(&now);

    data_systemowa(&dzien, &miesiac, &rok);

    CHECK(miesiac >= 1 && miesiac <= 12);
    CHECK(miesiac == expected.tm_mon + 1);
    CHECK(dzien == expected.tm_mday);
    CHECK(rok == expected.tm_year + 1900);
}

static void test_save_without_new_record_does_not_exit_report(void)
{
    struct agenda existing;
    struct agenda *Aserv = &existing;
    struct agenda *Anew = NULL;
    int nr_rekordu = 3;

    memset(&existing, 0, sizeof(existing));
    existing.number_of_calls = 5;

    CHECK(decyzje_run('w', &Aserv, &Anew, &nr_rekordu, 1, "test") == 0);
    CHECK(Aserv == &existing);
    CHECK(existing.number_of_calls == 5);
    CHECK(nr_rekordu == 3);
}

static void test_record_lookup_past_last_returns_last_record(void)
{
    struct Service service;
    struct agenda first = ready_task(1);
    struct agenda second = ready_task(1);
    int nr_rekordu = 5;

    memset(&service, 0, sizeof(service));
    service.kod_uslugi = 7;
    first.S = &service;
    second.S = &service;

    reset_scheduler();
    SysA[1] = &first;
    SysA[3] = &second;

    /* nr_w_agendzie < 0: szukanie po numerze rekordu, ktory nie istnieje */
    CHECK(rekord_zlecenia_agendy(&nr_rekordu, -1, 7) == &second);
    CHECK(nr_rekordu == 1);

    nr_rekordu = 0;
    CHECK(rekord_zlecenia_agendy(&nr_rekordu, -1, 7) == &first);
    CHECK(nr_rekordu == 0);

    nr_rekordu = 0;
    CHECK(rekord_zlecenia_agendy(&nr_rekordu, -1, 8) == NULL);

    reset_scheduler();
}

int main(void)
{
    test_max_priority_scans_whole_queue();
    test_no_priority_marks_selected_task_running();
    test_random_mode_uses_real_queue_index();
    test_random_mode_accepts_zero_weight();
    test_completed_entry_is_removed_before_selection();
    test_agenda_releases_ready_task_to_queue();
    test_status_snapshot_explains_scheduler_state();
    test_sensor_sample_finishes_queue_execution();
    test_system_date_uses_calendar_month();
    test_save_without_new_record_does_not_exit_report();
    test_record_lookup_past_last_returns_last_record();

    if (failures != 0) {
        fprintf(stderr, "%d scheduler test(s) failed\n", failures);
        return 1;
    }
    puts("scheduler tests: OK");
    return 0;
}

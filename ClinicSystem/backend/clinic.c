#include "clinic.h"
#include <stdio.h>
#include <string.h>

Doctor doctors[MAX_DOCTORS];
int doctor_count = 0;

// Internal Helpers
bool check_working_hours(int start, int duration) { 
    return (start >= WORK_START_MIN) && (start + duration <= WORK_END_MIN);
}

void rebuild_day_avl(DaySchedule* day) {
    if (day->root) {
        avl_free(day->root);
        day->root = NULL;
    }
    Event* curr = day->head;
    bool success;
    while (curr) {
        day->root = avl_insert(day->root, curr->start_time, curr->start_time + curr->duration, &success);
        curr = curr->next;
    }
}

// Implementations
void system_init() {
    doctor_count = 0;
}

Doctor* doctor_find(const char* name) {
    for (int i = 0; i < doctor_count; i++) {
        if (strcmp(doctors[i].name, name) == 0) return &doctors[i];
    }
    return NULL;
}

void doctor_register(const char* name, const char* passkey) {
    if (doctor_count >= MAX_DOCTORS) return;
    Doctor* doc = &doctors[doctor_count++];
    strcpy(doc->name, name);
    strcpy(doc->passkey, passkey);
    for (int i = 0; i < DAYS_IN_WEEK; i++) {
        doc->week[i].head = NULL;
        doc->week[i].root = NULL;
        doc->week[i].event_count = 0;
    }
}

bool doctor_login(const char* name, const char* passkey) {
    Doctor* doc = doctor_find(name);
    return (doc && strcmp(doc->passkey, passkey) == 0);
}

int schedule_add(Doctor* doc, int day_idx, const char* name, int start, int duration, EventType type) {
    DaySchedule* day = &doc->week[day_idx];
    if (day->event_count >= MAX_EVENTS_PER_DAY) return -1;

    if (avl_check_collision(day->root, start, start + duration)) return -2; // Collision

    int new_id = 0;
    // Add to List
    day->head = list_add_sorted(day->head, name, start, duration, type, &new_id);
    day->event_count++;

    // Validate Working Hours for Red Flag (Post-Insertion check on the node)
    Event* node = list_find(day->head, new_id);
    if (!check_working_hours(start, duration) && node) node->is_collision = true;

    // Update AVL
    rebuild_day_avl(day); // Simple approach: rebuild tree from list to ensure consistency

    return new_id;
}

void schedule_extend(Doctor* doc, int day_idx, int event_id, int extra_time) {
    DaySchedule* day = &doc->week[day_idx];
    Event* event = list_find(day->head, event_id);
    if (!event) return;

    event->duration += extra_time;
    if (!check_working_hours(event->start_time, event->duration)) event->is_collision = true;

    list_propagate_delay(event, check_working_hours);
    rebuild_day_avl(day);
}

// JSON formatting
void min_to_str(int minutes, char* buf) {
    sprintf(buf, "%02d:%02d", minutes / 60, minutes % 60);
}

char* json_get_week(Doctor* doc) {
    static char buf[100000];
    strcpy(buf, "[");
    for (int i = 0; i < DAYS_IN_WEEK; i++) {
        char day_buf[10000];
        sprintf(day_buf, "{\"day\": %d, \"events\": [", i);
        Event* curr = doc->week[i].head;
        while (curr) {
            char t_start[10], t_end[10];
            min_to_str(curr->start_time, t_start);
            min_to_str(curr->start_time + curr->duration, t_end);
            char ev_json[512];
            sprintf(ev_json, 
                "{\"id\": %d, \"name\": \"%s\", \"start\": \"%s\", \"end\": \"%s\", \"type\": %d, \"collision\": %s}",
                curr->id, curr->name, t_start, t_end, curr->type, curr->is_collision ? "true" : "false"
            );
            strcat(day_buf, ev_json);
            if (curr->next) strcat(day_buf, ",");
            curr = curr->next;
        }
        strcat(day_buf, "]}");
        strcat(buf, day_buf);
        if (i < 6) strcat(buf, ",");
    }
    strcat(buf, "]");
    return buf;
}

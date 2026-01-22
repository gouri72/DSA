#ifndef CLINIC_H
#define CLINIC_H

#include "types.h"
#include "avl.h"
#include "event_list.h"

// Complex Structs involving multiple DS
typedef struct DaySchedule {
    Event *head;    // Linked List
    AVLNode *root;  // AVL Tree
    int event_count;
} DaySchedule;

typedef struct Doctor {
    char name[50];
    char passkey[50];
    DaySchedule week[DAYS_IN_WEEK]; 
} Doctor;

extern Doctor doctors[MAX_DOCTORS];
extern int doctor_count;

// System Logic
void system_init();
Doctor* doctor_find(const char* name);
void doctor_register(const char* name, const char* passkey);
bool doctor_login(const char* name, const char* passkey);

// Feature Logic
int schedule_add(Doctor* doc, int day_idx, const char* name, int start, int duration, EventType type);
void schedule_extend(Doctor* doc, int day_idx, int event_id, int extra_time);

// JSON helpers
char* json_get_week(Doctor* doc);

#endif

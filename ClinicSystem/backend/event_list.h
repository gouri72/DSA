#ifndef EVENT_LIST_H
#define EVENT_LIST_H

#include "types.h"
#include <stdlib.h>
#include <string.h>

typedef struct Event {
    int id;
    char name[50]; 
    int start_time;
    int duration;
    EventType type;
    bool is_collision; // Flag for UI 
    struct Event *next;
} Event;

Event* list_add_sorted(Event* head, const char* name, int start, int duration, EventType type, int* new_id);
Event* list_find(Event* head, int id);
void list_propagate_delay(Event* curr, bool (*check_hours_func)(int, int));
// No delete implemented yet as not strictly requested in flow, but good practice
void list_free(Event* head);

#endif

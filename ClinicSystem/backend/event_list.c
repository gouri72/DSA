#include "event_list.h"
#include <stdio.h> // for rand

Event* list_add_sorted(Event* head, const char* name, int start, int duration, EventType type, int* out_id) {
    Event* new_event = (Event*)malloc(sizeof(Event));
    int id = rand();
    if (out_id) *out_id = id;
    
    new_event->id = id;
    strncpy(new_event->name, name, 49);
    new_event->start_time = start;
    new_event->duration = duration;
    new_event->type = type;
    new_event->is_collision = false;
    
    if (!head || head->start_time >= start) {
        new_event->next = head;
        return new_event;
    } else {
        Event* curr = head;
        while (curr->next && curr->next->start_time < start) {
            curr = curr->next;
        }
        new_event->next = curr->next;
        curr->next = new_event;
        return head;
    }
}

Event* list_find(Event* head, int id) {
    Event* curr = head;
    while (curr) {
        if (curr->id == id) return curr;
        curr = curr->next;
    }
    return NULL;
}

void list_propagate_delay(Event* curr, bool (*check_hours_func)(int, int)) {
    if (!curr || !curr->next) return;
    
    Event* next = curr->next;
    int curr_end = curr->start_time + curr->duration;
    
    if (curr_end > next->start_time) {
        int shift = curr_end - next->start_time;
        next->start_time += shift;
        
        if (check_hours_func && !check_hours_func(next->start_time, next->duration)) {
            next->is_collision = true;
        }
        
        list_propagate_delay(next, check_hours_func);
    }
}

void list_free(Event* head) {
    while (head) {
        Event* temp = head;
        head = head->next;
        free(temp);
    }
}

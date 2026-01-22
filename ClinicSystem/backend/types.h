#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define MAX_DOCTORS 2
#define DAYS_IN_WEEK 7
#define MAX_EVENTS_PER_DAY 7
#define WORK_START_MIN 540  // 09:00
#define WORK_END_MIN 1020   // 17:00

typedef enum {
    EVENT_PATIENT,
    EVENT_LUNCH,
    EVENT_BREAKFAST,
    EVENT_DINNER,
    EVENT_OTHER
} EventType;

#endif

/**
 * Copyright 2017 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include <msgpack.h>
#include <time.h>

#include "schedule.h"

static void decodeRequest(msgpack_object *deserialized, schedule_t *);

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See schedule.h for details. */
int decode_schedule(size_t count, uint8_t *bytes, schedule_t **s) {
    schedule_t *schedule;
    msgpack_zone mempool;
    msgpack_object deserialized;
    msgpack_unpack_return unpack_ret;

    if (!count || !bytes) {
        return -1;
    }

    schedule = malloc(sizeof (schedule_t));
    if (!schedule) {
        return -2;
    }

    *s = schedule;
  
    msgpack_zone_init(&mempool, 2048);
    unpack_ret = msgpack_unpack((const char *) bytes, count, NULL, &mempool, &deserialized);

    switch (unpack_ret) {
        case MSGPACK_UNPACK_SUCCESS:
            if (deserialized.via.map.size != 0) {
                decodeRequest(&deserialized, schedule);
            }
            msgpack_zone_destroy(&mempool);
            break;

        default:
            free(schedule);
            return -3;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */

/*----------------------------------------------------------------------------*/

void decodeRequest(msgpack_object *deserialized, schedule_t *t) {
    msgpack_object_kv* p = deserialized->via.map.ptr;

    (void ) t;
    
    while (deserialized->via.map.size--) {
        //msgpack_object keyType = p->key;
        //msgpack_object ValueType = p->val;
        // key name keyType.via.str.ptr
        switch (p->val.type) {
            case MSGPACK_OBJECT_ARRAY:
                /*
                    msgpack_object_array array = ValueType.via.array;
                    msgpack_object *ptr = array.ptr;
                    int num_elements = array.size;
                 *  if (!strcmp(p->key.via.str.ptr, "macs") {
                 * }
                 *                  */
                break;
            case MSGPACK_OBJECT_MAP:
                /*"weekly-schedule" and "absolute-schedule" are maps? 

                    if (!strcmp(p->key.via.str.ptr, "weekly-schedule")) {
                 * decodeMap();
                 } else if (!strcmp(p->key.via.str.ptr, "absolute-schedule") {
                 decodeMap();
                 } else {return error;}
                 */
                break;
            default:
                break;
        }
        p++;
    }

}

mac_address *create_mac_table(int count, schedule_t *t) {
    mac_address * macs = (mac_address *) malloc(count * sizeof (mac_address));

    memset(macs, 0, count * sizeof(mac_address));
    t->macs = macs;
    t->mac_count = count;

    return t->macs;
}

void insert_mac_address(mac_address *mac, size_t index, schedule_t *t) {
    /* This is just replacing, "insert" will require a list or a dynamic array */
    if (index >= t->mac_count) {
        return;
    }
    t->macs[index] = *mac;
}

void destroy_mac_table(schedule_t *t) {
    free(t->macs);
    t->macs = NULL;
}

void insert_weekly_schedule(schedule_t *t, schedule_event *e) {
    if (!(t && e)) {
        return;
    }

    if (NULL == t->reoccuring) {
        schedule_event *new_event = (schedule_event *) malloc(sizeof (schedule_event));
        *new_event = *e;
        new_event->next = NULL;
        t->reoccuring = new_event;
        return;
    }

    schedule_event *head = t->reoccuring;
    schedule_event *new_event = (schedule_event *) malloc(sizeof (schedule_event));
    *new_event = *e;
    while (head) {
        if (head->start <= new_event->start) {
            if (NULL == head->next) {
                /* End of List */
                head->next = new_event;
                new_event->next = NULL;
            }
            head = head->next;
            continue;
        }
        new_event->next = head->next;
        head->next = new_event;

        break;
    }
}

void insert_absolute_schedule(schedule_t *t, schedule_event *e) {
    if (!(t && e)) {
        return;
    }

    if (NULL == t->absolute) {
        schedule_event *new_event = (schedule_event *) malloc(sizeof (schedule_event));
        *new_event = *e;
        new_event->next = NULL;
        t->absolute = new_event;
        return;
    }

    schedule_event *head = t->absolute;
    schedule_event *new_event = (schedule_event *) malloc(sizeof (schedule_event));
    *new_event = *e;
    while (head) {
        if (head->start <= new_event->start) {
            if (NULL == head->next) {
                /* End of List */
                head->next = new_event;
                new_event->next = NULL;
            }
            head = head->next;
            continue;
        }
        new_event->next = head->next;
        head->next = new_event;

        break;
    }
}

void destroy_lsit_of_events(schedule_event *e) {
    schedule_event *temp = e;
    while (NULL != temp) {
        schedule_event *next = temp->next;
        free(temp);
        temp = next;
    }
}

uint8_t *extract_mac_addresses_for_time_window(schedule_t *t, int relative_time, int abs_time) {
    uint8_t *cp = NULL;
    struct tm calendar_time;
    time_t time_now = time(NULL);

    (void ) t; (void ) relative_time; (void ) abs_time;
    
    if (NULL == localtime_r(&time_now, &calendar_time)) {
        return cp;
    }

    return cp;
}

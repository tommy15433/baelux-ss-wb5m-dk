#include "app_zigbee_clientList.h"
#include "stdio.h"
#include "stdlib.h"

#define HEADIDX 0
#define TAILVALUE   APP_ZIGBEE_CLIENTLIST_MAXALLOC + 1
#define INVALIDIDX  -1

typedef app_zigbee_clientList_Address_t addr_t;
typedef int idx_t;

typedef struct client
{
    app_zigbee_clientList_Address_t value;
    struct client* prev;
    struct client* next;
}client_t;


static client_t* head = NULL;

static client_t* getTail(client_t* start)
{
    client_t* ptr = start;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }

    return ptr;
}

static client_t* findNode(addr_t addr) 
{
    if (head->value == addr){
        return head;
    }
    
    client_t* ptr = head;
    while (ptr != NULL) {
        if (ptr->value == addr) {
            return ptr;
        }
        ptr = ptr->next;
    }
        
    return NULL;
}

void app_zigbee_clientList_deleteAll(void)
{
    client_t* base = head;
    while (base->next != NULL) {
        client_t* ptr = base->next;
        base->next = ptr->next;
        free(ptr);
        ptr = NULL;
    }

    free(head);
    head = NULL;
}

void app_zigbee_clientList_delete(app_zigbee_clientList_Address_t addr)
{
    client_t* cur = findNode(addr);
    if (cur == head) {
        free(head);
        head = NULL;
    } else if (cur != NULL) {
        cur->prev->next = NULL;
        free(cur);
        cur = NULL;
    }
}

bool app_zigbee_clientList_add(app_zigbee_clientList_Address_t addr)
{
    if (head == NULL) {
        head = malloc(sizeof(client_t));
        head->value = addr;
        head->prev = NULL;
        head->next = NULL;
    } else {
        client_t* tail = getTail(head);
        client_t* newTail = malloc(sizeof(client_t));
        tail->next = newTail;
        newTail->prev = tail;
        newTail->next = NULL;
        newTail->value = addr;
    }

    return true;
}

bool app_zigbee_clientList_find(app_zigbee_clientList_Address_t addr)
{
    if (head == NULL) {
        return false;
    }
    client_t* ptr = findNode(addr);

    return (ptr == NULL) ? false : true;
}

void app_zigbee_clientList_debug() {
    client_t* ptr = head;

    printf("client list debug msg\r\n");
    while (ptr != NULL) {
        printf("%x\r\n", ptr->value);

        ptr = ptr->next;
    }
}

/**
 * returns 0 if empty, else returns value of app_zigbee_clientList_Address_t
*/

static client_t* ptr = NULL;
app_zigbee_clientList_Address_t app_zigbee_clientList_next() 
{
    app_zigbee_clientList_Address_t ret = 0;

    if (head == NULL) {
        return 0;
    } else {
        if (ptr == NULL) {
            ptr = head;
            ret = ptr->value;
        } else {
            if (ptr->next != NULL) {
                ptr = ptr->next;
                ret = ptr->value;
            }else {
                ptr = head;
                ret = ptr->value;
            }
        }
    }
    return ret;
}
app_zigbee_clientList_Address_t app_zigbee_clientList_cur()
{
    if (ptr == NULL){
        return 0;
    } else {
        return ptr->value;
    }
}
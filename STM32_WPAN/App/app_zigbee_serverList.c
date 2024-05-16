
#include "app_zigbee_serverList.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#define HEADIDX 0
#define TAILVALUE   APP_ZIGBEE_SERVERLIST_MAXALLOC + 1
#define INVALIDIDX  -1

typedef serverlist_addr addr_t;
typedef int idx_t;

typedef struct server
{
    serverlist_t value;
    struct server* prev;
    struct server* next;
}server_t;

static addr_t m_address_buffer[APP_ZIGBEE_SERVERLIST_MAXALLOC] = {0,};

static server_t* head = NULL;

static serverlist_time m_lastUpdatedTime = 0;

static serverlist_addr m_filter = 0;

static server_t* getTail(server_t* start)
{
    server_t* ptr = start;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }

    return ptr;
}

static server_t* findNode(addr_t addr) 
{
    server_t* ptr = head;
    while (ptr != NULL) {
        if (ptr->value.addr == addr) {
            return ptr;
        }
        ptr = ptr->next;
    }
        
    return NULL;
}

void app_zigbee_serverList_deleteAll(void)
{
    server_t* base = head;
    while (base->next != NULL) {
        server_t* ptr = base->next;
        base->next = ptr->next;
        free(ptr);
        ptr = NULL;
    }

    free(head);
    head = NULL;
}

void app_zigbee_serverList_delete(addr_t addr)
{
    server_t* cur = findNode(addr);
    if (cur == head) {
        free(head);
        head = NULL;
    } else if (cur != NULL) {
        cur->prev->next = NULL;
        free(cur);
        cur = NULL;
    }
}

bool app_zigbee_serverList_add(addr_t addr, serverlist_time timestamp)
{
    if (m_filter != 0 && addr == m_filter)
    {
        return true;
    }
    if (head == NULL) {
        head = malloc(sizeof(server_t));
        head->value.addr = addr;
        head->value.lastUpdatedTime = timestamp;
        head->prev = NULL;
        head->next = NULL;
    } else {
        server_t* tail = getTail(head);
        server_t* newTail = malloc(sizeof(server_t));
        tail->next = newTail;
        newTail->prev = tail;
        newTail->next = NULL;
        newTail->value.addr = addr;
        newTail->value.lastUpdatedTime = timestamp;
    }

    return true;
}

bool app_zigbee_serverList_find(addr_t addr)
{
    if (head == NULL) {
        return false;
    }
    server_t* ptr = findNode(addr);

    return (ptr == NULL) ? false : true;
}

static server_t* ptr = NULL;
serverlist_addr app_zigbee_serverList_next() 
{
    serverlist_addr ret = 0;

    if (head == NULL) {
        return 0;
    } else {
        if (ptr == NULL) {
            ptr = head;
            ret = ptr->value.addr;
        } else {
            if (ptr->next != NULL) {
                ptr = ptr->next;
                ret = ptr->value.addr;
            }else {
                ptr = head;
                ret = ptr->value.addr;
            }
        }
    }

    return ret;
}
serverlist_addr app_zigbee_serverList_cur()
{
    serverlist_t tmp = {0};
    if (ptr == NULL){
        if (head == NULL) {
            return 0;
        } else {
            return head->value.addr;
        }
    } else {
        return ptr->value.addr;
    }
}

// void app_zigbee_serverList_debug() {
//     server_t* ptr = head;

//     printf("server list debug msg\r\n");
//     while (ptr != NULL) {
//         printf("%x\r\n", ptr->value);

//         ptr = ptr->next;
//     }
// }

bool app_zigbee_serverList_update(addr_t addr, serverlist_time timestamp)
{
    server_t* ptr = findNode(addr);
    m_lastUpdatedTime = timestamp;

    if (ptr == NULL) {
        app_zigbee_serverList_add(addr ,timestamp);

    } else {
        ptr->value.lastUpdatedTime = timestamp;
    }

    // enumerate through addresses and check their timeouts

    ptr = head;
    while (ptr != NULL) {
        if (timestamp - ptr->value.lastUpdatedTime >= unrecognizable_timeout) {
            return false;
        }
        ptr = ptr->next;
    }

    return true;
}

uint16_t app_zigbee_clientList_getTimeouts(serverlist_addr* addresses)
{
    server_t* ptr = head;
    int size = 0;

    while (ptr != NULL) {
        serverlist_time curtime = ptr->value.lastUpdatedTime;
        if (m_lastUpdatedTime - curtime >= unrecognizable_timeout) {
            m_address_buffer[size++] = ptr->value.addr;
        }
    }

    addresses = m_address_buffer;

    return size;    
}

void app_zigbee_serverList_clearTimeouts(void)
{
    for (int i = 0; i < APP_ZIGBEE_SERVERLIST_MAXALLOC; i++) {
        if (m_address_buffer[i] == 0) {
            break;
        }
        app_zigbee_serverList_delete(m_address_buffer[i]);
    }

    memset(m_address_buffer, 0, sizeof(addr_t) * APP_ZIGBEE_SERVERLIST_MAXALLOC);
}

void app_zigbee_serverList_setFilter(serverlist_addr filter) 
{
    m_filter = filter;
}
/**
 * returns 0 if empty, else returns value of serverlist_t
*/

#include "app_zigbee_utils.h"

char m_str[64];

int char2val(char c)
{
    int ret = 0;
    
    if (c >= '0' && c <= '9') {
        ret = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        ret = (c - 'a') + 10;
    } else if (c >= 'A' && c <= 'f') {
        ret = (c - 'A') + 10; 
    } else {
        ret = -1;
    }

    // printf("char2val ret: %d\r\n", ret);
    return ret;
}
char val2char(int v)
{
    if (v >= 0 && v <= 9)
    {
        return v + '0';
    } else if (v <= 15) {
       return (v - 10) + 'a'; 
    } else {
        return 0;
    }
}
// uint64_t strToU64(char* value)
// {
//     uint64_t ret = 0;
//     for (int i = 0; i < 16; i++)
//     {
//         // char hn = char2val((value[i] & 0xf0) >> 4);
//         // char ln = char2val((value[i] & 0x0f));

//         char tmp = char2val(value[i]);
//         printf("ori %x hn %x \r\n",value[i], tmp);
//         ret = ret << 4 | tmp;
//     }
//     return ret;
// }
uint16_t strToU16(char* value)
{
    uint16_t ret = 0;

    for (int i = 0; i < 4; i++) {
        // printf("value[%d] = %c\r\n", i, value[i]);
        ret = ret * 16 + char2val(value[i]);
    }

    return ret;
}
char* u64ToStr(uint64_t value)
{
    sprintf(m_str, "%d", value);

    return m_str;
}
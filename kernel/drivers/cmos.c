#include "../include/cmos.h"
#include "../include/ports.h"

uint8_t get_cmos_status_register_A(){
    outportb(CMOS_ADDRESS,CMOS_STATUS_A);
    return inportb(CMOS_DATA);
}

uint8_t cmos_is_updating(){
    return get_cmos_status_register_A() & 0x80;
}

void cmos_wait_untill_ready(){
    while(cmos_is_updating());
}

uint8_t cmos_get_b_register(){
    outportb(CMOS_ADDRESS,0x0B);
    return inportb(CMOS_DATA);
}

uint8_t cmos_get_RTC_value(int reg){
    uint8_t breg = cmos_get_b_register();
    outportb(CMOS_ADDRESS,reg);
    uint8_t t = inportb(CMOS_DATA);
    if(!(breg&0x04))
    {
        t = (t & 0x0F) + ((t / 16) * 10);
    }
    return t;
}

uint8_t getCentury(){
    return 20;
}

uint8_t getSecond(){
    cmos_wait_untill_ready();
    return cmos_get_RTC_value(0x00);
}

uint8_t getMinute(){
    cmos_wait_untill_ready();
    return cmos_get_RTC_value(0x02);
}

uint8_t getHour(){
    cmos_wait_untill_ready();
    return cmos_get_RTC_value(0x04);
}

uint8_t getDay(){
    cmos_wait_untill_ready();
    return cmos_get_RTC_value(0x07);
}

uint8_t getMonth(){
    cmos_wait_untill_ready();
    return cmos_get_RTC_value(0x08);
}

uint8_t getYear(){
    cmos_wait_untill_ready();
    return cmos_get_RTC_value(0x09);
}

char* getMonthString(){
    uint8_t month = getMonth();
    char* months[13] = {
        "",
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "Augustus",
        "September",
        "October",
        "November",
        "December"
    };
    return months[month];
}
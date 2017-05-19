#include <cstdint>
#include <time.h>
#include "gbtime.h"

static const int secondsInDay = 24 * 60 * 60;
static const int secondsInHours = 60 * 60;
static const int interestingTimeIncrement = 256 * secondsInDay;

inline uint32_t convert_to_second(struct tm *sys);

void gbtime::set_time(int type, uint8_t dat) {

    struct tm sys{};
    time_t t = time(nullptr);
    localtime_r(&t, &sys);

    uint32_t now = convert_to_second(&sys);
    uint32_t adj = now - _cur_time;

    switch (type) {
        case 8:
            adj = (adj / 60) * 60 + (dat % 60);
            break;
        case 9:
            adj = (adj / secondsInHours) * secondsInHours + (dat % 60) * 60 + (adj % 60);
            break;
        case 10:
            adj = (adj / secondsInDay) * secondsInDay + (dat % 24) * secondsInHours + (adj % secondsInHours);
            break;
        case 11:
            adj = (adj / interestingTimeIncrement) * interestingTimeIncrement + (dat * secondsInDay) + (adj % secondsInDay);
            break;
        case 12:
            adj = (dat & 1) * interestingTimeIncrement + (adj % interestingTimeIncrement);
            break;
        default:
            break;
    }
    _cur_time = now - adj;
}

uint8_t gbtime::get_time(int type) {
    struct tm sys{};
    time_t t = time(0);
    localtime_r(&t, &sys);
    uint32_t now = convert_to_second(&sys) - _cur_time;

    switch (type) {
        case 8:
            return (uint8_t) (now % 60);
        case 9:
            return (uint8_t) ((now / 60) % 60);
        case 10:
            return (uint8_t) ((now / (3600)) % 24);
        case 11:
            return (uint8_t) ((now / (86400)) & 0xff);
        case 12:
            return (uint8_t) ((now / (256 * 86400)) & 1);
        default:
            return 0;
    }
}

uint32_t convert_to_second(struct tm *sys) {
    uint32_t i, ret = 0;
    static int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for (i = 1; i + 1950 < (uint32_t) sys->tm_year; i++) {
        if ((i & 3) == 0) {
            if ((i % 100) == 0) {
                ret += 365 + ((i % 400) == 0 ? 1 : 0);
            } else {
                ret += 366;
            }
        } else {
            ret += 365;
        }
    }

    for (i = 1; i < (uint32_t) sys->tm_mon; i++) {
        if (i == 2) {
            if ((sys->tm_year & 3) == 0) {
                if ((sys->tm_year % 100) == 0) {
                    if ((sys->tm_year % 400) == 0) {
                        ret += 29;
                    } else {
                        ret += 28;
                    }
                } else {
                    ret += 29;
                }
            } else {
                ret += 28;
            }
        } else {
            ret += month_days[i];
        }
    }

    ret += sys->tm_mday - 1;

    ret *= 24 * 60 * 60;

    ret += sys->tm_hour * 60 * 60;
    ret += sys->tm_min * 60;
    ret += sys->tm_sec;

    return ret;
}
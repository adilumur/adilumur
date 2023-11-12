#ifndef __PROGTEST__
#include <stdio.h>
#include <assert.h>
#endif /* __PROGTEST__ */



int leap_year(int year){
            if(year % 4000 == 0)
                return 0;
    else    if (year % 400 == 0) 
                return 1;
    else    if (year % 100 == 0)
                return 0;
    else    if (year % 4 == 0) 
                return 1;
    else 
                return 0;
}

int check_months(int i, int year){
    int days = 0;
    switch(i){
        case 1: days = 31; break;
        case 2: 
            if(leap_year(year))
                days = 29;
            else 
                days = 28; 
            break;
        case 3: days = 31; break;
        case 4: days = 30; break;
        case 5: days = 31; break;
        case 6: days = 30; break;
        case 7: days = 31; break;
        case 8: days = 31; break;
        case 9: days = 30; break;
        case 10: days = 31; break;
        case 11: days = 30; break;
        case 12: days = 31; break;
    }
    return days;
}



int dateToIndex(int day, int month, int year, int *idx) {
    int count_days = 0;
    if(year < 2000
       || month < 0
       || month > 12
       || day < 1
       || day > check_months(month, year)) return 0;
    if(day > 28 && month == 2 && !leap_year(year)) return 0;
    for (int i = 1; i < month; i++){
        count_days += check_months(i, year);
    }
    count_days += day;
    *idx = count_days;
    return 1;



}

#ifndef __PROGTEST__
int main (int argc, char * argv []) {
    int idx;
    assert(dateToIndex( 1,  1, 2000, &idx) == 1 && idx == 1);
    assert(dateToIndex( 1,  2, 2000, &idx) == 1 && idx == 32);
    assert(dateToIndex(29,  2, 2000, &idx) == 1 && idx == 60);
    assert(dateToIndex(29,  2, 2001, &idx) == 0);
    assert(dateToIndex( 1, 12, 2000, &idx) == 1 && idx == 336);
    assert(dateToIndex(31, 12, 2000, &idx) == 1 && idx == 366);
    assert(dateToIndex( 1,  1, 1999, &idx) == 0);
    assert(dateToIndex( 6,  7, 3600, &idx) == 1 && idx == 188);
    assert(dateToIndex(29,  2, 3600, &idx) == 1 && idx == 60);
    assert(dateToIndex(29,  2, 4000, &idx) == 0);
    return 0;
}
#endif /* __PROGTEST__ */


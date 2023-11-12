#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#endif /* __PROGTEST__ */

int sunday(int d, int m, int y){
    return ((d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7);
}

int leap_year(int year){
            if (year % 400 == 0) 
                return 1;
    else    if (year % 100 == 0)
                return 0;
    else    if (year % 4 == 0) 
                return 1;
    else 
                return 0;
}

void count_years(int y1, int m1, int d1, int h1,
                int y2, int m2, int d2, int h2, long long int * count_h, long long int * count_i){
    int sund = 0;
    int days = 0;
    for (int i = y1 + 1; i < y2; i++){
        if(sunday(1,1,i) == 0 || (leap_year(i) && sunday(1,1,i) == 6))
            sund+= 53;
        else sund += 52;
        if(leap_year(i)) days += 366;
        else days += 365;
    }
    int tmp = days - sund;
    *count_h += tmp * 156;
    *count_i += tmp * 240;
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

void count_months(int y1, int m1, int d1, int h1,
                  int y2, int m2, int d2, int h2, long long int * count_h, long long int * count_i){
    int days = 0, count_start = 0, count_end = 0;
    int tmp;
    // if within one year
    if (m1 != m2 && y1 == y2){
        for (int i = m1 + 1; i < m2; i++){
            days += check_months(i,y1);
        }

    // if diffrent years
    } else {
        for (int i = m1+1; i <= 12; i++){
            days += check_months(i,y1);
        }
        for (int i = 1; i < m2; i++){
            days += check_months(i,y2);
        }
    }
    // if within one month
    if(m1 == m2 && y1 == y2){
        days = 0;
        tmp = 0;
    }
    
    // Find first sunday
    int foo = check_months(m1+1, y1);
    for (int j = 1; j <= foo; j++){
        if(sunday(j,m1+1,y1) == 0) break;
        else count_start++;
    }

    foo = check_months(m2-1, y1);
    for (int j = foo; j > 0; j--){
        if(sunday(j,m2-1,y2) == 0) break;
        else count_end++;
    }

    tmp = days - ((days - count_start - count_end)/7);
    *count_i += (tmp-2) * 240;
    *count_h += (tmp-2) * 156;
}

// Days count within months
void count_days(int y1, int m1, int d1, int h1,
                int y2, int m2, int d2, int h2, long long int * count_h, long long int * count_i){
    int days = 0;

    if((m2 != m1 && y1 == y2) || (m1 == m2 && y1 != y2) || (m1 != m2 && y1 != y2)){
        int days_m1 = check_months(m1,y1);
        for (int i = d1 + 1; i <= days_m1; i++){
            if(sunday(i,m1,y1) == 0) continue;
            else days++;
        }
        for (int i = 1; i <= d2; i++){
            if(sunday(i,m2,y2) == 0) continue;
            else days++;
        }
    
    } else {
        for(int i = d1 + 1; i <= d2; i++){
            if(sunday(i,m1,y1) == 0) continue;
            else days++;
        }
    }
    *count_h += days * 156; // 156 - const value, 156 knoks within one day every hour
    *count_i += days * 240; // 240 - const value, 240 knoks within one day every minet
}

int count_hours(int y1, int m1, int d1, int h1, int i1,
                int y2, int m2, int d2, int h2, int i2){
    
    // Knoks amount every hour
    // 00 = 12 ударов
    // 01 = 1 удар
    // ...
    // 13 = 1 удар
    // ...
    // 24 = 00 = 12 ударов
    int bells[24] = {12,1,2,3,4,5,6,7,8,9,10,11,12,1,2,3,4,5,6,7,8,9,10,11};
    int count_h = 0;
    // if first date is sunday
    if(sunday(d1,m1,y1) == 0){
        if(i1 == 0) h1--;
        // cals only second date hours
        for (int i = 0; i <= h2; i++){
            count_h += bells[i];
        }
    } 
    // if second date is sunday
    else if(sunday(d2,m2,y2) == 0){
        // calc only first date hours
        if(i1 == 0) h1--;
        for (int i = h1 + 1; i < 24; i++){
            count_h += bells[i];
        }
    } else {


// if both dates are not sundays
        if(h1 > h2 || d1 != d2){
            if(i1 == 0) h1--;
            for (int i = h1 + 1; i < 24; i++){
                count_h += bells[i];
            }
            for (int i = 0; i <= h2; i++){
                count_h += bells[i];
            }
        } else {
            if(i1 != 0)
                for (int i = h1 + 1; i <= h2; i++){
                    count_h += bells[i];
                }
            else
                for (int i = h1; i <= h2; i++){
                    count_h += bells[i];
                }
        }
    }
    if(sunday(d1,m1,y1) == 0 && sunday(d2,m2,y2) == 0) count_h = 0;
    return count_h;
}

void check_minutes(int j, int * count){
    if(j == 0) *count += 4;
    if(j == 15) *count += 1;
    if(j == 30) *count += 2;
    if(j == 45) *count += 3;
}

int count_minutes ( int y1, int m1, int d1, int h1, int i1,
                    int y2, int m2, int d2, int h2, int i2){
    
    int count_h = 0, count_i = 0;
    
    while(i1 % 15 != 0){
        if(i1 == 0) break;
        i1++;
    }

    while(i2 % 15 != 0){
        if(i2 == 0) break;
        i2--;
    }

    if(sunday(d1,m1,y1) == 0){
        count_h = h2 + 1;
        for (int j = 0; j <= i2; j+=15){
            check_minutes(j, &count_i);
        }
    }
    else if(sunday(d2,m2,y2) == 0){
        count_h = (24 - h1);
        for (int j = i1; j < 60; j+=15){
            check_minutes(j, &count_i);
        }   
    } else {

        for (int j = i1; j < 60; j+=15){
            check_minutes(j, &count_i);
        }    
        for (int j = 0; j <= i2; j+=15){
            check_minutes(j, &count_i);
        }


        if(h1 > h2 || d1 != d2){
            count_h = (24 - h1) + h2;
        } else {
            count_h = h2 - h1;
        }
    }

    count_h = (count_h - 1) * 10; // within one hour 10 knocks minute
    count_i += count_h;
    if(sunday(d1,m1,y1) == 0 && sunday(d2,m2,y2) == 0) count_i = 0; 
    return count_i;
}

int check(int y1, int m1, int d1, int h1, int i1,
          int y2, int m2, int d2, int h2, int i2){
    if(y1 < 1600
        || m1 < 1
        || m1 > 12
        || d1 < 1
        || d1 > check_months(m1,y1)
        || h1 < 0
        || h1 > 23
        || i1 < 0
        || i1 > 59
    ) return 0;
    else if(y2 < 1600 
        || m2 < 1
        || m2 > 12
        || d2 < 1
        || d2 > check_months(m1,y1)
        || h2 < 0
        || h2 > 23
        || i2 < 0
        || i2 > 59
    ) return 0;   
    else
    if(y1 > y2
     || (m1 > m2 && y1 == y2)
     || (d1 > d2 && m1 == m2 && y1 == y2)
     || (h1 > h2 && d1 == d2 &&  m1 == m2 && y1 == y2)
     || (i1 > i2 && h1 == h2 && d1 == d2 &&  m1 == m2 && y1 == y2)
      ) return 0;
    else 
    if(m1 == 2 && !leap_year(y1) && d1 > 28) return 0; 
    else if(m2 == 2 && !leap_year(y2) && d2 > 28) return 0;
    else return 1;

}


            // year   month   day    hour    minutes
int bells ( int y1, int m1, int d1, int h1, int i1,
            int y2, int m2, int d2, int h2, int i2,
            long long int * b1, long long int * b2 ){
    
    long long int count_i = 0, count_h = 0;  

    int tmp = check(y1, m1, d1, h1, i1, y2, m2, d2, h2, i2);
    if(tmp == 0) return tmp;

    count_i += count_minutes(y1, m1, d1, h1, i1, y2, m2, d2, h2, i2);
    count_h += count_hours(y1, m1, d1, h1, i1, y2, m2, d2, h2, i2); 
    if(m1 != m2 || y1 != y2 || (d2 - d1 != 1 && d1 != d2)){
    
        count_days(y1, m1, d1, h1, y2, m2, d2, h2, &count_h, &count_i); 
        count_months(y1, m1, d1, h1, y2, m2, d2, h2, &count_h, &count_i);
        count_years(y1, m1, d1, h1, y2, m2, d2, h2, &count_h, &count_i);
    }

        *b1 = count_i;
        *b2 = count_h;
        return tmp;
}


#ifndef __PROGTEST__
int main ( int argc, char * argv [] ){
  long long int b1, b2;

  assert ( bells ( 2022, 10,  1, 13, 15,
                   2022, 10,  1, 18, 45, &b1, &b2 ) == 1
           && b1 == 56
           && b2 == 20 );
  assert ( bells ( 2022, 10,  3, 13, 15,
                   2022, 10,  4, 11, 20, &b1, &b2 ) == 1
           && b1 == 221
           && b2 == 143 );
  assert ( bells ( 2022, 10,  1, 13, 15,
                   2022, 10,  2, 11, 20, &b1, &b2 ) == 1
           && b1 == 106
           && b2 == 65 );
  assert ( bells ( 2022, 10,  2, 13, 15,
                   2022, 10,  3, 11, 20, &b1, &b2 ) == 1
           && b1 == 115
           && b2 == 78 );
  assert ( bells ( 2022, 10,  1, 13, 15,
                   2022, 10,  3, 11, 20, &b1, &b2 ) == 1
           && b1 == 221
           && b2 == 143 );
  assert ( bells ( 2022,  1,  1, 13, 15,
                   2022, 10,  5, 11, 20, &b1, &b2 ) == 1
           && b1 == 56861
           && b2 == 36959 );
  assert ( bells ( 2019,  1,  1, 13, 15,
                   2019, 10,  5, 11, 20, &b1, &b2 ) == 1
           && b1 == 57101
           && b2 == 37115 );
  assert ( bells ( 2024,  1,  1, 13, 15,
                   2024, 10,  5, 11, 20, &b1, &b2 ) == 1
           && b1 == 57341
           && b2 == 37271 );
  assert ( bells ( 1900,  1,  1, 13, 15,
                   1900, 10,  5, 11, 20, &b1, &b2 ) == 1
           && b1 == 57101
           && b2 == 37115 );
  assert ( bells ( 2022, 10,  1,  0,  0,
                   2022, 10,  1, 12,  0, &b1, &b2 ) == 1
           && b1 == 124
           && b2 == 90 );
  assert ( bells ( 2022, 10,  1,  0, 15,
                   2022, 10,  1,  0, 25, &b1, &b2 ) == 1
           && b1 == 1
           && b2 == 0 );
  assert ( bells ( 2022, 10,  1, 12,  0,
                   2022, 10,  1, 12,  0, &b1, &b2 ) == 1
           && b1 == 4
           && b2 == 12 );
  assert ( bells ( 2022, 11,  1, 12,  0,
                   2022, 10,  1, 12,  0, &b1, &b2 ) == 0
           && b1 == 4
           && b2 == 12 );
  assert ( bells ( 2022, 10, 32, 12,  0,
                   2022, 11, 10, 12,  0, &b1, &b2 ) == 0
           && b1 == 4
           && b2 == 12 );
  assert ( bells ( 2100,  2, 29, 12,  0,
                   2100,  2, 29, 12,  0, &b1, &b2 ) == 0
           && b1 == 4
           && b2 == 12 );
  assert ( bells ( 2000,  2, 29, 12,  0,
                   2000,  2, 29, 12,  0, &b1, &b2 ) == 1
           && b1 == 4
           && b2 == 12 );
  assert ( bells ( 2004,  2, 29, 12,  0,
                   2004,  2, 29, 12,  0, &b1, &b2 ) == 1
           && b1 == 0
           && b2 == 0 );
  
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

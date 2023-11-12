#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(){
    double x,y;
    char sign, equal_sign;
    printf("Zadejte vzorec:\n");
    if(scanf("%lf %c %lf %c", &x, &sign, &y, &equal_sign)!= 4 || (sign == '/' && y == 0) || equal_sign != '='){
        printf("Nespravny vstup.\n");
        return 1;
    }
    switch(sign){
        case '+':
            printf("%g\n", (x + y));
            break;
        case '-':
            printf("%g\n", (x - y));
            break;
        case '*':
            printf("%g\n", (x * y));
            break;
        case '/':
            printf("%g\n", trunc(x / y));
            break;
        default:
            printf("Nespravny vstup.\n");
            return 1;
    }
    return 0;
}

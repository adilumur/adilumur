#include <stdio.h>
#include <string.h>

int digits (int a){
    int dig = 0;
    while(a > 0){
        a /= 10;
        dig++;
    }
    return dig;
}

int main(){
    int x;
    printf("Rozsah:\n");
    if(scanf("%d", &x) != 1 || x <= 0){
        printf("Nespravny vstup.\n");
        return 0;
    }
    int Xspaces = digits(x*x) + 1;
    int Yspaces = Xspaces - 1;
    int tmp = 0;
    
    printf("%*s", Yspaces, "");
    printf("|");
    
    for (int i = x; i > 0; i--){
        printf("%*d", Xspaces, i);
    }
    printf("\n");
    for (int i = 0; i < Yspaces; i++, printf("%c", '-'));
    printf("+");
    for (int i = 0; i < (Xspaces * x) ; i++, printf("%c", '-'));
    printf("\n");

    for (int i = 1; i <= x; i++){
        printf("%*d|", Yspaces, i);
        for (int j = x; j > tmp; j--){
            printf("%*d", Xspaces, i * j);
        }
        tmp++;
        printf("\n");        
    }
    return 0;
}

#include <stdio.h>

int main(){
    int x,y,z;
    char c;
    printf("Zadejte barvu v RGB formatu:\n");
    if(scanf(" rgb ( %d , %d , %d %c", &x, &y, &z, &c) != 4 
        || x < 0 || y < 0 || z < 0 
        || x > 255 || y > 255 || z > 255){
        printf("Nespravny vstup.\n");
        return 1;
    }
    printf("#%02X%02X%02X\n", x,y,z);
    return 0;
}

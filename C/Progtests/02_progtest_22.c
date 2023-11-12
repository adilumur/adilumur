#include <stdio.h>
#include <stdlib.h>
#include <float.h>   
#include <math.h>

int compare(double a, double b){
    double tmp = fabs(a - b);
    a = fabs(a);
    b = fabs(b);
    const double epsilon = DBL_EPSILON * fmax(a, b) * 1000;
    return tmp <= epsilon;
}

double midFunc(double a, double b, double c){
    double mid;
    if((a > b && a < c) || (a < b && a > c))
        mid = a;
    else if ((b > a && b < c) || (b < a && b > c))
        mid = b;
    else /*if ((c > a && c < b) || (c < a && c > b))*/ 
        mid = c;
    return mid;
}

int main(){
    // variables:
    double aX, aY, bX, bY, cX, cY;
    
    // inputs:
    printf("Bod A:\n");
    if(scanf(" %lf %lf", &aX, &aY) != 2){
        printf("Nespravny vstup.\n");
        return 1;
    }
    printf("Bod B:\n");
    if(scanf(" %lf %lf", &bX, &bY) != 2){
        printf("Nespravny vstup.\n");
        return 1;
    }
    printf("Bod C:\n");
    if(scanf(" %lf %lf", &cX, &cY) != 2){
        printf("Nespravny vstup.\n");
        return 1;
    }

    // check for equals:
    if((compare(aX, bX) && compare(aY, bY)) ||
       (compare(bX, cX) && compare(bY, cY)) ||
       (compare(cX, aX) && compare(cY, aY)) ){
        printf("Nektere body splyvaji.\n");
        return 0;
    }
    
    // start calc:
    double mid = 0;
    if(compare((aY - bY) * (aX - cX), (aY - cY) * (aX - bX))){
        printf("Body lezi na jedne primce.\n");
        if(compare(aX,bX) && compare(bX,cX)){
            mid = midFunc(aY, bY, cY);
            if(compare(mid, aY)){
                printf("Prostredni je bod A.\n");
                return 0;
            }
            if(compare(mid, bY)){
                printf("Prostredni je bod B.\n");
                return 0;
            }
            if(compare(mid, cY)){
                printf("Prostredni je bod C.\n");
                return 0;
            }

        } else {
            if(compare(aY,bY) && compare(bY, cY)){
                mid = midFunc(aX, bX, cX);
                if(compare(mid, aX)){
                    printf("Prostredni je bod A.\n");
                    return 0;
                }
                if(compare(mid, bX)){
                    printf("Prostredni je bod B.\n");
                    return 0;
                }
                if(compare(mid, cX)){
                    printf("Prostredni je bod C.\n");
                    return 0;
                }
            } else {
                mid = midFunc(aX, bX, cX);
            }
        }
        
        if(compare(mid, aX)){
            printf("Prostredni je bod A.\n");
            return 0;
        }
        if(compare(mid, bX)){
            printf("Prostredni je bod B.\n");
            return 0;
        }
        if(compare(mid, cX)){
            printf("Prostredni je bod C.\n");
            return 0;
        }
    } 
    else printf("Body nelezi na jedne primce.\n");
    
    return 0;
}

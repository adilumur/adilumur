#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

//-----------------------------STRUCT----------------------------------
typedef struct {
    double *x;
    double *y;
    size_t size, cap;  
    char **name;
}Plane;

void init_Plane(Plane *a){
    a->x = a->y = NULL;
    a->name = NULL;
    a->size = a->cap = 0;
}

void destroy_Plane(Plane *a){
    free(a->x);
    free(a->y);

    for (size_t i = 0; i < a->size; i++) 
        free(a->name[i]);                    
    if(a->size > 1) free(a->name[a->size]); 
    
    free(a->name);
}
//---------------------------------------------------------------------

//------------------------------READ-----------------------------------
void append(Plane *a, double tmpX, double tmpY, char *buffer){
    if(a->size >= a->cap){
        a->cap = a->cap + 10000;
        a->x = (double*)realloc(a->x, a->cap * sizeof(double)); // |
        a->y = (double*)realloc(a->y, a->cap * sizeof(double)); // | -> allocate memmory if it needs
        a->name = (char**)malloc(a->cap * sizeof(char*));       // |
    }

    int length = 0;
    length = strlen(buffer);
    buffer[length] = '\0';
    a->name[a->size] = (char*)malloc((length+1) * sizeof(char)); // allocate memmory for airplane name
    
    strcpy(a->name[a->size], buffer); // |
    a->x[a->size] = tmpX;             // | -> filling arrays 
    a->y[a->size] = tmpY;             // |
    
    a->size++;
}

int read(Plane *a){
    int check;
    double tmpX, tmpY;  
    char buffer[100000];   
    int control = 0;    // need for check how much planes was write in input

    // loop will read data until EOF
    while(1){
        check = scanf("%lf, %lf: %s", &tmpX, &tmpY, buffer);
        if(check != 3) {
            if(control > 1){
                if(a->size != 0) a->size--;
            }
            else return 0; // EXIT_FAILURE
            return check == EOF;
        }

        // check if airplane name have characters '.' or ':', then EXIT_FAILURE
        char *pValue = strchr(buffer, ',');
        if(pValue != NULL) goto fail;
        pValue = strchr(buffer, ':');
        if(pValue != NULL) goto fail;
    
        append(a, tmpX, tmpY, buffer);
        control++;
    }

    fail:
    if(a->size != 0) a->size--;
    return 0; // EXIT_FAILURE



    if(a->size != 0) a->size--;
    return 1; // EXIT_SUCCESS
}
//---------------------------------------------------------------------

//------------------------------CALC-----------------------------------

double compareEqualMax(double a, double b){         // if distance will be smallest numbers (0.0000000001 for example)
    double tmp = fabs(a - b);
    a = fabs(a);
    b = fabs(b);
    const double epsilon = DBL_EPSILON * fmax(a, b) * 10e9;
    return tmp <= epsilon;
}

double compareEqualMin(double a, double b){         // if distance will be largest numbers (10e6 for example)
    double tmp = fabs(a - b);
    a = fabs(a);
    b = fabs(b);
    const double epsilon = DBL_EPSILON * fmax(a, b) * 10e-9;
    return tmp <= epsilon;
}

double distance(double x1, double x2, double y1, double y2){
    double dist = ((x2 - x1) * (x2 - x1))\
                + \
                ((y2 - y1) * (y2 - y1));
    return sqrt(dist);
}


void calc(Plane a){
    // there is 3 same double loops, diffrents only inside of loops (if, calc, compare, printf)
    // loops are for compare one by one
    
    // first loops, find shortest distance between airplanes
    double min = 10e10;
    double tmp = 0;
    for (size_t c = 0; c < a.size; c++){
        for (size_t b = c+1; b <= a.size; b++){
            tmp = distance(a.x[c], a.x[b], a.y[c], a.y[b]);
            if(compareEqualMax(tmp, min) || tmp <= min || compareEqualMin(tmp, min))
                min = tmp;
        }
    }
    if(tmp == 0) min = 0;
    printf("Vzdalenost nejblizsich letadel: %f\n", min);
    
    // second loops, calculate how much airplanes have shotrest distence between each other
    int foo = 0;
    for (size_t c = 0; c < a.size; c++){
        for (size_t b = c+1; b <= a.size; b++){
            tmp = distance(a.x[c], a.x[b], a.y[c], a.y[b]);
            if(compareEqualMax(tmp, min) || compareEqualMin(tmp, min))
                foo++;
        }
    }
    printf("Nalezenych dvojic: %d\n", foo);
    // third loops, that is for printf all airplanes with shortest distance
    for (size_t c = 0; c < a.size; c++){
        for (size_t b = c+1; b <= a.size; b++){
            tmp = distance(a.x[c], a.x[b], a.y[c], a.y[b]);
            if(compareEqualMax(tmp, min) || compareEqualMin(tmp, min))
                printf("%s - %s\n", a.name[c], a.name[b]); 
        }
    }
}
//---------------------------------------------------------------------

int error(Plane a);

int main(){
    Plane a;
    printf("Pozice letadel:\n");
    init_Plane(&a);
    if(!read(&a)){
        return error(a);    
    } 
    else calc(a);


    destroy_Plane(&a);
    return 0;
}

int error(Plane a){
    printf("Nespravny vstup.\n");
    destroy_Plane(&a);
    return 1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//---------------------------------- STRUCT --------------------------------------------------
typedef struct{
    char **product_name;
    char **product_name_lowwer;
    int *length;
    size_t size;
    size_t cap;
}PRODUCT;

void init_struct(PRODUCT *product, int *size, int *cap){
    for(int i = *size; i < *cap; i++){
        product[i].product_name = NULL;
        product[i].product_name_lowwer = NULL;
        product[i].length = 0;
        product[i].size = 0;
        product[i].cap = 0;
    }
}
void destroy_product(PRODUCT *product){
    for(size_t i = 0; i < product->size; i++){
        free(product->product_name[i]);
        free(product->product_name_lowwer[i]);
    }
}
void destroy_struct(PRODUCT *product, int size, int cap){
    for(int i = size; i < cap; i++){
        destroy_product(&product[i]);
        free(product[i].product_name);
        free(product[i].product_name_lowwer);
        free(product[i].length);
    }
    free(product);
}
void append(PRODUCT *product, int *size, int *cap, char buffer[], int length){
    if(*size >= *cap){
        *cap += 10000;
        product = (PRODUCT*)realloc(product, *cap * sizeof(PRODUCT));
        init_struct(product, size, cap); 
    }
    if(product[*size].size >= product[*size].cap){
        product[*size].cap = product[*size].cap + 10000;
        product[*size].product_name = (char**)realloc(product[*size].product_name , product[*size].cap * sizeof(char*));
        product[*size].length = (int*)realloc(product[*size].length, product[*size].cap * sizeof(int));
        product[*size].product_name_lowwer = (char**)realloc(product[*size].product_name_lowwer , product[*size].cap * sizeof(char*));    
    }

    product[*size].product_name[product[*size].size] \
            = \
        (char*)malloc((length) * sizeof(char));
    strcpy(product[*size].product_name[product[*size].size], buffer);

    for (int i = 0; i < length; i++)
        buffer[i] = tolower(buffer[i]);

    product[*size].product_name_lowwer[product[*size].size] \
            = \
        (char*)malloc((length) * sizeof(char));
    strcpy(product[*size].product_name_lowwer[product[*size].size], buffer);
    product[*size].length[product[*size].size] = length;
   
    product[*size].size++;
}
//--------------------------------------------------------------------------------------------
//------------------------------------ READ --------------------------------------------------
int read_start(PRODUCT *cell, int *size_cell){
    int num = 0;
    char buffer[100000];
    fgets(buffer, sizeof(buffer), stdin);
    int length = strlen(buffer);
    if(buffer[length] == EOF || buffer[length - 1] == EOF ) return 0;
    buffer[length - 1] = '\0';
    if(buffer[0] == '#'){
        for(int i = 1; i < length - 1; i++){
            if(isdigit(buffer[i]))
                buffer[i-1] = buffer[i];
            else 
                return 0;
            if(i == length-2) buffer[i] = '\0';
        }
        num = atoi(buffer);
        if(num != 0) return 0;
        *size_cell = num;
    } 
    else return 0;
    return 1;
}

int read(PRODUCT *cell, PRODUCT *list, int *size_cell, int *cap_cell, int *size_list, int *cap_list){
    int num = 0;                
    char buffer[100000];        
    int length = 0;             
    char *control;              
    while(1){
        control = fgets(buffer, sizeof(buffer), stdin);
        if(feof(stdin)) return 0; // out(error) ->
        
        if(control[0] == '\n') break;
        if(control == NULL) return 0;

        length = strlen(buffer);
        if(buffer[length - 1] == EOF) return 0;
        if(buffer[length - 1] != '\n') return 0;
        buffer[length - 1] = '\0';
        if(buffer[0] == '#'){
            for(int i = 1; i < length - 1; i++){
                if(isdigit(buffer[i]))
                    buffer[i-1] = buffer[i];
                else 
                    return 0;                       // out(error) ->

                if(i == length-2) buffer[i] = '\0'; 
            }
            num = atoi(buffer);
            *size_cell += 1;
            if(num != *size_cell) return 0;          // out(error) ->
        } 
        else append(cell, size_cell, cap_cell, buffer, length);
        // memset(buffer, 0, length);
        length = 0;
        num = 0;
    }
    length = 0;
    num = 0;
    int blabla = 0;     
    while(1){
        control = fgets(buffer, sizeof(buffer), stdin);
        if(feof(stdin) && control == NULL) break;

        if(buffer[0] == '#') return 0;
        if(control[0] == '\n'){ 
            blabla++;
            if(blabla >= 1) return 0;
            *size_list += 1;
            continue;
        }
        blabla--;
        length = strlen(buffer);    
        if(buffer[length - 1] == '\n'){buffer[length - 1] = '\0';
            append(list, size_list, cap_list, buffer, length);
            // memset(buffer, 0, length);
        }
        else {
            buffer[length] = '\0';
            append(list, size_list, cap_list, buffer, length+1);
            // memset(buffer, 0, length+1);
        }
        if(feof(stdin)) break; // out(succeses) ->
    }
    return 1;
}
//--------------------------------------------------------------------------------------------
//------------------------------------ CALC --------------------------------------------------
int product_in_other_cell(PRODUCT *cell, char *tmp_list, int size_cell, int ii, int length_list){
    int length_cell = 0;
    char dup_cell[100000];

    for (int i = 0; i <= size_cell; i++){
        for (size_t j = 0; j < cell[i].size; j++){
            length_cell = cell[i].length[j];
            if(length_cell == length_list){
                strcpy(dup_cell, cell[i].product_name_lowwer[j]); 
                if(!strcmp(dup_cell, tmp_list)) return 1; // EXIT_SUCCESS
            }
            memset(dup_cell,0,length_cell);                    
        }
    }
    return 0;
}




void calc(PRODUCT *cell, PRODUCT *list, int size_cell, int start_cell, int size_list, int start_list){
    int length_list = 0, length_cell = 0;
    char dup_cell[100000]; 
    char dup_list[100000]; 

    // LISTS
    for (int i = start_list; i <= size_list; i++){
        int control = 0;
        int index[100000] = {0};
        printf("Optimalizovany seznam:\n");
        // CELLS
        for (int ii = start_cell; ii <= size_cell; ii++){
            // LISTS PRODUCTS
            for (size_t j = 0; j < list[i].size; j++){
                if(index[j] == 0){
                    // CELLS PRODUCTS
                    for (size_t jj = 0; jj < cell[ii].size; jj++){

                        strcpy(dup_list, list[i].product_name_lowwer[j]);
                        length_list =  list[i].length[j];                   
                            
                        strcpy(dup_cell, cell[ii].product_name_lowwer[jj]); 
                        length_cell =  cell[ii].length[jj];

                        if(strcmp(dup_cell, dup_list) == 0){
                            printf(" %d. %s -> #%d %s\n", control,list[i].product_name[j], ii, cell[ii].product_name[jj]);
                            memset(dup_cell,0,length_cell);                    
                            index[j]++;
                            control++;
                            continue;
                        }
                        else if(product_in_other_cell(cell, dup_list, size_cell, ii, length_list)) {
                            memset(dup_cell,0,length_cell);                    
                            continue;
                        }
                        else if(strstr(dup_cell, dup_list) && index[j] == 0){
                            printf(" %d. %s -> #%d %s\n", control, list[i].product_name[j], ii, cell[ii].product_name[jj]);
                            index[j]++;
                            control++; 
                        }
                        memset(dup_cell,0,length_cell);                    
                    }
                    memset(dup_list,0,length_list);
                }
            }
        }
        
        for (size_t j = 0; j < list[i].size; j++){
            if(index[j] == 0) {
                printf(" %d. %s -> N/A\n", control, list[i].product_name[j]);
                control ++;
            }
        }
    }


     
}
//--------------------------------------------------------------------------------------------

void error(PRODUCT *cell, PRODUCT *list, int size_cell, int cap_cell, int size_list, int cap_list);
// void print(PRODUCT *product, int size, int cap);

int main(){
    PRODUCT *cell;
    PRODUCT *list;
    int cap_cell = 1e4;
    int size_cell = 0;
    int cap_list = 1e4;
    int size_list = 0;
    cell = (PRODUCT*)malloc(cap_cell * sizeof(PRODUCT));
    list = (PRODUCT*)malloc(cap_list * sizeof(PRODUCT));

// always start with "#{number}", we must get first size_cell
    if(!read_start(cell, &size_cell)){
        error(cell, list, size_cell, cap_cell, size_list, cap_list);
        return 1;
    }

    init_struct(cell, &size_cell, &cap_cell);
    init_struct(list, &size_list, &cap_list);

    int tmp_size_cell = size_cell;


    if(!read(cell, list, &size_cell, &cap_cell, &size_list, &cap_list)){
        error(cell, list, tmp_size_cell, cap_cell, size_list, cap_list);
        return 1;
    }
    else calc(cell, list, size_cell, tmp_size_cell, size_list, 0);   



    
    destroy_struct(cell, 0, cap_cell);
    destroy_struct(list, 0, cap_list);
    
    return 0;
}

void error(PRODUCT *cell, PRODUCT *list, int size_cell, int cap_cell, int size_list, int cap_list){
    printf("Nespravny vstup.\n");
    destroy_struct(cell, size_cell, cap_cell);
    destroy_struct(list, size_list, cap_list);
}
// void print(PRODUCT *product, int size, int cap){
//     for(int i = size; i < 5; i++){
//         printf("\n");
//         printf("#%d\n", i);
//         for(size_t j = 0; j < product[i].size; j++){
//             printf("%s\n", product[i].product_name[j]);
//         }
//     }
// }

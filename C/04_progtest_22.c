#include <stdio.h>
#include <stdlib.h>

void swap(int* xp, int* yp){
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
 
void bubbleSort(int arr[], int n){
    int i, j;
    for (i = 0; i < n - 1; i++) 
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                swap(&arr[j], &arr[j + 1]);
}

/// @brief 
/// @param arr - input
/// @param count - variable, we start from it 
/// @param size - numbers amount
/// @param sums - an array into which we drive the sum of the elements of the array arr
/// @param size_sums - needed to write new elements to the sums array

void calc(int arr[], int count, int size, int sums[], int *size_sums){
    int sum = 0;      
    int tmp = *size_sums;
    // let's go through the array (for example, we had an array [1 5 2 4 2 2 2], we first get [1 5] sum = 6, then [1 5 2] sum = 8, etc.)
    for (int j = count + 1; j < size; j++){
        sum = 0;
        for (int i = count; i <= j; i++){
            sum += arr[i];
        }
        sums[tmp] = sum;
        tmp++;
    }
    *size_sums = tmp;
}


int main(){
    int arr[2010];
    int j = 0;
    int check = 1;
    printf("Posloupnost:\n");
    // read and check input
    while(check == 1 || check != EOF){
        check = scanf("%d", &arr[j]);
        if(j > 2000){
            printf("Nespravny vstup.\n");
            return 1;
        }
        j++;
    };
    j--;
    if(j <= 0){
        printf("Nespravny vstup.\n");
        return 1;
    }
    // end check input
    
    // start calculations
    int sums[2000000];
    int size = 0; 
    int dup = 0;
    for (int i = 0; i < j; i++){
        calc(arr, i, j, sums, &size);
    }
    bubbleSort(sums, size);
    int tmp = 1;
    for (int i = 0; i < size - 1; i++){
        if(sums[i] == sums[i+1]){
            tmp++;
        }
        else if(sums[i] != sums[i+1]){
            dup += (tmp * (tmp- 1))/2;
            tmp = 1;
        }
    }
    printf("Pocet dvojic: %d\n", dup);

    return 0;
}

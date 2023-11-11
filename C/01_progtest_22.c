#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int main()
{
    double kredit, debit;
    int day, maney;
    double res_money = 0;

    printf("Zadejte kreditni urok [%%]:\n");
    
    if (scanf("%lf", &kredit) != 1 || kredit <= 0){    
        printf("Nespravny vstup.\n");    
        return EXIT_FAILURE;
    }
    kredit = kredit / 100;
    
    printf("Zadejte debetni urok [%%]:\n");
    
    if (scanf("%lf", &debit) != 1 || debit <= 0){
        printf("Nespravny vstup.\n");
        return EXIT_FAILURE;
    }
    
    debit = debit / 100;
    
    int check_days = 0;
    printf("Zadejte transakce:\n");
    if (scanf("%d, %d", &day, &maney) != 2 || check_days > day){
        printf("Nespravny vstup.\n");
        return EXIT_FAILURE;
    }
    if (maney == 0) return 0;
    res_money += (double)maney;
    check_days = day;
    while (true){
        if (scanf("%d, %d", &day, &maney) != 2 || check_days > day){
            printf("Nespravny vstup.\n");
            return EXIT_FAILURE;
        }    
        if (res_money > 0){        
            for (int i = check_days; i < day; i++){
                res_money += trunc((res_money * kredit) * 100) / 100;
            }    
        } else {
            for (int i = check_days; i < day; i++){        
                res_money += trunc((res_money * debit) * 100) / 100;
            }
        }
        if (maney == 0) break;    
        
        res_money += (double)maney;
        check_days = day;
        
    }
    printf("Zustatek: %.2f\n", res_money);
    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <stdlib.h>
/*
    determines if a credit card number is valid or not and which type of credit card it belongs to if so.
*/

int main (void){
    
    // get user input
    long long card_no;
    printf("Number: ");
    while(1){
        card_no = get_long_long();
        if (card_no > 0){
            break;
        } // else
        printf("Retry: ");
    }
    

    /* determine cardtype */
    char card_str[20]; 
    sprintf(card_str, "%lld", card_no);
    int len = strlen(card_str);
    
    // get first two numbers of card (adjust for ascii)
    int x = card_str[0] - 48;
    int y = card_str[1] - 48;

    char type[12];
    if ((len == 13 || len == 16) && x == 4){
        strcpy(type, "VISA");
    } else if (len == 16 && x == 5 && (y >= 1 && y <= 5)){
        strcpy(type, "MASTERCARD");
    } else if (len == 15 && x == 3 && (y == 4 || y == 7)){
        strcpy(type, "AMEX");
    } else {
        printf("INVALID\n");
        return 0;
    }
   
   
    /* checksum */
    bool odd = true;
    int checksum = 0;
    
    // collecting both odds and even indexed digits (working from the back of card_no) in one loop.
        // the odd indexed digits are added to the checksum unmodified
        // the even indexed digits will be multiplied by two and the digits of that sum added to the checksum;  
    while (card_no){
        int num = card_no % 10;
        card_no /= 10;
    
        if(odd){
            odd = false;
        } else {
            num *= 2;
            // get product of digits if greater than two numbers.
            if (num >= 10) {
                int temp = num % 10;
                temp += num /= 10;
                num = temp;
            }
            odd = true;
        }
        
        checksum += num;
    }
    
    // validate
    if (checksum % 10 != 0) {
        printf("INVALID\n");
    } else {
        printf("%s\n", type);
    }
    return 0;

}
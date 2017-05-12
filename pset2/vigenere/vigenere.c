#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

/*
    a program that encrypts messages using Vigenère’s cipher.
    Takes keyword as command line argument and prompts user for word to encrypt.
*/

int shift(int c, int n);

int main (int args, char *argv[]) {
    // return error if number of arguments is wrong
    if (args != 2){
        printf("ERROR! One keyword should be supplied\n");
        return 1;
    }
    
    // get key
    string key = argv[1];
    int key_len = strlen(key);
    
    // return error if key contains non-alpha chars.
    for (int i = 0; i < key_len; i++){
        if (!isalpha(key[i])){
            printf("ERROR! KEY must be a word containing only letters\n");
            return 1;
        } 
    }

    // SHOWTIME    
    // get string to process
    printf("plaintext: ");
    string s = get_string();
    int l = strlen(s);
    
    printf("ciphertext: ");
    
    // loop and alter
    // i is the input string index. j is the key string index.
    for (int i = 0, j = 0; i < l; i++){
        char c = s[i];
        if (isalpha(c)) {
             
            int shift_n = toupper(key[j])-65;   // Aligns A, a to 1, B, b to 2...
            printf("%c", shift(s[i], shift_n));
            
            // stop the key index (j) sliding off the end.
            if(j == key_len - 1) {
                j = 0;
            } else {
                j++;
            }    
            
        } else {    // pass non-alphabetic characters unmolested.
            printf("%c", c);
        }
        
    }
    printf("\n");    
    return 0;
}


int shift(int c, int n){
    /* decerns case of char(int) passed to it and shifts accordingly*/
    int upper;
    
    // find upper limit as to what a characters value could be
    if(isupper(c)){
        upper = 91;
    } else {            // lower
        upper = 122;
    }

    c += n;
    
    // shift character adjustment has sent value off the end of the alphabet.
    while (c > upper) {
        c -= 26;
    }
    
    return c;
}

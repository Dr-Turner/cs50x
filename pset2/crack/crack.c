#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*  
    a program that cracks passwords. Takes hash via command line argument;
*/

// Global variables 
char word[5] = {0, 0, 0, 0};
char hash[20];
char salt[2];

void match (void);

void shift(int n, int i);

int main (int args, char *argv[]) {
    // reject if incorrect number of arguments. 
    if (args != 2) {
        printf("Usage: ./crack hash\n");
        return 1;
    }
    
    // set up hash and salt
    strcpy(hash, argv[1]);
    strncpy(salt, hash, 2);
    

    // iterate (through shift()) for each of the four letters
    int a, b, c, d;
    for (a = 0; a < 52; a++){
        for (b = 0; b < 52; b++){
            for (c = 0; c < 52; c++){
                for (d = 0; d < 52; d++){
                    shift(0, d);
                }
                shift(1, c);
            }
            shift(2, b);
        }
        shift(3, a);
    }
    return 0;
}


void shift(int n, int i){
    /*
     shift n-th letter of word to value i
     and check match();
    */
    
    if (i <= 25){
        i += 65;
    } else {
        i += 71;
    }
    word[n] = i;
    
    match();
}


void match (void) {
    /*
    check if hash matches the result of crack(salt, word)
    exit and print word if so.
    */
    
    if (strcmp(crypt(word, salt), hash) == 0){
        // Found word
        printf("%s\n", word);                
        exit(0);
    }
}

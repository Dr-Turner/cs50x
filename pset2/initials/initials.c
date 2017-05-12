#include <stdio.h>
#include <cs50.h>
#include <ctype.h>
#include <string.h>

/*
    a program that, given a person’s name, prints the person’s initials
    using user input for persons name and deals with extra whitespace.
*/


int main (void) {
    string s = get_string();
    
    char c;
    // initialising last as whitespace will capture the first character of the string if it's a a letter
    char last = ' ';
    
    for (int i = 0; i < strlen(s);i++){
        c = s[i];
        // captures every character AFTER whitespace as an initial
        if (last == ' ' && isalpha(c)) {
            putchar(toupper(c));
        }
        last = c;
    }
printf("\n");    
}
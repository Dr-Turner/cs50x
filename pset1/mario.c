#include <cs50.h>
#include <stdio.h>
/*
    a program that prints out a double half-pyramid of a user-specified height.
*/
int main (void) {
    int height;
    
    /*Get input*/
    while (true) {
        printf("Height: ");
        height = get_int();
        
        // accept if within tolerable range
        if (height >= 0 && height <= 23){
            break;
        }
    }
    
    /*Print pyramid row by row*/
    for (int i = 1 ; i <= height ; i++) {
        
        // if in pyramid area print '#' else ' '
        for (int j = 0; j < (height + 2 + i); j++) {
            if ((j < height && j >= (height - i)) ||
                (j > (height + 1) && j < (height + 2 + i))) {
                printf("#");
            } else {
                printf(" ");
            } 
        }
        printf("\n");
    }

}
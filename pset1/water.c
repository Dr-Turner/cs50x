#include <cs50.h>
#include <stdio.h>
/*
    program that converts (n) minutes of water use to (n x 12) bottles. Takes input via the command line.
*/

int main (void) {
    printf("Minutes: ");
    int minutes = get_int();
    printf("Bottles: %d\n", minutes * 12);
}
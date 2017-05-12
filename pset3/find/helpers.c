/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */
 
#include <cs50.h>
#include "helpers.h"


/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    // false if n is non-positive
    if (n < 0) {
        return false;
    }
    
    // initialise start & end values
    int start = 0; 
    int end = n-1;
    
    
    while(1) {
        
        // make sure the space we search is in the middleof the current start & end indeces
        int middle = start + ((end-start) / 2);
        
        
        // FOUND it
        if (values[middle] == value){
            return true;
            
        // realign end to discard second half of current selection
        } else if (values[middle] > value) {
            
            // this checks if we're left with a subarray of 1 item. 
            //    If we are at this point, then it doesn't contain the value.
            if (end == middle){
                return false;   
            }
            
            end = middle;
            
            
        // realign end to discard first half of current selection
        } else {
            start = middle + 1;
 
            // if the start is at a higher index than the end, there is not value in values.
            if (start > end) {
                return false;
            }
        }
    }    
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    // counting sort.
    int array[65537];
    int max = 0;
    
    // step one. go through values[] and mark down the number of occurences of each value in it's index in array[] 
    for (int i = 0 ; i < n; i++){
        int x = values[i];
        values[i] = 0;
        array[x]++;
        if (x > max){
            max = x;
        }
    }
    
    // step 2. for quantity in array[], place value of index back in values[], starting with the smallest and stopping when you
    // reach the highest value
    for (int l = 0, j = 0; l <= max; l++){
        while(array[l] > 0){
            values[j] = l;
            array[l]--;
            j++;
        }
    }
    return;
}
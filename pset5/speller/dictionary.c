/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "dictionary.h"

#define CHARS 27



unsigned int total;

/*
  Node structure
*/
typedef struct Node {
    bool is_word;
    struct Node* children[CHARS];
} Node;

// root node. this is where all inserts and checks must begin.
Node* root;

/**
 *returns int value for char argument (0 = ', 1 = 'a'... 26 = 'z')
 */
int get_char_int(int c){
    // for ' characters
    if(c == 39) {
        return 0;
    } 
    else if (c >= 97) {
        return (c - 'a') + 1;
    } 
    else {
        return (c - 'A') + 1;
    }
}

/** 
 * calls itself recursively and unloads each node that has no children.
 */

void unloadNode(Node *head){
    for (int i = 0; i < CHARS; i++){
        if(head->children[i] != NULL){
            unloadNode(head->children[i]);
        }
    }
    free(head);
}



/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char *word)
{
    int i = 0, x = 0;
    char c;
    Node* head = root;


    while (head != NULL){
        // get current character from word and (if its's not an end of word character) convert to int.
        // if it is the end of a word, see if that word is a word (rather than just a part of a bigger word)
        c = word[i];
        if(c == 0){
            return head->is_word;        
        }
        x = get_char_int(c);
        
        // reposistion for next character.
        head=head->children[x];
        i++;
    }
    // it will only reach this code if we've not hit a NULL
    return false;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary){
    // give root node memory allocation here. It took me hours to get around segmentation faults and this is the way.
    root = calloc(sizeof(Node), 1); 
    Node* head = root;

    int c = 0;

    // open dictionary
    FILE *dictptr = fopen(dictionary, "r");
    if (dictptr == NULL) {
        fclose(dictptr);
        return false;
    }

    
    // loop through each character of dictionary
    while(c != EOF){
        c = fgetc(dictptr);
        
        // if end of word: declare is_word, and start again at root.
        if (c == '\n' || c == EOF){
            head->is_word = true;
            head = root;
            // only increment total on new lines.
            if (c == '\n'){
                total++;    
            }
        // else it will be a character. if there is no child node for that character make one and make it head node. 
        // Otherwise just make that child node head.
        } else {
            
            c = get_char_int(c);
            if (head->children[c] == NULL){
                Node *temp = (Node*) malloc(sizeof(Node));
                head->children[c] = temp;
                head = temp;
            } else {
                head = head->children[c];
            }
        }
    }

    fclose(dictptr);
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return total;
}


/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    unloadNode(root);  // recursive function that'll do all the work for us. 
    return true;
}

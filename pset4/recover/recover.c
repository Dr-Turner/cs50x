#include <stdio.h>
#include <stdbool.h>


int main(int argc, char *argv[]){
    
    // check input
    if (argc != 2){
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }
    
    // open memcard file    
    char *rawName = argv[1];
    FILE *rawptr = fopen(rawName, "r");
    
    
    if (rawptr == NULL){
        fprintf(stderr, "Could not open %s.\n", rawName);
        return 2;
    }
    
    // set card counter int
    int i = 0;
    FILE *photoptr;
    char filename[10];
    unsigned char buffer[512];
    bool writeMode = false;
    
    // while there is sytes to be read:
    while(fread(&buffer, 512, 1, rawptr)){

        // if first bytes mean start of jpeg, close previous file (in writemode) and open new file (incrementing i)
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0) {
            
            if (writeMode){
                fclose(photoptr);
            }
            
            sprintf(filename, "%03i.jpg", i);
            photoptr = fopen(filename, "w");
            i++;
            writeMode = true;
        }
        // else write to file (if in writemode)
        if (writeMode){
            fwrite(&buffer, 512, 1, photoptr);
        }
        
    }
    // wrap it up
    fclose(rawptr);
    fclose(photoptr);
    return 0;
    
}
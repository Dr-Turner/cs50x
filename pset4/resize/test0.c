/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./test0 scale infile\n");
        return 1;
    }



    // remember filenames
    float scale;
    sscanf(argv[1], "%f", &scale);
    char *infile = argv[2];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }
    
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    int biHeight = abs(bi.biHeight);

    RGBTRIPLE array[biHeight][bi.biWidth];

    // iterate over infile's scanlines
    for (int i = 0; i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            array[i][j] = triple;

        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    //display contents of array
    int oldSize = biHeight;
    int newSize = round(oldSize * scale);
    
    for (int i = 0; i < newSize; i++){
        for (int j = 0; j < newSize; j++){
            int x = floor(i / scale);
            int y = floor(j / scale);
            
            printf("%02x%02x%02x ", array[x][y].rgbtBlue,
                                    array[y][y].rgbtGreen,
                                    array[x][y].rgbtRed);       
        }
        printf("\n");
    }
    

    // success
    return 0;
}

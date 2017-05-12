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
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./test1 scale infile outfile\n");
        return 1;
    }


    // remember filenames
    float scale;
    sscanf(argv[1], "%f", &scale);
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }
    
    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
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

    // determine padding for input scanlines
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

    // change header properties for the outfile.
    bi.biWidth = bi.biWidth *= scale;
    bi.biHeight = bi.biHeight *= scale;
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding) * biHeight;
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write outfile
    int oldSize = biHeight;
    int newSize = round(oldSize * scale);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for output scanlines
    padding = (4 - (newSize * sizeof(RGBTRIPLE)) % 4) % 4;

    
    for (int i = 0; i < newSize; i++){
        for (int j = 0; j < newSize; j++){
            int x = floor(i / scale);
            int y = floor(j / scale);
            
            RGBTRIPLE triple;
            
            triple = array[x][y];
            
            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }
        
        // padding :)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
        
    }
    
    
    fclose(outptr);
    
    // success
    return 0;
}

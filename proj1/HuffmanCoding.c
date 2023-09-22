#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    FILE* file;
    char* inFile, *outFile;
    
    // defaults
    inFile = "defaultIn.txt";
    outFile = "DefaultOut.txt";
    
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-i") == 0)
        {
          inFile = argv[++i];
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
          outFile = argv[++i];
        }
    }
    //printf("inFile: %s\noutFile: %s\n", inFile, outFile);
    
    // try to open an existing file to read in
    file = fopen(inFile, "r");
    if (file == NULL){
        printf("%s does not exist.", inFile);
        return -1;
    }
    
    // create an int array of a bytesize (256) and initialize it with 0's
    const int BYTESIZE = 256;
    int freq[BYTESIZE];
    for (int i = 0; i < BYTESIZE; i++) freq[i] = 0;
    
    // loop through the file and increase the array at the same index of the chars ascii value
    int index;
    do{
        index = fgetc(file);
        freq[index]++;
    } while(index != EOF);
    
    fclose(file);
    
    for (int i = 0; i < BYTESIZE; i++) if (freq[i] > 0) printf("%c = %d\n", i, freq[i]);
    return 0;
}

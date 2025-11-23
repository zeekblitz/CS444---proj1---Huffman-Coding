#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTESIZE 256

typedef struct byteBuffer{
    unsigned char buffer;
    int count;
} ByteBuffer;

// finish the file
void flushBuffer(ByteBuffer* bb){
    if (bb->count > 0){
        while(bb->count != 8){
            bb->buffer = (bb->buffer << 1) | 0;
            bb->count++;
        }
    }
}

// Node
typedef struct node{
    int data;
    int priority; // Lower values indicate higher priority
    struct node *next;
    struct node *left;
    struct node *right;
} Node;

// Function to Create A New Node
Node* newNode(int d, int p){
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;
    temp->left = NULL;
    temp->right = NULL;

    return temp;
}

// Function to push according to priority
void push(Node **head, int d, int p){
    // Create a new Node
    Node *temp = newNode(d, p);

    // add the first node in the list
    if ((*head) == NULL){
        //printf("adding first node...\n");
        (*head) = temp;
        return;
    }

    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    Node *start = (*head);
    if ((*head)->priority > p){

        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else{
        // Traverse the list and find a position to insert new node
        while (start->next && start->next->priority < p){
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

// function to convert the list to a Huffman tree
Node* listToTree(Node **head){
    Node* start = (*head);
    Node* second = start->next;
    // create a new empty node that will hold the two nodes
    // and their combined priority value
    int d = 0;
    int p = start->priority + second->priority;
    Node* parent = newNode(d, p);
    
    // assign the start node to the left of the new parent node
    parent->left = start;
    // assign the second node to the right of parent
    if (second) parent->right = second;
    
    // point the parent to the next (third) node of the list
    if (second->next) parent->next = second->next;
    // delete the link from the first node to the second node
    start->next = NULL;
    // delete the second link
    second->next = NULL;
    
    // sort the parent node into the pq list
    Node* first;
    if (parent->next && parent->next->priority < p){
        // the parent node is currently the first node in the list
        start = parent;
        // the next node will be the new head of the list
        first = start->next;
        while (start->next && start->next->priority < p){
                start = start->next;
        }
        // Either at the ends of the list
        // or at required position
        parent->next = start->next;
        start->next = parent;
    }
    else first = parent;

    // if the next node in the list is not null,
    // call the function again with the new starting node
    if (first->next) return listToTree(&first);
    
    // else the tree is complete,
    // so return the parent node as the first node of the tree
    return first;
}

void printTree(Node* root, int row[], int col[], int top) {
    if (root->left) {
        col[top] = 1;
        printTree(root->left, row, col, top + 1);
    }
    if (root->right) {
        col[top] = 0;
        printTree(root->right, row, col, top + 1);
    }
    if (!root->left && !root->right) {
        printf("%3d = %8d: ", root->data, root->priority);
        
        for (int i = 0; i < top; ++i) {
            printf("%d", col[i]);
        }
        printf("\n");
    }
}

void buildLookupTable(Node* root, int** table, int index, int row[], int top){
    if (root->left) {
        row[top] = 1;
        buildLookupTable(root->left, table, index, row, top + 1);
    }

    if (root->right) {
        row[top] = 0;
        buildLookupTable(root->right, table, index, row, top + 1);
    }

    if ((!root->left && !root->right) && root->data == index) {
        *table = (int*)malloc(sizeof(int) * (top + 1));
        
        // store the length of the array in the first element
        (*table)[0] = top + 1;

        for (int i = 1; i < top; i++){
            (*table)[i] = row[i];
        }

        return;
    }
}

void writePQToFile(Node* head, char* ofile, int length){
    FILE* outfile = fopen(ofile, "wb");

    // store the length of the list to the first bytes of the file
    fwrite(&length, sizeof(int), 1, outfile);
    //printf("length: %d\n", length);

    Node *temp = head;
    while (temp){
        //printf("%c = %d\n", temp->data, temp->priority);
        fwrite(&(temp->data), sizeof(int), 1, outfile);
        fwrite(&(temp->priority), sizeof(int), 1, outfile);
        temp = temp->next;
    }
    //temp = NULL;
    fclose(outfile);
}

Node* readPQFromFile(char* ifile){
    FILE* inFile = fopen(ifile, "rb");
    // get the length of the list from the first bytes of the file
    int length;
    fread(&length, sizeof(int), 1, inFile);
    printf("length: %d\n", length);

    // continue reading in the index and frequency pairs
    Node *new = NULL;
    int index, frequency;
    for (int i = 1; i < length; i++){
        fread(&index, sizeof(int), 1, inFile);
        fread(&frequency, sizeof(int), 1, inFile);
        printf("%c = %d\n", index, frequency);
        push(&new, index, frequency);
    }
    fclose(inFile);
    return new;
}

int main(int argc, char *argv[]){
    FILE *file, *ofile;
    char inFile[128], outFile[128];

    // defaults
    //inFile = "completeShakespeare.txt";
    //outFile = "huffman.out";

    // test for file
    if (argc < 2){
        printf("ERROR: file required\n");
        return 0;
    }

    // determine file type and encode/decode accordingly
    char *sub = strrchr(argv[1], '.');
    if (sub == NULL){
        printf("ERROR: missing extension\n");
        return 0;
    }

    char extension[5];
    strcpy(extension, sub);
    if (strcmp(extension, ".txt") == 0){
        // encode the text file
        strcpy(inFile, argv[1]);
        // copy the input file name to the output file name and replace the extension
        int length = sub - argv[1];
        if (length > 128){
            printf("ERROR: filename too long\n");
            return 0;
        }
        strncpy(outFile, argv[1], length);
        outFile[length] = '\0';
        strcat(outFile, ".out");
        printf("file = %s\n", outFile);
    }
    else if (strcmp(extension, ".out") == 0){
        // decode the binary file
    }
    else{
        printf("ERROR: invalid file type\n");
        return 0;
    }

    // try to open an existing file to read in
    file = fopen(inFile, "r");
    if (!file){
        printf("%s does not exist.\n", inFile);
        return 0;
    }

    // create an int array of a bytesize (256) and initialize it with 0's
    int freq[BYTESIZE];
    for (int i = 0; i < BYTESIZE; i++) freq[i] = 0;

    // loop through the file and increase the array at the same index of the chars ascii value
    int index = fgetc(file);
    while (index != EOF) {
        freq[index]++;
        index = fgetc(file);
    }

    fclose(file);
    
    //for (int i = 0; i <= BYTESIZE; i++) if (freq[i] > 0) printf("%d = %d\n", i, freq[i]);

    // make a priority queue to hold and sort the chars
    Node *pq = NULL;
    int pqLength = 0;
    for (int i = 0; i < BYTESIZE; i++){
        if (freq[i] > 0){
            push(&pq, i, freq[i]);
            pqLength++;
        }
    }

    // write the pq to a file
    //writePQToFile(pq, outFile, pqLength);

    //Node* test = readPQFromFile(outFile);

    // convert the priority queue into a binary tree
    pq = listToTree(&pq);

    // print the tree with the codes
    //int row[BYTESIZE], col[128];
    //printTree(pq, row, col, 0);

    // build the lookup table to store the codes for each ascii char
    int** table = (int**)malloc(sizeof(int) * BYTESIZE);
    static int row[100];
    for (int i = 0; i < BYTESIZE; i++){
        buildLookupTable(pq, &table[i], i, row, 0);
    }
    
    // open the text file again
    file = fopen(inFile, "r");
    
    // open a new file to write to in binary
    ofile = fopen(outFile, "wb");
    fseek(ofile, 0, SEEK_END);

    static int bin[100];
    ByteBuffer bb;

    // read in the first char
    int ch;
    ch = fgetc(file);
    do{
        for (int i = 1; i < table[ch][0]; i++){
            // write those bits to the buffer
            bb.buffer = (bb.buffer << 1) | table[ch][i];
            bb.count++;
            // if the buffer has 8 bits, write the byte to the file
            if (bb.count == 8){
                fwrite(&(bb.buffer), 1, 1, ofile);
                // reset the buffer
                bb.buffer = 0;
                bb.count = 0;
            }
        }
        // read in the next char
        ch = fgetc(file);
    } while(ch != EOF);
    
    // check for any remaining bits and add 0's to the end to make a full byte
    flushBuffer(&bb); 
    //write the last byte to the file
    fwrite(&(bb.buffer), 1, 1, ofile);
    //close both files
    fclose(file);
    fclose(ofile);

    // free the allocated memory
    free(table);
    
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define BYTESIZE 256

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
    Node *start = (*head);

    // Create new Node
    Node *temp = newNode(d, p);

    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
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
    // create a new empty node the will hold the two nodes
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

void findChar(Node* root, char find, int arr[], int top, FILE* ofile, ByteBuffer *bb) {
    if (root->left) {
        arr[top] = 1;
        findChar(root->left, find, arr, top + 1, ofile, bb);
    }

    if (root->right) {
        arr[top] = 0;
        findChar(root->right, find, arr, top + 1, ofile, bb);
    }

    if ((!root->left && !root->right) && root->data == find) {
        for (int i = 0; i < top; i++){
            // write those bits to the buffer
            bb->buffer = (bb->buffer << 1) | arr[i];
            bb->count++;
            // if the buffer has 8 bits, write the byte to the file
            if (bb->count == 8){
                fwrite(&(bb->buffer), 1, 1, ofile);
                // reset the buffer
                bb->buffer = 0;
                bb->count = 0;
            }
        }
        return;
    }
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

int main(int argc, char *argv[]){
    FILE *file, *ofile;
    char *inFile, *outFile;
    const int BYTESIZE = 256;

    // defaults
    inFile = "completeShakespeare.txt";
    outFile = "huffman.out";

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-i") == 0) inFile = argv[++i];
        else if (strcmp(argv[i], "-o") == 0) outFile = argv[++i];
    }

    // try to open an existing file to read in
    file = fopen(inFile, "r");
    if (!file){
        printf("%s does not exist.\n", inFile);
        return -1;
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
    Node *pq;

    // find the first element that has a value
    // and assign it to the first node in the pq
    index = 0;
    do{
        if (freq[index] > 0){
            pq = newNode(index, freq[index]);
            break;
        }
        index++;
    }while (index < BYTESIZE);

    // assign the rest of the chars to the pq
    while (index < BYTESIZE){
        index++;
        if (freq[index] > 0) push(&pq, index, freq[index]);
    }
    
    // convert the priority queue into a binary tree
    pq = listToTree(&pq);

    // print the tree with the codes
    
    int row[BYTESIZE], col[128];
    printTree(pq, row, col, 0);
    
    // open the file again
    file = fopen(inFile, "r");
    
    // open a new file to write to in binary
    ofile = fopen(outFile, "wb");
    
    static int bin[100];
    char ch;
    ByteBuffer bb;
    do{
        // read in each char
        ch = fgetc(file);
        
        // search for it along the tree
        findChar(pq, ch, bin, 0, ofile, &bb);
    } while(ch != EOF);
    
    // check for any remaining bits and add 0's to the end to make a full byte
    flushBuffer(&bb); 
    //write the last byte to the file
    fwrite(&(bb.buffer), 1, 1, ofile);
    //close both files
    fclose(file);
    fclose(ofile);
    
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Return the value at head
int peek(Node **head){
    return (*head)->data;
}

// Removes the element with the
// highest priority from the list
void pop(Node **head){
    Node *temp = *head;
    (*head) = (*head)->next;
    free(temp);
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
        while (start->next != NULL && start->next->priority < p){
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

// Function to check if list is empty
int isEmpty(Node **head){
    return (*head) == NULL;
}

void printListSize(Node* root){
    if (root == NULL) return;
    int size = 0;
    while (root != NULL){
        size++;
        root = root->next;
    }
    printf("size = %d\n", size);
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
    if (second != NULL) parent->right = second;
    
    // point the parent to the next (third) node of the list
    if (second->next != NULL) parent->next = second->next;
    // delete the link from the first node to the second node
    start->next = NULL;
    // delete the second link
    second->next = NULL;
    
    // sort the parent node into the pq list
    Node* first;
    if (parent->next != NULL && parent->next->priority < p){
        // the parent node is currently the first node in the list
        start = parent;
        // the next node will be the new head of the list
        first = start->next;
        while (start->next != NULL && start->next->priority < p){
                start = start->next;
        }
        // Either at the ends of the list
        // or at required position
        parent->next = start->next;
        start->next = parent;
    }
    else first = parent;
    
    // print the length of the list
    //printListSize(first);

    // if the next node in the list is not null,
    // call the function again with the new starting node
    if (first->next != NULL) return listToTree(&first);
    
    // else the tree is complete,
    // so return the parent node as the first node of the tree
    return first;
}

void printTree(Node* root) {
    if (root==NULL) return;
    if (root->data != 0) printf("%d = %d\n", root->data, root->priority);
    printTree(root->left);
    printTree(root->right);
}

int main(int argc, char *argv[]){
    FILE *file;
    char *inFile, *outFile;
    const int BYTESIZE = 256;

    // defaults
    inFile = "defaultIn.txt";
    outFile = "DefaultOut.txt";

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-i") == 0) inFile = argv[++i];
        else if (strcmp(argv[i], "-o") == 0) outFile = argv[++i];
    }
    // printf("inFile: %s\noutFile: %s\n", inFile, outFile);

    // try to open an existing file to read in
    file = fopen(inFile, "r");
    if (file == NULL){
        printf("%s does not exist.", inFile);
        return -1;
    }

    // create an int array of a bytesize (256) and initialize it with 0's
    int freq[BYTESIZE];
    for (int i = 0; i < BYTESIZE; i++) freq[i] = 0;

    // loop through the file and increase the array at the same index of the chars ascii value
    int index;
    do{
        index = fgetc(file);
        freq[index]++;
    }while (index != EOF);

    fclose(file);
    //for (int i = 0; i < BYTESIZE; i++) if (freq[i] > 0) printf("%c = %d\n", i, freq[i]);

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
    
    /*
    while (!isEmpty(&pq)){
        printf("%c ", peek(&pq));
        pop(&pq);
    }
    */
    
    // convert the priority queue into a binary tree
    pq = listToTree(&pq);
    
    printTree(pq);
    
    return 0;
}

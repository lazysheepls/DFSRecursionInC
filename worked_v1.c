#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Graph.h"
#include "Quack.h"

///////// worked

#define STD_BUF_SIZE 10

// Function declarations
bool isSameStr(char *, char *);
bool differByOne(char *, char *);
char* readFromStdin(char *);
int splitToWords(char *,char **);
void dfs(Graph, int*,int, char **);
void dfsR(Graph, Vertex, int, int, int*, int *, Quack *, Quack *, char **);


// Global variables
//char* words[20];
//int numberOfWords=0;

int main(void)
{
    // get words from stdin
    char **words;
    words = (char**)malloc(sizeof(char*)*20*1000);
    
    //todo
    int numberOfWords=0;
    char* strBufferPtr = (char*)malloc(STD_BUF_SIZE);
    char* inputStr = readFromStdin(strBufferPtr);
    numberOfWords = splitToWords(inputStr,words);

    // print dictionary
    printf("Dictionary\n");
    for(int i =0;i<numberOfWords;i++)
        printf("%d: %s\n",i,words[i]);

    // build new graph
    Graph graph = newGraph(numberOfWords);
    for(int i=0;i<numberOfWords;i++)
    {
        for(int j=i+1;j<numberOfWords;j++)
        {
            // printf("word %d: %s | word %d: %s diff by one?%d\n",i,words[i],j,words[j],differByOne(words[i],words[j]));
            if(differByOne(words[i],words[j]))
            {
                Edge edge = newEdge(i,j);
                insertEdge(edge,graph);
            }
        }
    }
    printf("Ordered Word Ladder Graph\n");
    showGraph(graph);

    // dfs
    int maxDepth = 0;
    dfs(graph,&maxDepth,numberOfWords,words);
    printf("Final max depth:%d\n",maxDepth);

    // free memories
    free(strBufferPtr);
    strBufferPtr = NULL;
    free(words);
    graph = freeGraph(graph);
}

#pragma region dfs recursion
void showLadder(Quack result, int maxDepth, char** words)
{
    // store stack into queue
    Quack queue = createQuack();
    while(!isEmptyQuack(result))
    {
        push(pop(result),queue);
    }

    int lineCnt = 0;
    printf("Maximal ladders:\n");
    while(!isEmptyQuack(queue))
    {      
        printf("%d: ",++lineCnt);

        for(int i=0;i<maxDepth;i++)
        {
            printf("%s",words[pop(queue)]);
            if (i != maxDepth-1)
                printf(" -> ");
        }
        printf("\n");
    }

    free(queue);
}

void dfs(Graph graph, int *maxDepth, int numberOfVertex, char** words)
{
    int* visited = (int*)malloc(numberOfVertex*sizeof(int));
    if(visited == NULL){
        fprintf (stderr, "create visited int array: no memory, aborting\n");
        exit(1);
    }
    for(int i=0;i<numberOfVertex;i++){
        *(visited+i) = -1;
    }

    int order = 0;
    Quack quack = createQuack();
    Quack quackTest = createQuack();

    for(int startVertex = 0; startVertex<numberOfVertex;startVertex++){
        dfsR(graph,startVertex,numberOfVertex,order,maxDepth,visited,&quack,&quackTest,words);
    }

    printf("Show result quack:\n");
    showQuack(quackTest);
    showLadder(quackTest,*maxDepth,words);

    free(visited);
    visited = NULL;

    makeEmptyQuack(quack);
    makeEmptyQuack(quackTest);
    quack = destroyQuack(quack);
    quackTest = destroyQuack(quackTest);
}

void dfsR(Graph graph, Vertex vertex, int numberOfVertex, int order, int *maxDepth, int *visited, Quack *quack, Quack *result, char ** words)
{
    order++;
    visited[vertex]=order;
    push(vertex,*quack);

    for(int lookupVertex=vertex;lookupVertex<numberOfVertex;lookupVertex++)
    {
        Edge edge = newEdge(vertex,lookupVertex);
        if(isEdge(edge,graph) && visited[lookupVertex] == -1){
            dfsR(graph,lookupVertex,numberOfVertex,order,maxDepth,visited,quack,result,words);
        }
    }

    if(order == *maxDepth)
    {
        // Add to result queue
        printf("{");
        for(int i=0;i<numberOfVertex;i++){
            printf("%d, ",visited[i]);
        }
        printf("}\n");

        //Append to the current result
        for(int i=0;i<numberOfVertex;i++){
            printf("Index%d: ",i);
            if(visited[i] != -1){
                push(i,*result);
                printf("Pushed %s into result.\n",words[i]);
            }   
        }
    }
    else if (order > *maxDepth)
    {
        printf("New max order is:%d\n",order);
        printf("New max depth is:%d\n {",*maxDepth);
        for(int i=0;i<numberOfVertex;i++){
            printf("%d, ",visited[i]);
        }
        printf("}\n");

        *maxDepth = order;
        // Create new quack to store
        makeEmptyQuack(*result);
        for(int i=0;i<numberOfVertex;i++){
            printf("Index%d: ",i);
            if(visited[i] != -1){
                push(i,*result);
                printf("Pushed %s into result.\n",words[i]);
            }   
        }
    }

    visited[vertex] = -1;
    pop(*quack);
}
#pragma endregion

#pragma region Handle Stdin
char* readFromStdin(char* strBufferPtr)
{
    char thisChar;
    // char* strBufferPtr = (char*)malloc(STD_BUF_SIZE);
    if (strBufferPtr == NULL) {
      fprintf (stderr, "create string buffer: no memory, aborting\n");
      exit(1);
   }
    int actualBufferSize = STD_BUF_SIZE;
    int bufferCnt = 0; // buffer counter
    char* resultStr = "";

    thisChar = getchar();
    while(thisChar!=EOF)
    {
        if(bufferCnt >= actualBufferSize-2)
        {
            actualBufferSize += STD_BUF_SIZE;
            strBufferPtr = realloc(strBufferPtr,actualBufferSize);
            if(strBufferPtr==NULL)
            {
                fprintf(stderr, "Memory Error.\n");
                exit(1);
            }
        }

        *(strBufferPtr+bufferCnt) = thisChar;
        bufferCnt++;
        thisChar = getchar();   
    }
    *(strBufferPtr+bufferCnt) = '\0';
    resultStr = strBufferPtr;
    printf("read in str is:%s\n",strBufferPtr);
    // free(strBufferPtr);
    // strBufferPtr = NULL;

    return resultStr;
}

//todo: handle duplicates
int splitToWords(char* inputStr, char ** words)
{
    const char* delim = " \t\n\r";
    char* thisWord;
    int index=0;

    thisWord = strtok(inputStr,delim);
    while(thisWord != NULL)
    {
        // check duplication
        bool sameStrFound = false;
        for(int i=0;i<index;i++)
        {
            sameStrFound = isSameStr(thisWord,words[i])?true:false;
            if(sameStrFound)
                break;
        }

        if(!sameStrFound)
            words[index++] = thisWord;
        
        thisWord = strtok(NULL,delim);
    }

    return index;
}
#pragma endregion

#pragma region String Compare
bool isSameStr(char * str1, char * str2)
{
    if(strlen(str1)!=strlen(str2))
        return false;
    
    for(int i=0;i<strlen(str1);i++)
    {
        if (*(str1+i) != *(str2+i))
            return false;
    }

    return true;
}

bool differByOne(char * str1, char * str2)
{
    int len1 = (int)strlen(str1);
    int len2 = (int)strlen(str2);
    int diffCnt;
    
    // Secnario 1: Length difference too big, no need to compare
    if (abs(len1 - len2)>1)
        return false;
    
    // Senario 2: Length is equal, compare character by character
    if (len1 == len2)
    {
        diffCnt = 0;
        for(int i=0;i<len1;i++)
        {
            if(*(str1+i) != *(str2+i))
                diffCnt++;
        }
        
        return diffCnt==1?true:false;
    }
    
    // Scenario 3: length difference is one, compare shorter str to longer str
    if (abs(len1-len2)==1)
    {
        char * shortStr,* longStr;
        int shortLen, longLen;
        
        shortStr = len1>len2?str2:str1;
        longStr = len1>len2?str1:str2;
        shortLen = len1>len2?len2:len1;
        longLen = len1>len2?len1:len2;
        
        diffCnt = 0;
        int shortIndex = 0;
        int longIndex = 0;
        while(shortIndex <= (shortLen-1) && longIndex <= (longLen-1))
        {
            if(*(shortStr+shortIndex)!=*(longStr+longIndex))
            {
                diffCnt++;
                longIndex++;
            }
            else
            {
                shortIndex++;
                longIndex++;
            }
        }
        
        return diffCnt>1?false:true;
    }
    
    return false; // catch all
}
#pragma endregion
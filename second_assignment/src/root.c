#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include "root.h"
#include "utils.h"
int main(int argc, char* argv[]) {
    if(argc != 13){
        fprintf(stderr, "Usage: ./lexan -i <TextFile> -l <numOfSplitter> -m <numOfBuilders> -t <TopPopular> -e <ExclusoionListFile> -o <OutputFile>\n" );
    }

    int option;
    char* inputFile = NULL;
    int numOfSplitter;
    int numOfBuilders;
    int topPopular;
    char* outputFile = NULL;
    List exclusionList = listCreate(free, NULL);
    char* exclusionFile = NULL;
    while((option = getopt(argc, argv, "i:l:m:t:e:o:")) != -1){

        if(option == 'i'){
            inputFile = optarg;
        }
        else if(option == 'l'){
            numOfSplitter = atoi(optarg);
        }
        else if(option == 'm'){
            numOfBuilders = atoi(optarg);
        }
        else if(option == 't'){
            topPopular = atoi(optarg);
        }
        else if(option == 'e'){
            exclusionFile = optarg;
        }
        else if(option == 'o'){
            outputFile = optarg;
        } 
        else{
            return 1;
        }
    }


    int fdInput = open(inputFile, O_RDONLY);
    if(fdInput == -1){
        fprintf(stderr, "Error opening file %s\n", inputFile);
        return 1;
    }

    int lines = 1;
    int bytesRead;
    char buffer[4096];
    // calculate the number of lines in the file
    while ((bytesRead = read(fdInput, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                lines++;
            }
        }
    }

    int* bytesPerLine = malloc((lines + 1) * sizeof(int));
    bytesPerLine[0] = 0;
    // lines are 1 based
    lines = 1;
    lseek(fdInput, 0, SEEK_SET);
    int bytesOfLine = 0;
    // count bytes per line to pass to each splitter 
    // so that it knows where to start reading from
    while( (bytesRead = read(fdInput, buffer, sizeof(buffer))) > 0){
        for (int i = 0; i < bytesRead; i++) {
            bytesOfLine += sizeof(buffer[i]); ;
            if (buffer[i] == '\n') {
                bytesPerLine[lines] = bytesOfLine + bytesPerLine[lines - 1];
                lines++;
                bytesOfLine = 0;
            }
        }
    }   

    // Close the file, each time the splitters open it so that 
    // we don't need to reposition the read pointer
    close(fdInput);
    
    // Set the read pointer to the beginning of the file
    memset(buffer, 0, sizeof(buffer));
    lseek(fdInput, 0, SEEK_SET);


//---------------------------------------------------------------------builders---------------------------------------------------------------------
    //creating pipes for communication from splitters to builders(one for every builder)
    int pipesSplitterToBuilder[numOfBuilders][2];
    for (int b = 0; b < numOfBuilders; b++) {
        if(pipe(pipesSplitterToBuilder[b]) == -1){
            perror("Error creating pipe");
            exit(1);
        }
    }


    pid_t builderPids[numOfBuilders];
    for (int b = 0; b < numOfBuilders; b++) {
        pid_t pid = fork();
        
        if (pid == -1) {
            perror("Error forking builder process");
            exit(1);
        }

        else if (pid == 0) {
            // Close the read end of the pipe in the builder exept for the pipe of this builder
            for(int i = 0; i < numOfBuilders; i++){
                if(i != b){
                    close(pipesSplitterToBuilder[i][0]);
                }
                // Close the write end of the pipe in the builder
                close(pipesSplitterToBuilder[i][1]);
            }
            int fdForBuilder = pipesSplitterToBuilder[b][0];
            char fdForBuilderStr[16];
            sprintf(fdForBuilderStr, "%d", fdForBuilder);
            execlp("./builder", "./builder", fdForBuilderStr, NULL);
            exit(EXIT_SUCCESS);
        }
        else {
            //for parent process close both write and read end
            builderPids[b] = pid;
        }
    }
//---------------------------------------------------------------------splitters---------------------------------------------------------------------


    // Create l splitters with lines/l lines each
    // except for the last one which might take a few more lines due to imperfect division
    int linesForSplitter = lines / numOfSplitter;
    pid_t splitterPids[numOfSplitter];

    int currentLine = 1;

    int bytesRemainingInBuffer = 0;
    int nextCharToRead = 0;

    char numberOfBuilders[16];
    sprintf(numberOfBuilders, "%d", numOfBuilders);

    //creating a string of write end file descriptors of pipes so as to pass it to every splitter
    //also doing it one time before fork, so that we don't need to do it for every splitter
    char* pipeWriteEnds = printingFdsToString(numOfBuilders, pipesSplitterToBuilder);

    for (int i = 0; i < numOfSplitter; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Error forking splitter %d");
            exit(EXIT_FAILURE);
        }

        // splitter
        else if (pid == 0){
            for(int b = 0; b < numOfBuilders; b++){
                // Close the read end of the pipe in the splitter
                close(pipesSplitterToBuilder[b][0]);
            }

            int startLine = ( (i == 0) ? 1 : (i * linesForSplitter) + 1);

            // If it is the last line, the last splitter will take the remaining lines
            int endLine = (i == numOfSplitter - 1) ? lines : (i + 1) * linesForSplitter;

            char start[16];
            char end[16];
            sprintf(start, "%d", startLine);
            sprintf(end, "%d", endLine);
            

            // after how many bytes it will start reading
            int position = i * linesForSplitter;
            char firstByteForSplitter[32];
            sprintf(firstByteForSplitter, "%d", bytesPerLine[position]);

           
            execlp("./splitter", "./splitter", inputFile, start, end, firstByteForSplitter, numberOfBuilders, pipeWriteEnds, NULL);

            perror("Error executing splitter");
            exit(EXIT_FAILURE);
        }
         
        else {
            // In the parent, we store the PID of the created splitter
            splitterPids[i] = pid;
        }
        
    }
    for(int i = 0; i < numOfBuilders; i++){
        close(pipesSplitterToBuilder[i][0]);
        close(pipesSplitterToBuilder[i][1]);
    }


    // Waiting every splitter to finish
    for (int i = 0; i < numOfSplitter; i++) {
        waitpid(splitterPids[i], NULL, 0);
    }
    for (int i = 0; i < numOfBuilders; i++) {
        waitpid(builderPids[i], NULL, 0);
    }

    
    free(bytesPerLine);
    free(pipeWriteEnds);
    // When freeing the list, free will be called for each node
    // so the space allocated for the string in the main will also be freed
    listDestroy(exclusionList);

}

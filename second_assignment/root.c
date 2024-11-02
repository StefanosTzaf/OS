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

int main(int argc, char* argv[]) {
    if(argc < 13){
        fprintf(stderr, "Usage: ./lexan -i <TextFile> -l <numOfSplitter> -m <numOfBuilders> -t <TopPopular> -e ExclusoionList -o <OutputFile>\n" );
    }

    int option;
    char* inputFile = NULL;
    int numOfSplitter;
    int numOfBuilders;
    int topPopular;
    char* outputFile = NULL;
    List exclusionList = listCreate(free, NULL);
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
            //optind είναι το επόμενο όρισμα στο argv, μέχρι να βρούμε -
            //δηλαδή να τελειώσουν τα ορίσματα της -e παραμέτρου
            while (optind < argc && argv[optind][0] != '-') {
                //δεσμευω εδω χόρω για το string που θα μπει στην λίστα
                //η λίστα θα αποθηκεύση δυναμικά μόνο τον pointer
                char* nodeToAdd = malloc(strlen(argv[optind]) + 1);
                strcpy(nodeToAdd, argv[optind]);
                listInsert(exclusionList, nodeToAdd);
                optind++;
            }
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
    //μετρημα γραμμών
    while ((bytesRead = read(fdInput, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                lines++;
            }
        }
    }
    
    //δείκτης ανάγνωσης στην αρχή του αρχείου
    memset(buffer, 0, sizeof(buffer));
    lseek(fdInput, 0, SEEK_SET);


//---------------------------------------------------------------------builders---------------------------------------------------------------------
    //δημιουργία n*l named pipes για την επικοινωνία των splitter με τους builders
    char fifoName[1024];
    for (int s = 0; s < numOfSplitter; s++) {
        for (int b = 0; b < numOfBuilders; b++) {
            //print σαν συμβολοσειρά τον αριθμό του named pipe
            //που καθορίζεται μονοσύμαντα από τον αριθμό του splitter και του builder
            snprintf(fifoName, 1024, "fifo_splitter%d_builder%d", s, b);
            if (mkfifo(fifoName, 0666) == -1) {
                perror("Error creating FIFO");
                exit(1);
            }
        }
    }

    for (int b = 0; b < numOfBuilders; b++) {
        pid_t pid = fork();
        
        if (pid == -1) {
            perror("Error forking builder process");
            exit(1);
        }
    }
//---------------------------------------------------------------------splitters---------------------------------------------------------------------
    //φτίαχνουμε pipes οσα και οι splitters
    int pipes[numOfSplitter][2];

    for (int i = 0; i < numOfSplitter; i++) {
        if (pipe(pipes[i]) == -1) {
            fprintf(stderr,"Error creating pipe %d\n", i);
            exit(1);
        }
    }

    //δημιουργούμε l splitters με lines/l γραμμές το καθένα
    int linesForSplitter = lines / numOfSplitter;
    pid_t splitterPids[numOfSplitter];

    int currentLine = 1;

    int bytesRemainingInBuffer = 0;
    int nextCharToRead = 0;
    for (int i = 0; i < numOfSplitter; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            fprintf(stderr, "Error forking splitter %d\n", i);
            exit(EXIT_FAILURE);
        }
        //splitter
        else if (pid == 0){
            //κλείνουμε το γράψιμο στουςς splitter
            close(pipes[i][1]);
            dup2(pipes[i][0], STDIN_FILENO);
            close(pipes[i][0]);

            execlp("./splitter", "./splitter", NULL);
            fprintf(stderr, "Error executing splitter %d\n", i);
            exit(EXIT_SUCCESS); // Τερματισμός του splitter
        }
         
        else {
            close(pipes[i][0]);
            int startLine = ( (i == 0) ? 1 : (i * linesForSplitter) + 1);
            //αν είναι η τελευταία γραμμή τότε το τελευταίο splitter θα πάρει τις υπόλοιπες γραμμές
            //(μπορεί να μην διαιρείται ακριβώς με τον αριθμό των splitters και να υπάρχουν κάποιες παραπάνω γραμμές)
            int endLine = (i == numOfSplitter - 1) ? lines : (i + 1) * linesForSplitter;
            
            //Διαβάζουμε το αρχείο γραμμή προς γραμμή και γράφουμε στο pipe του splitter
            if (bytesRemainingInBuffer == 0) {
                memset(buffer, 0, sizeof(buffer));
                bytesRemainingInBuffer = read(fdInput, buffer, sizeof(buffer));
                if(bytesRemainingInBuffer <= 0){
                    break;
                }
                nextCharToRead = 0;
            }
            do{

                for (int j = nextCharToRead; j < sizeof(buffer); j++) {
                    if (buffer[j] == '\n') {
                        currentLine++;
                    }
                    if (currentLine >= startLine && currentLine <= endLine) {
                        write(pipes[i][1], buffer + j, 1);
                        bytesRemainingInBuffer--;
                        nextCharToRead = j + 1;
                    }
                    if (currentLine > endLine) {
                        write(pipes[i][1], buffer + j, 1);
                        bytesRemainingInBuffer--;
                        nextCharToRead = j + 1;
                        break;
                    }
                }
                if (currentLine > endLine) {
                    break;
                }
                if (bytesRemainingInBuffer == 0) {
                    memset(buffer, 0, sizeof(buffer));
                    bytesRemainingInBuffer = read(fdInput, buffer, sizeof(buffer));
                    if(bytesRemainingInBuffer <= 0){
                        break;
                    }
                    nextCharToRead = 0;
                }
            }while(bytesRemainingInBuffer > 0);


            close(pipes[i][1]);
            // Στον γονέα, αποθηκεύουμε το PID του splitter
            splitterPids[i] = pid;
        }
    }


    // Περιμένουμε όλους τους splitters να ολοκληρωθούν
    for (int i = 0; i < numOfSplitter; i++) {
        waitpid(splitterPids[i], NULL, 0);
    }
    //TODO builders wait
    
    //στην αποδεύσμευση της λίστας θα καλείται η free για κάθε κόμβο
    //αρα θα απελευθερώνεται και ο χώρος που έχει δεσμευτεί για το string στην Main
    listDestroy(exclusionList);
}
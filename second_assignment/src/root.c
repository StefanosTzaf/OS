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

    int* bytesPerLine = malloc((lines+1) * sizeof(int));
    bytesPerLine[0] = 0;
    //η αρίθμηση των γραμμών ξεκινάει απο 1
    lines = 1;
    lseek(fdInput, 0, SEEK_SET);
    int bytesOfLine = 0;
    //μετρημα bytes ανα γραμμή για να το περάσουμε στον κάθε splitter 
    //ωστε να ξέρει από που θα ξεκινήσει να διαβάζει
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

    //κλείνουμε το αρχείο, κάθε φορά οι splitters το ανοιγουν για να μη χρειάστει να 
    //κάνουμε επανατοποθέτηση του δείκτη ανάγνωσης
    close(fdInput);
    
    //δείκτης ανάγνωσης στην αρχή του αρχείου
    memset(buffer, 0, sizeof(buffer));
    lseek(fdInput, 0, SEEK_SET);


//---------------------------------------------------------------------builders---------------------------------------------------------------------
    //δημιουργία n*l named pipes για την επικοινωνία των splitter με τους builders
    // char fifoName[1024];
    // for (int s = 0; s < numOfSplitter; s++) {
    //     for (int b = 0; b < numOfBuilders; b++) {
    //         //print σαν συμβολοσειρά τον αριθμό του named pipe
    //         //που καθορίζεται μονοσύμαντα από τον αριθμό του splitter και του builder
    //         snprintf(fifoName, 1024, "fifo_splitter%d_builder%d", s, b);
    //         if (mkfifo(fifoName, 0666) == -1) {
    //             perror("Error creating FIFO");
    //             exit(1);
    //         }
    //     }
    // }

    // pid_t builderPids[numOfBuilders];
    // for (int b = 0; b < numOfBuilders; b++) {
    //     pid_t pid = fork();
        
    //     if (pid == -1) {
    //         perror("Error forking builder process");
    //         exit(1);
    //     }
    //     else if (pid == 0) {
    //         char fifoBuilder[1024];
    //         execlp("./builder", "./builder", NULL);
    //         exit(EXIT_SUCCESS);
    //     }
    //     else {
    //         builderPids[b] = pid;
    //     }
    // }
//---------------------------------------------------------------------splitters---------------------------------------------------------------------


    //δημιουργούμε l splitters με lines/l γραμμές το καθένα
    //εκτος από το τελευταίο που ίσως πάρει λίγες παραπάνω λόγω μη τέλειας διαίρεσης
    int linesForSplitter = lines / numOfSplitter;
    pid_t splitterPids[numOfSplitter];

    int currentLine = 1;

    int bytesRemainingInBuffer = 0;
    int nextCharToRead = 0;

    for (int i = 0; i < numOfSplitter; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Error forking splitter %d");
            exit(EXIT_FAILURE);
        }

        //splitter
        else if (pid == 0){
            //κλείνουμε το γράψιμο στους splitter
            int startLine = ( (i == 0) ? 1 : (i * linesForSplitter) + 1);

            //αν είναι η τελευταία γραμμή τότε το τελευταίο splitter θα πάρει τις υπόλοιπες γραμμές
            int endLine = (i == numOfSplitter - 1) ? lines : (i + 1) * linesForSplitter;

            char start[16];
            char end[16];
            sprintf(start, "%d", startLine);
            sprintf(end, "%d", endLine);

            //μετά από πόσες γραμμές θα ξεκινήσει να διαβάζει
            int position = i*linesForSplitter;
            char firstByteForSplitter[32];
            sprintf(firstByteForSplitter, "%d", bytesPerLine[position]);
            execlp("./splitter", "./splitter", inputFile, start, end, firstByteForSplitter, NULL);

            perror("Error executing splitter");
            exit(EXIT_FAILURE);
        }
         
        else {

            // Στον γονέα, αποθηκεύουμε το PID του splitter που δημιουργήσαμε
            splitterPids[i] = pid;
        }
    }


    // Περιμένουμε όλους τους splitters να ολοκληρωθούν
    for (int i = 0; i < numOfSplitter; i++) {
        waitpid(splitterPids[i], NULL, 0);
    }
    // for (int i = 0; i < numOfBuilders; i++) {
    //     waitpid(builderPids[i], NULL, 0);
    // }

    free(bytesPerLine);
    //στην αποδεύσμευση της λίστας θα καλείται η free για κάθε κόμβο
    //αρα θα απελευθερώνεται και ο χώρος που έχει δεσμευτεί για το string στην Main
    listDestroy(exclusionList);
}

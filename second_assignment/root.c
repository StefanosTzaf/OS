#include "root.h"
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

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
    lseek(fdInput, 0, SEEK_SET);

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
            int startLine = i * linesForSplitter;
            //αν είναι η τελευταία γραμμή τότε το τελευταίο splitter θα πάρει τις υπόλοιπες γραμμές
            //(μπορεί να μην διαιρείται ακριβώς με τον αριθμό των splitters και να υπάρχουν κάποιες παραπάνω γραμμές)
            int endLine = (i == numOfSplitter - 1) ? lines : (i + 1) * linesForSplitter;

            //Γράφουμε στο pipe του splitter τις γραμμές που του αναλογούν
            int currentLine = 0;
            char buffer[4096];
            while (read(fdInput, buffer, sizeof(buffer)) > 0) {
                for (int j = 0; j < sizeof(buffer); j++) {
                    if (buffer[j] == '\n') {
                        currentLine++;
                    }
                    if (currentLine >= startLine && currentLine < endLine) {
                        write(pipes[i][1], buffer + j, 1);
                    }
                    if (currentLine >= endLine) {
                        break;
                    }
                }
                if (currentLine >= endLine) {
                    break;
                }
            }


            close(pipes[i][1]);
            // Στον γονέα, αποθηκεύουμε το PID του splitter
            splitterPids[i] = pid;
        }
    }

    // Περιμένουμε όλους τους splitters να ολοκληρωθούν
    for (int i = 0; i < numOfSplitter; i++) {
        waitpid(splitterPids[i], NULL, 0);
    }


    //στην αποδεύσμευση της λίστας θα καλείται η free για κάθε κόμβο
    //αρα θα απελευθερώνεται και ο χώρος που έχει δεσμευτεί για το string στην Main
    listDestroy(exclusionList);
}
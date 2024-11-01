#include "root.h"
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>

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

    pid_t spliterPids[numOfSplitter];

    for (int i = 0; i < numOfSplitter; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            fprintf(stderr, "Error forking splitter %d\n", i);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0){

            // Εδώ μπορείς να εκτελέσεις το πρόγραμμα του splitter
            // execvp("splitter_program", args);
            // Για παράδειγμα:
            exit(EXIT_SUCCESS); // Τερματισμός του splitter
        } 
        else {
            // Στον γονέα, αποθηκεύουμε το PID του splitter
            spliterPids[i] = pid;
        }
    }

    // Περιμένουμε όλους τους splitters να ολοκληρωθούν
    for (int i = 0; i < numOfSplitter; i++) {
        waitpid(spliterPids[i], NULL, 0);
    }
    

    //στην αποδεύσμευση της λίστας θα καλείται η free για κάθε κόμβο
    //αρα θα απελευθερώνεται και ο χώρος που έχει δεσμευτεί για το string στην Main
    listDestroy(exclusionList);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

int main(int argc, char* argv[]){
    //χωρίς ορίσματα(το αρχείο είναι ανοιχτό από τη κλήση της exec δεν το περνάμε σαν όρισμα)
    if(argc != 5){
        fprintf(stderr, "Usage: ./splitter <inputFile> <startLine> <endLine> <numberOfBytes>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        perror("Error opening file");
        exit(1);
    }
    //διαβάζουμε το αρχείο από εκεί που ξεκινάει ο συγκεκριμένος splitter
    int startLine = atoi(argv[2]);
    int endLine = atoi(argv[3]);
    int firstByteToRead = atoi(argv[4]);

    lseek(fd, firstByteToRead, SEEK_SET);
    printf("Splitter id : %d\n", getpid());

    int linesToRead = endLine - startLine + 1;

    int currentLine = startLine;
    char buffer[1024];
    bool allLinesRead = false;
    
    int bytesRead = read(fd, buffer, sizeof(buffer));

    while(bytesRead > 0){

        int size = 0;
        //αν μία λέξη τελειώνει με σημείο στίξης
        int endsWithPunct = 0;
        int capacity = 10;
        //αν μία λέξη είναι της μορφης λ3ξη$ τότε δεν είναι έγκυρη
        int valid = 1;
        int ch;
        char *word = malloc(capacity * sizeof(char));

        for (int i = 0; i < bytesRead; i++) {
            char ch = buffer[i];
            if (ch == '\n') {
                currentLine++;
            }
            //αν φτάσαμε στην επόμενη γραμμή, από την τελευταία η δουλειά του splitter έχει τελειώσει
            if(currentLine == atoi(argv[3]) + 1){
                allLinesRead = true;
                break;
            }

            if (isalpha(ch)) {
                //δυναμική αύξηση του word (δεν έχουμε όριο στο μέγεθος της λέξης)
                if (size >= capacity - 1) {
                    capacity *= 2;
                    word = realloc(word, capacity * sizeof(char));
                }
                word[size++] = ch;
                endsWithPunct = 0;
                valid = 1;
                continue;
            } 
            // else if ((ch == '!' || ch == '?' || ch == '.') && (size > 0) && (valid)) {
            //     endsWithPunct = 1;
            //     word[size] = '\0';
            //     printf("Read valid word: %s\n", word);
            //     size = 0;
            // } 
            else if (isspace(ch) ) {
                if (size > 0 && valid) {
                    if (size > 0) {
                        word[size] = '\0';
                        printf("Read valid word: %s\n", word);
                    }
                }
                //για την επόμενη λέξη
                size = 0;
                valid = 1;
                endsWithPunct = 0;
            } 
            // else {
            //     //σε περίπτωση που έχουμε μη εγκυρο χαρακτήρα
            //     valid = 0;
            //     //προσπερνάμε τους χαρακτήρες μέχρι να βρούμε κενό
            //     while ((ch != EOF) && (ch != ' ' ) && (ch != '\n')) {
            //         printf("Skipping invalid character: %c\n", ch);
            //         ch = getchar();
            //     }
            //     size = 0;

            // }
            
        }
        // Handle the last word if input didn't end with a space
        // if (size > 0 && valid) {
        //     if (endsWithPunct || size > 0) {
        //         word[size] = '\0';
        //         printf("Read valid word: %s\n", word);
        //     }
            

        free(word);


        if(allLinesRead){
            break;
        }
        //εάν έχουμε διαβάσει όλο τον buffer και δεν έχουμε διαβάσει
        //τις γραμμές που αναλογούν στον splitter τότε διαβάζουμε ξανά

        if(bytesRead == 0){
            bytesRead = read(fd, buffer, sizeof(buffer));
        }

    }

    close(fd);
    return 0;

}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[]){
    //χωρίς ορίσματα(το αρχείο είναι ανοιχτό από τη κλήση της exec δεν το περνάμε σαν όρισμα)
    if(argc != 1){
        fprintf(stderr, "Usage: ./splitter <startLine> <endLine> <file descriptor>\n");
        return 1;
    }
    
    int capacity = 10;
    char *word = malloc(capacity * sizeof(char));
    int size = 0;
    //αν μία λέξη είναι της μορφης λ3ξη$ τότε δεν είναι έγκυρη
    int valid = 1;
    //αν μία λέξη τελειώνει με σημείο στίξης
    int endsWithPunct = 0;
    int ch;
    while ((ch = getchar()) != EOF) {
        if (isalpha(ch)) {
            //δυναμική αύξηση του buffer (δεν έχουμε όριο στο μέγεθος της λέξης)
            if (size >= capacity - 1) {
                capacity *= 2;
                word = realloc(word, capacity * sizeof(char));
            }
            word[size++] = ch;
            endsWithPunct = 0;
            valid = 1;
            continue;
        } 
        else if ((ch == '!' || ch == '?' || ch == '.') && (size > 0) && (valid)) {
            endsWithPunct = 1;
            word[size] = '\0';
            printf("Read valid word: %s\n", word);
            size = 0;
        } 
        else if (isspace(ch)) {
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
        else {
            //σε περίπτωση που έχουμε μη εγκυρο χαρακτήρα
            valid = 0;
            //προσπερνάμε τους χαρακτήρες μέχρι να βρούμε κενό
            while ((ch != EOF) && (ch != ' ' ) && (ch != '\n')) {
                printf("Skipping invalid character: %c\n", ch);
                ch = getchar();
            }
            size = 0;

        }
    }

    // Handle the last word if input didn't end with a space
    // if (size > 0 && valid) {
    //     if (endsWithPunct || size > 0) {
    //         word[size] = '\0';
    //         printf("Read valid word: %s\n", word);
    //     }
        
    // }
    free(word);


     return 0;

}
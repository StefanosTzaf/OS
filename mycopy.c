#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {


    if (argc != 3) {
        printf("3 Arguments are required\n");
        return 1;
    }

    const char* source_file = argv[1];
    const char* destination_file = argv[2];

    FILE *src, *dest;
    char buffer[4096]; // Μεγέθος buffer για αντιγραφή
    size_t bytes;

    // Άνοιγμα αρχείου προέλευσης
    src = fopen(source_file, "rb");
    if (src == NULL) {
        perror("Error opening source file");
        return 1; // Επιστροφή κωδικού σφάλματος
    }

    // Άνοιγμα αρχείου προορισμού
    dest = fopen(destination_file, "wb");
    if (dest == NULL) {
        perror("Error opening destination file");
        fclose(src); // Κλείσιμο αρχείου προέλευσης
        return 1; // Επιστροφή κωδικού σφάλματος
    }

    // Αντιγραφή περιεχομένου
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    // Έλεγχος σφαλμάτων κατά την ανάγνωση
    if (ferror(src)) {
        perror("Error reading from source file");
    }

    // Κλείσιμο αρχείων
    fclose(src);
    fclose(dest);

    return 0;
}
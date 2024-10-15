#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "3 arguements are needed: mycopy <source file> <destination file>\n");
        return -1;
    }

    //Aνοιγμα αρχείου (πρωτο syscall μεσα στην open)
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        fprintf(stderr, "Error opening source file\n");
        return -1;
    }

    //Aνοιγμα αρχείου για αντιγραφη (δεύτερο syscall μεσα στην open) δημιουργία αν το αρχείο δεν υπάρχει
    //το 0644 έχει να κάνει με τα δικαιώματα του αρχείου σε user group και others
    int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    //Διαβάζουμε κάθε φορά στον buffer και γράφουμε στο outoput file(2 syscalls)
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
    }
    //άλλα 2 syscalls
    close(src_fd);
    close(dest_fd);
    return 0;
}
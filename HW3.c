/*  Christina Harford
 *
 *  In accordance with the UNC Honor pledge, I certify that I have neither
 *  given nor recieved unauthorized aid on this assignment.
 *
 *  This program reads in lines of input from the keyboard, replaces every
 *  carriage return with a space, replaces every pair of asterisks (**) with a
 *  carrot (^), and prints out 80 character lines of output with a carriage
 *  return on the end.  When the EOF character is read, the program terminates.
 *  The program implements this functionality using threads and buffers
 *  (defined in buffer.c and buffer.h) and a producer/consumer system. These
 *  threads work together a pipeline system, with each thread processing the
 *  input in a different manner. There are four threads and three buffers, and
 *  each thread acts as a consumer for the previous thread and a producer
 *  for the next thread. When an EOF character is read, each thread stores
 *  the EOF into the next buffer (so that the next will know to terminate) and
 *  terminates itself.
 */

// Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Library and custom includes
#include "st.h"
#include "buffer.h"

// One extra char on the end of each to be terminated by a null character
#define OUTPUT_LENGTH 80

// Functions for the execution of each thread.
void *getInputThread(void *state);
void *convertNewlineThread(void *state);
void *convertAsterisksThread(void *state);
void *outputThread(void *state);

// Buffers that act as producers and consumers
buffer *inputBuffer;
buffer *newlineBuffer;
buffer *asteriskBuffer;

// Creates four threads that execute and terminate on their own
int main (int argc, char const *argv[]) {
    // Initialize buffers
    inputBuffer = createBuffer(OUTPUT_LENGTH);
    newlineBuffer = createBuffer(OUTPUT_LENGTH);
    asteriskBuffer = createBuffer(OUTPUT_LENGTH);

    // Initialize the libST runtime.
    st_init();

    // Create threads; if there is an error, print out message to user and exit
    // Create thread that obtains character input
    if (st_thread_create(getInputThread, NULL, 0, 0) == NULL) {
        perror("st_thread_create failed for thread a");
        exit(EXIT_FAILURE);
    }
    // Create thread that converts each newline character to a space
    if (st_thread_create(convertNewlineThread, NULL, 0, 0) == NULL) {
        perror("st_thread_create failed for thread b");
        exit(EXIT_FAILURE);
    }
    // Create thread that converts adjacent asterisks to a carrot
    if (st_thread_create(convertAsterisksThread, NULL, 0, 0) == NULL) {
        perror("st_thread_create failed for thread c");
        exit(EXIT_FAILURE);
    }
    // Create thread that writes characters to output
    if (st_thread_create(outputThread, NULL, 0, 0) == NULL) {
        perror("st_thread_create failed for thread d");
        exit(EXIT_FAILURE);
    }

    // Once threads have terminated, exit main thread
    st_thread_exit(NULL);

    // Free objects that were malloced
    free(inputBuffer->bufArray);
    free(inputBuffer);
    free(newlineBuffer->bufArray);
    free(newlineBuffer);
    free(asteriskBuffer->bufArray);
    free(asteriskBuffer);

    return 0;
}

// Thread function that obtains character input and puts it into first buffer
void *getInputThread(void *state) {
    char current;
    // Reads input one character at a time. When EOF is read, thread terminates
    // While loop continues to execute until EOF read
    while((current = getc(stdin)) != EOF) {
      deposit(inputBuffer, current);
    }
    // Once EOF is reached, deposits character into buffer and terminates thread
    deposit(inputBuffer, current);
    st_thread_exit(NULL);
}

// Thread function that converts newline characters from prev buffer to spaces
void *convertNewlineThread(void *state) {
    char current;
    // Obtains char from previous buffer one at a time
    // Loop continues to execute until EOF is read; then the thread exits
    while((current = remoove(inputBuffer)) != EOF) {
      // If charcter is a newline, put space into next buffer
      if (current == '\n') {
        deposit(newlineBuffer, ' ');
      }
      // Otherwise, simply put charcter into next buffer
      else {
        deposit(newlineBuffer, current);
      }
    }
    // Once EOF is reached, deposits character into buffer and terminates thread
    deposit(newlineBuffer, current);
    st_thread_exit(NULL);
}

// Thread function that converts two adjacent asterisks to a carrot
void *convertAsterisksThread(void *state) {
    // Static variable previous maintains the previous character that was read
    static char previous = '\0';
    char current;
    // Obtains char from previous buffer one at a time
    // Loop continues to execute until EOF is read; then the thread exits
    while((current = remoove(newlineBuffer)) != EOF) {
      if (previous == '*') {
        // If both previous and current char are *, put a carrot in the buffer
        if (current == '*') {
           current = '^';
           deposit(asteriskBuffer, current);
        }
        // Otherwise, put both previous and current into buffer
        else {
            deposit(asteriskBuffer, previous);
            deposit(asteriskBuffer, current);
        }
      }
      // If current is not an *, simply deposit into buffer
      else if (current != '*') {
          deposit(asteriskBuffer, current);
      }
      // Set previous for next loop iteration
      previous = current;
    }
    // Once EOF is reached, deposits character into buffer and terminates thread
    deposit(asteriskBuffer, current);
    st_thread_exit(NULL);
}

void *outputThread(void *state) {
    char output[OUTPUT_LENGTH];
    memset(output, 0, sizeof(output));
    int i = 0;
    char current;
    // Obtains char from previous buffer one at a time
    // Loop continues to execute until EOF is read; then the thread exits
    while((current = remoove(asteriskBuffer)) != EOF) {
        // Save characters to output in character array until we reach max
        output[i] = current;
        // i keeps track of how many output characters we have in our array
        i++;
        // Once we reach max of output characters, print out and reset values
        if (i == OUTPUT_LENGTH) {
            printf("%s\n", output);
            memset(output, 0, sizeof(output));
            i = 0;
        }
    }
    // Once EOF is reached, terminates thread
    st_thread_exit(NULL);
}

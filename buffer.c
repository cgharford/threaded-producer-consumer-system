/*  Defines implementation of the methods declared in buffer.c.
 */

// Standard libraries included
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Custom libraries included
#include "buffer.h"

// Creates buffer struct and initializes proper values
buffer* createBuffer(int size) {
  // Allocate memory for buffer struct
  buffer *buf;
  // If initial malloc returns null, attempt to free and malloc again
  if ((buf = malloc(sizeof(buffer))) == NULL) {
    free(buf);
    // If second malloc fails, exit program after printing error message
    if ((buf = malloc(sizeof(buffer))) == NULL) {
      printf("Malloc error; program terminated\n");
      exit(EXIT_FAILURE);
    }
  }
  buf->n = size;
  // nextIn and nextOut initially point to same location; character counter is 0
  buf->nextIn = 0;
  buf->nextOut = 0;
  buf->counter = 0;
  // Create semaphores that keep track of empty and full buffers
  createSem(&buf->emptyBuffers, buf->n);
  createSem(&buf->fullBuffers, 0);
  // Allocate memory for buffer array
  // If initial malloc returns null, attempt to free and malloc again
  if ((buf->bufArray = malloc(sizeof(char)*buf->n)) == NULL) {
    free(buf->bufArray);
    // If second malloc fails, exit program after printing error message
    if ((buf->bufArray = malloc(sizeof(char)*buf->n)) == NULL) {
      printf("Malloc error; program terminated\n");
      exit(EXIT_FAILURE);
    }
  }
  return buf;
}

// Producer functionality; puts a character into buffer
void deposit(buffer* buf, char c) {
  // Must bracket code with a mutual exclusion lock
  down(&buf->emptyBuffers);
  // Ensure that the condition down finished on is still true
  assert(buf->counter < buf->n);
  // Put charcter into buffer, increment character counter
  buf->bufArray[buf->nextIn] = c;
  buf->nextIn = (buf->nextIn + 1) % buf->n;
  buf->counter += 1;
  assert(buf->counter > 0);
  up(&buf->fullBuffers);
}

// Consumer functionality; removes a charcter from buffer and returns it
char remoove(buffer* buf) {
  // Must bracket code with a mutual exclusion lock
  down(&buf->fullBuffers);
  // Ensure that the condition down finished on is still true
  assert(buf->counter > 0);
  // Remove charcter from buffer, decrement character counter
  char c = buf->bufArray[buf->nextOut];
  buf->nextOut = (buf->nextOut + 1) % buf->n;
  buf->counter -= 1;
  assert(buf->counter < buf->n);
  up(&buf->emptyBuffers);
  return c;
}

/*  Defines the buffer struct and methods that provide the resources needed
 *  to implement a producer/consumer system.  The methods are defined in
 *  buffer.c.
 */

// Library and custom includes
#include "st.h"
#include "semaphore.h"

// Defines the buffer struct and elements within
typedef struct {
  semaphore fullBuffers;
  semaphore emptyBuffers;
  int nextIn;
  int nextOut;
  int n;
  int counter;
  char *bufArray;
} buffer;

// Methods associated with the buffer to be implemented
void deposit(buffer* buf, char c);
char remoove(buffer* buf);
buffer* createBuffer(int size);

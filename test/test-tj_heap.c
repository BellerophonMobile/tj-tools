/*
 * Copyright (c) 2011 Joe Kopena <tjkopena@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tj_heap.h>

int fail = 0;

#define FAIL(M, ...) {fail = 1; printf("FAIL: " M "\n", ##__VA_ARGS__);}

int less(int a, int b) { return a  < b; }

TJ_HEAP_DECL(myheap, int, char *, less);

TJ_HEAP_IMPL(myheap);


int
main(int argc, char *argv[])
{

  // Skipping the error checking...

  myheap *heap = myheap_create(4);

  myheap_add(heap, 234, "mushi");
  myheap_add(heap, 23, "foobar");
  myheap_add(heap, 467, "sushi");



  int i;
  for (i = 0; i < heap->m_used; i++) {
    printf("%d\n", heap->m_array[i].m_key);
  }
  myheap_finalize(heap);

  //--------------------------------------------
  //  heap = tj_heap_create(24);

  /*
Algorithm add(newEntry)
if (the array heap is full)
	Double the size of the array
newIndex = index of next available array location
parentIndex = newIndex/2 // index of parent of available location
while (newEntry > heap[parentIndex])
{	heap[newIndex] = heap[parentIndex] 
	// move parent to available location
	// update indices
	newIndex = parentIndex
	parentIndex = newIndex/2
} // end while
  */



  //--------------------------------------------
     //  tj_heap_finalize(heap);

  if (fail) {
    printf("** There were errors.\n");
    return -1;
  }

  printf("Done.\n");

  return 0;
  // end main
}

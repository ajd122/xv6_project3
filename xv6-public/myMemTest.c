#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>

int main()
{
    //grab 1000 bytes of memory for the process, putting it likely in the middle of a page
    char *str;
    str = (char *) malloc(1000);

    //print the address of this memory to show where the page is
    printf("Address = %u\n", str);

    for(int i=0; i < 1000){

        //wait one second
        delay(1000);
		
		//begin timer for purpose of measuring performance difference
		struct timeval stop, start;
		gettimeofday(&start, NULL);

		//allocate a full page for the process, and return the address of where the memory space begins
        str = (char *) malloc(4096)
		
		//close timer
		gettimeofday(&stop, NULL);
		
		
        printf("Address = %u\n took %lu seconds\n", str, stop.tv_usec - start.tv_usec);
    }
}
#include<stdio.h>
#include<stdlib.h>

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

	//allocate a full page for the process, and return the address of where the
        //memory space begins
        str = (char *) malloc(4096)
        printf("Address = %u\n", str);
    }
}
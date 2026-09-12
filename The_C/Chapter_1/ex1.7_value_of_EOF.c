//Write a program to print the value of EOF.
#include <stdio.h>

int main(void){
    int c;
    
        while((c = getchar()) != EOF)
        ;

    printf("\n%d\n", c);
    return 0;        
}
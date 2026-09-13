//Write a program to count blanks, tabs, and newlines.
#include <stdio.h>

int main(void){
    int nl, c;
    nl = 0;

    while ((c = getchar()) != EOF) 
        if (c == '\t') //' ' - for counting blanks. '\t' - tabs. '\n' - newlines
            ++nl;

    printf("\n%d\n", nl);
    return 0;
}

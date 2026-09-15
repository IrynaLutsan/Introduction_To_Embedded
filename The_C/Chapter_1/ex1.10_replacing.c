//Write a program to copy its input to its output, replacing each tab by \t, each backspace by \b, 
//and each backslash by \\. This makes tabs and backspaces visible in an unambiguous way.

//Напишіть програму для копіювання її вхідних даних на вихід, замінивши кожну вкладку на \t, кожен backspace на \b, 
//а кожну зворотну косу риску на \\. Це робить вкладки та backspaces видимими однозначно.

#include <stdio.h>

int main(void){
    int c;
    char slash = '\\';
    char t = 't';
    char b = 'b';

    while ((c = getchar()) != EOF){
        switch(c){
            case '\t':
                putchar(slash);
                putchar(t);
                break;
            case '\b':
                putchar(slash);
                putchar(b);
                break;
            case '\\':
                putchar(slash);
                putchar(slash);
                break;
            default:
                putchar(c);
        }
    }
    return 0;
}
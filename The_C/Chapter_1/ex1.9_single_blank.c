//Write a program to copy its input to its output, replacing each string of one or more blanks by a single blank.
//Напишіть програму для копіювання вхідних даних на вихід, замінивши кожен рядок одного або декількох пробілів одним пробілом.

//якщо зустрічається кілька пробілів підряд:
//"   " → " "
//тобто всі повторювані пробіли треба звести до одного

#include <stdio.h>
#include <stdbool.h>

int main(void){
    int c;
    bool prev_c = false;

    while ((c = getchar()) != EOF)

        if (c == ' ') {
            if (!prev_c) {
                prev_c = true;
                putchar(c);
            }
        } else {
            prev_c = false;
            putchar(c);
        }
}
//First implementation. More complecated:
// int main(void){
//     int c;
//     bool prev_c = false;

//     while ((c = getchar()) != EOF)
// if (c == ' ' && prev_c == true){
        //     ;
        // } else if ((c != ' ') && prev_c == true){
        //     prev_c = false;
        //     putchar(c);
        // } else if (c == ' ' && prev_c == false){ 
        //     prev_c = true;
        //     putchar(c);
        // } else {
        //     putchar(c);

        // }
#include <stdio.h>

int main(){
    // c = (5/9)(f-32)

    float fahr = 0;
    float celsius = 0;
    int lower = 0;
    int upper = 300;
    int step = 20;

    printf("Fahrenheit-Celsius table:\n");

    while (fahr <= upper) {

        celsius = 5.0*(fahr - 32.0)/9.0;
        printf("%3.0f\t %.2f\n", fahr, celsius);
        fahr += step;
    }
}

// int main(){
//     // c = (5/9)(f-32)

//     int fahr = 0;
//     int celsius = 0;
//     int lower = 0;
//     int upper = 300;
//     int step = 20;

//     while (fahr <= upper) {

//         celsius = 5*(fahr - 32)/9;
//         printf("%d\t %d\n", fahr, celsius);
//         fahr += step;
//     }
// }
#include <string.h>

#define N 97

void sieveOfEratosthenes(int numbers[], int n) {
    int prime = 2;
    while (1) {
        int tmp = prime * prime;
        if (tmp > n) {
            break;
        }

        while (tmp <= n) {
            numbers[tmp - 2] = -1;
            tmp += prime;
        }

        ++prime;
        for (; prime <= n && numbers[prime - 2] == -1; ++prime) {}
        if (prime * prime >= n) {
            break;
        } 
    }
}


int main() {
    int numbers[N] = {0};

    numbers[0] = 2;
    for (int i = 3; i <= N; ++i) {
        if (i % 2 == 1) {
            numbers[i - 2] = i;
        } else {
            numbers[i - 2] = -1;
        }
    }
    sieveOfEratosthenes(numbers, N);

    return numbers[N - 2];  // must return 97
}

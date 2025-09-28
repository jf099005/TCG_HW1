int is_prime(int n)
{
    if (n == 2 || n == 3) {
        return 1;
    }

    if (n <= 1 || n % 2 == 0 || n % 3 == 0) {
        return 0;
    }

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return 0;
        }
    }

    return 1;
}
#include<stdio.h>
int main()
{
    #define GOAL 23614847
    int count = 1;
    for (int i = 6; i < GOAL; i += 6) {
        count += is_prime(i - 1) && is_prime(i + 1);
        if(count == 42 && is_prime(i - 1) && is_prime(i + 1)){
            printf("at count = 42: %d\n", i);
        }
    }
    printf("total numbre of pairs: %d", count);
    return 0;
}
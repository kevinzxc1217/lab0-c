#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

int i_sqrt(int x)
{
    if (x <= 1) /* Assume x is always positive */
        return x;
    int msb;
    int tmp = x;
    while(tmp){
        tmp = tmp >> ffs(x);
        msb += ffs(x);
    }

    int z = 0;
    for (int m = 1UL << (msb & ~1UL); m; m >>= 2) {
        int b = z + m;
        z >>= 1;
        
        int mask = -(x >= b);
        x -= b & mask;
        z += m & mask;
    }
    return z;
}

int main()
{
    int ans = i_sqrt(64);
    printf("%d",ans);

    return 0;
}
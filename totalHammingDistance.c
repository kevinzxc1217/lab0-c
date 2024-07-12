#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h> 

int totalHammingDistance(int* nums, int numsSize)
{
    int total = 0;
    int tmp = 0;
    for (int i = 0;i < 32;i++){
        for (int j = 0; j < numsSize;j++){
            if((nums[j] >> i) & 1){
                tmp += 1;
            }
        }
        total += tmp * (numsSize - tmp);
        tmp = 0;
    }
    return total;
}

int main()
{
    int nums[4] = {4, 14, 2};
    int ans = totalHammingDistance(nums, 3);
    printf("%d",ans);

    return 0;
}
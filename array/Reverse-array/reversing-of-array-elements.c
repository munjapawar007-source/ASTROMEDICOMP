#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// function declarations
void reversing_of_array_elements(void);

int main()
{
    reversing_of_array_elements();

    return 0;
}

void reversing_of_array_elements()
{
    int *arr, *reverse_num, num, i;

    printf("Enter array of index:");
    scanf("%d", &num);

    arr = (int*)malloc(num * sizeof(int));
    reverse_num = (int*)malloc(num * sizeof(int));

    // fill array with elements
    for(i = 0; i < num; i++)
    {
        printf("Enter elements of array[%d]:",i);
        scanf("%d", arr + i);
    }

    for(i = 0; i < num; i++)
    {
        reverse_num[i] = arr[num - 1 - i];
        printf("reversed elements in array[%d] :%d\n", i, reverse_num[i]);
    }
}


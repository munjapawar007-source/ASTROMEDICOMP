#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(void)
{
    char date_str[] = "13/11/2025";
    char vertex_entry[] = "v 1.1 2.2 3.3";

    char* token = NULL;
    char* date_separator = "/";
    char* vertex_entry_separator = " ";

    int day, month, year;
    float x, y, z;

    token = strtok(date_str, date_separator);
    printf("token = %s\n", token);
    day = atoi(token);
    printf("day = %d\n", day);

    token = strtok(NULL, date_separator);
    printf("tokem = %s\n",token);
    month = atoi(month);
    printf("month = %d\n",month);

    token = strtok(NULL, date_separator);
    printf("tokem = %s\n",token);
    year = atoi(year);
    printf("year = %d\n",year);



    return (EXIT_SUCCESS);
}
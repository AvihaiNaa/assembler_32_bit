
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "utility.h"
#include "global.h"
#include "assembler_interface.h"
#include "unit_tests.h"

/*Compares 2 files*/
bool compare_files(FILE* fp1, FILE* fp2)
{
    /* fetching character of two file*/
    /* in two variable ch1 and ch2*/
    char ch1 = getc(fp1);
    char ch2 = getc(fp2);

    /* error keeps track of number of errors*/
    /* pos keeps track of position of errors*/
    /* line keeps track of error line*/
    int error = 0, pos = 0, line = 1;

    /* iterate loop till end of file*/
    while (ch1 != EOF && ch2 != EOF)
    {
        pos++;

        /* if both variable encounters new*/
        /* line then line variable is incremented*/
        /* and pos variable is set to 0*/
        if (ch1 == NEW_LINE && ch2 == NEW_LINE)
        {
            line++;
            pos = 0;
        }

        /* if fetched data is not equal then*/
        /* error is incremented*/
        if (ch1 != ch2)
        {
            error++;
            printf("Line Number : %d \tError"
                " Position : %d \n", line, pos);
        }

        /* fetching character until end of file*/
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }

    printf("Total Errors : %d\t", error);
    return error == 0;
}


/*Loads 2 files using 2 file name and opens them, and initiates the comparision*/
bool crossing_result(char* first, char* second) {
    bool status;
    /*Opening both file in read only mode*/
    FILE* fp1 = fopen(first, "r");
    FILE* fp2 = fopen(second, "r");

    if (fp1 == NULL || fp2 == NULL)
    {
        printf("Error : Files not open");
        exit(0);
    }
    status = compare_files(fp1, fp2);
    if (status)
        printf("\n\nScenario test - passed\n\n");
    else
        printf("\n\nScenario test - failed\n\n");

    /* closing both file*/
    fclose(fp1);
    fclose(fp2);
    return status;
}
/* Driver code*/


/*Test the result - load sevral example files, process them - and compare the results*/
bool unit_tests() {
    char* first;
    char* second;
    bool test1, test2, test3;
    /*test ps file*/
    start_processing_file("C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\ps");
    first = "C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\ps.ob";
    second = "C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\ps.expected.ob";
    test1 = crossing_result(first, second);


    /*test work file*/
    start_processing_file("C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\work");
    first = "C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\work.ob";
    second = "C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\work.expected.ob";
    test2 = crossing_result(first, second);

    /*test assembly_file file*/
    start_processing_file("C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\assembly_file");
    first = "C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\assembly_file.ob";
    second = "C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\assembly_file.expected.ob";
    test3 = crossing_result(first, second);
    printf("\nAll Tests is Finished!\n\n");
    if(test1 & test2 & test3)
        printf("\nThe tests passed successfully\n");
    else
        printf("Some of the tests did not pass\n");

    return test1 & test2 & test3;
}


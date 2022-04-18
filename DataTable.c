/* DataTable driver */

#include <stdio.h>
#include <string.h>
#include "DataTableFunctions.h"

void my_func(column *my_column, TYPE t, int len);

int main()
{
    label_t labels[7] = {
            {"id", FLOAT},
            {"mobile", FLOAT},
            {"name", STRING},
            {"surname", STRING},
            {"profession", STRING},
            {"salary", FLOAT},
            {"dob", STRING}
    };

    printf(">> Initialising DataTable A\n");
    DataTable_t *table_a = initDT(labels, 7);

    char *path = "data_b.csv";
    printf(">> Loading ./%s into DataTable A\n", path);
    loadDT(path, table_a);

    printf("Contents of DataTable A:\n");
    showDT(table_a);

    printf(">> Projecting the first 5 elements of columns 2 - 5 of DataTable A into DataTable B\n", path);
    DataTable_t *table_b = projectDT(table_a, 2, 5, 0, 4);

    printf("Contents of DataTable B:\n");
    showDT(table_b);

    printf(">> Mutating column 3 of DataTable B\n");
    mutateDT(my_func, table_b, 3);

    printf("Contents of DataTable B:\n");
    showDT(table_b);

    char *new_path = "new_b.csv";
    printf(">> Exporting contents of DataTable B into ./%s\n", new_path);
    exportDT(table_b, new_path);

    return 0;
}

void my_func(column *my_column, TYPE t, int element_count)
{
    switch (t)
    {
        case FLOAT:;
            for (int i = 0; i < element_count; i++)
            {
                my_column->f[i] += 1000;
            }

            break;

        case STRING:;
            char str[CAT_BUF];

            for (int i = 0; i < element_count; i++)
            {
                int app = i + 1;

                sprintf(str, "_%d", app);

                strcat(my_column->s[i], str);
            }

            break;
    }
}
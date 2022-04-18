/* DataTable operation functions */

#include <stdio.h>
#include <string.h> // For strtok()
#include <stdlib.h> // For malloc(), calloc(), realloc(), free()
#include "DataTableFunctions.h"

DataTable_t *initDT(label_t *labels, int label_count)
{
    DataTable_t *table;

    // malloc() DataTable_t
    table = malloc(sizeof(*table));

    table->loaded = 0;

    table->label_count = label_count;

    table->labels_str = 0;
    table->labels_end = label_count - 1;

    // malloc() labels
    table->labels = malloc(label_count * sizeof(**(table->labels)));

    // malloc() columns
    table->columns = malloc(label_count * sizeof(*(table->columns)));

    for (int i = 0; i < label_count; i++)
    {
        // malloc() label_t's
        table->labels[i] = malloc(sizeof(*(table->labels[i])));
        *(table->labels[i]) = labels[i];

        switch (labels[i].label_type)
        {
            // malloc() float pointer if column contains floats
            case FLOAT:
                (table->columns[i]).f = calloc(CHUNK, sizeof(*((table->columns[i]).f)));
                
                break;

            // malloc() string pointer if column contains strings
            case STRING:
                (table->columns[i]).s = calloc(CHUNK, sizeof(*((table->columns[i]).s)));

                for (int s_i = 0; s_i < 10; s_i++)
                {
                    (table->columns[i]).s[s_i] = calloc(1, sizeof(*((table->columns[i]).s[s_i]))); // Store 1 char by default
                }

                break;

            default:
                printf("Incorrect type inputted\n");
                return NULL;
        }
    }

    return table;
}

void deinitDT(DataTable_t **table)
{
    for (int i = 0; i < (*table)->label_count; i++)
    {
        switch (((*table)->labels[i])->label_type)
        {
            // free() float column pointers
            case FLOAT:
                free((*table)->columns[i].f);
                (*table)->columns[i].f = NULL;
                
                break;

            // free() string_t pointers
            case STRING:
                if (i < (*table)->labels_str || i > (*table)->labels_end)
                {
                    // Setting non-populated string_t's to NULL (default allocated CHUNK, accomodate projectDT()) 
                    for (int s_i = 0; s_i < CHUNK; s_i++)
                    {
                        ((*table)->columns[i]).s[s_i] = NULL;
                    }
                }
                else
                {
                    // Setting present string_t's to NULL (accomodate projectDT())
                    for (int s_i = 0; s_i < (*table)->element_count; s_i++)
                    {
                        ((*table)->columns[i]).s[s_i] = NULL;
                    }
                }

                // free() string column pointer
                free(((*table)->columns[i]).s); 
                ((*table)->columns[i]).s = NULL;

                break;
        }

        // free() current label pointer
        free((*table)->labels[i]); 
        (*table)->labels[i] = NULL;
    }

    // free() label pointer pointer
    free((*table)->labels); 
    (*table)->labels = NULL;

    // free() column pointer
    free((*table)->columns); 
    (*table)->columns = NULL;

    // free() table pointer
    free(*table); 
    *table = NULL;

    printf(">> Deinitialised successfully\n");
}

void loadDT(char path[], DataTable_t *table)
{
    if (table == NULL)
    {
        printf("|| Invalid table\n");
        return;
    }
    else
    {
        table->element_count = 0;

        FILE *in;

        if ((in = fopen(path, "r")) == NULL)
        {
            printf("Error opening file!\n");
            return;
        }

        char *buffer;
        int bufsize = 1024;

        // Buffer for .csv lines
        buffer = malloc(bufsize * sizeof(*buffer));

        char *token;

        int i;
        int c = 0;

        // Read .csv line-by-line
        while (fgets(buffer, bufsize, in))
        {
            /* Dynamic memory allocation --> assign memory location for         */
            /* CHUNK more elements once assigned memory is going to be exceeded */
            if (table->element_count != 0 && table->element_count % CHUNK == 0)
            {
                for (i = 0; i < table->label_count; i++)
                {
                    switch ((table->labels[i])->label_type)
                    {
                        // Allocate memory for CHUNK more floats
                        case FLOAT:
                            table->columns[i].f = realloc((table->columns[i]).f,
                                                        ((table->element_count / CHUNK) + 1) * CHUNK *
                                                            sizeof(*(table->columns[i].f)));

                            if ((table->columns[i].f) == NULL)
                            {
                                printf("Error allocating memory\n");
                                exit(EXIT_FAILURE);
                            }

                            break;

                        // Allocate memory for CHUNK more strings
                        case STRING:
                            (table->columns[i]).s = realloc((table->columns[i]).s,
                                                            ((table->element_count / CHUNK) + 1) * CHUNK *
                                                                sizeof(*((table->columns[i]).s)));

                            if ((table->columns[i].s) == NULL)
                            {
                                printf("Error allocating memory\n");
                                exit(EXIT_FAILURE);
                            }

                            /* Allocate memory for 1 char in each of the CHUNK strings above,   */
                            /* these are later reallocated based on the length of strings found */
                            /* in the .csv file                                                 */
                            int start = (table->element_count / CHUNK) * CHUNK;

                            for (int s_i = start; s_i < start + CHUNK; s_i++)
                            {
                                (table->columns[i]).s[s_i] = calloc(1, sizeof(*((table->columns[i]).s[s_i])));
                            }

                            break;
                    }
                }
            }

            /* Obtain tokens from .csv lines delimited by ;, assigning each token to its */
            /* corresponding column in the DataTable_t                                   */
            token = strtok(buffer, ";");

            for (i = 0; i < table->label_count; i++)
            {
                switch ((table->labels[i])->label_type)
                {
                    // Convert token to double and assign to column as float_t
                    case FLOAT:
                        (table->columns[i]).f[c] = strtod(token, NULL);
                        break;

                    // Assign token to column as string_t
                    case STRING:
                        // realloc() column string based on length of string in file + CAT_BUF for use in mutateDT()
                        (table->columns[i]).s[c] = realloc((table->columns[i]).s[c], (strlen(token) + 1) + CAT_BUF);

                        strcpy((table->columns[i]).s[c], token);

                        // Cater for string columns as the last column of the .csv file
                        if ((table->columns[i]).s[c][strlen(token) - 1] == '\n') // exclude final line of .csv file
                        {
                            (table->columns[i]).s[c][strlen(token) - 1] = '\0';
                        }

                        break;
                }

                token = strtok(NULL, ";");
            }

            c++;
            table->element_count++;
        }

        free(buffer);
        fclose(in);

        table->loaded = 1;
    }
}

void exportDT(DataTable_t *table, char path[])
{
    if (table == NULL || table->loaded == 0)
    {
        printf("|| Invalid table\n");
        return;
    }
    else
    {
        FILE *out;

        out = fopen(path, "w");

        // Accomodate for projectDT()
        int labels_str = table->labels_str;
        int labels_end = table->labels_end;

        for (int j = 0; j < table->element_count; j++)
        {
            for (int i = labels_str; i <= labels_end; i++)
            {
                // Last column, omit ;
                if (i == labels_end)
                {
                    switch ((table->labels[i])->label_type)
                    {
                        case FLOAT:
                            fprintf(out, "%f", (table->columns[i]).f[j]);
                            break;

                        case STRING:
                            fprintf(out, "%s", (table->columns[i]).s[j]);
                            break;
                    }
                }
                else
                {
                    switch ((table->labels[i])->label_type)
                    {
                        case FLOAT:
                            fprintf(out, "%f;", (table->columns[i]).f[j]);
                            break;

                        case STRING:
                            fprintf(out, "%s;", (table->columns[i]).s[j]);
                            break;
                    }
                }
            }

            // Print new-line after every line except the last one
            if (j != table->element_count - 1)
            {
                fprintf(out, "%s", "\n");
            }
        }

        fclose(out);
    }
}

void showDT(DataTable_t *table)
{
    if (table == NULL || table->loaded == 0)
    {
        printf("|| Invalid table\n");
        return;
    }
    else
    {
        // Accomodate for projectDT()
        int labels_str = table->labels_str;
        int labels_end = table->labels_end;

        // Show label names
        for (int i = labels_str; i <= labels_end; i++)
        {
            printf("%10.10s\t", (table->labels[i])->label_name);
        }

        printf("\n");

        // Show data
        // j < table->element_count --> accomodate for DataTable_t's with less than 10 elements
        for (int j = 0; j < 10 && j < table->element_count; j++) 
        {
            for (int i = labels_str; i <= labels_end; i++)
            {
                switch ((table->labels[i])->label_type)
                {
                case FLOAT:
                    printf("%10.1f\t", ((table->columns)[i]).f[j]);
                    break;

                case STRING:
                    printf("%10.10s\t", ((table->columns)[i]).s[j]);
                    break;
                }
            }

            printf("\n");
        }

        if (table->element_count > 10)
        {
            printf("\n");
            for (int i = labels_str; i <= labels_end; i++)
            {
                printf("%10.10s\t", ". . .");
            }

            printf("\n\n");

            // Show last element of DataTable_t
            for (int i = labels_str; i <= labels_end; i++)
            {
                switch ((table->labels[i])->label_type)
                {
                case FLOAT:
                    printf("%10.1f\t", ((table->columns)[i]).f[table->element_count - 1]);
                    break;

                case STRING:
                    printf("%10.10s\t", ((table->columns)[i]).s[table->element_count - 1]);
                    break;
                }
            }

            printf("\n");
        }
    }

    printf("\n");
}

DataTable_t *projectDT(DataTable_t *table, int m, int n, int x, int y)
{
    if (table == NULL || table->loaded == 0)
    {
        printf("|| Invalid table\n");
        return NULL;
    }

    /* m < table->labels_str || n > table->labels_end --> accomodate for */
    /* DataTable_t's on which projectDT() has already been performed         */
    if (m < table->labels_str || n > table->labels_end || m > n ||
        x < 0 || y > table->element_count - 1 || x > y)
    {
        printf("|| Invalid input\n");
        return NULL;
    }

    // Temporary labels to copy from the original DataTable_t into the new DataTable_t
    label_t *labels;
    labels = malloc(table->label_count * sizeof(*(labels)));

    for (int i = 0; i < table->label_count; i++)
    {
        labels[i] = *(table->labels[i]);
    }

    DataTable_t *new_table;
    new_table = initDT(labels, table->label_count);

    // New DataTable_t made usable in other functions despite not all columns containing values
    new_table->labels_str = m;
    new_table->labels_end = n;

    new_table->element_count = y - x + 1;

    // calloc() exact amount of memory required based on number of elements, y - x + 1
    for (int i = 0; i < new_table->label_count; i++)
    {
        switch ((new_table->labels[i])->label_type)
        {
        case FLOAT:
            (new_table->columns[i]).f = calloc(y - x + 1, sizeof(*((new_table->columns[i]).f)));
            if ((new_table->columns[i]).f == NULL)
            {
                printf("|| Error assigning memory\n");
                exit(EXIT_FAILURE);
            }
            break;

        case STRING:
            (new_table->columns[i]).s = calloc(y - x + 1, sizeof(*((new_table->columns[i]).s)));
            if ((new_table->columns[i]).s == NULL)
            {
                printf("|| Error assigning memory\n");
                exit(EXIT_FAILURE);
            }
            
            for (int s_i = 0; s_i < y - x + 1; s_i++)
            {
                // malloc() string in new DataTable_t according to size of corresponding string in original DataTable_t
                (new_table->columns[i]).s[s_i] = malloc(sizeof(table->columns[i]).s[s_i]);
            
                if ((new_table->columns[i]).s[s_i] == NULL)
                {
                    printf("|| Error assigning memory\n");
                    exit(EXIT_FAILURE);
                }
            }

            break;
        }
    }

    // Copy values from original DataTable_t into new DataTable_t
    for (int j = x, new_j = 0; j <= y; j++, new_j++)
    {
        for (int i = new_table->labels_str; i <= new_table->labels_end; i++)
        {
            switch ((table->labels[i])->label_type)
            {
            case FLOAT:
                (new_table->columns[i]).f[new_j] = (table->columns[i]).f[j];
                break;

            case STRING:
                strcpy((new_table->columns[i]).s[new_j], (table->columns[i]).s[j]);
                break;
            }
        }
    }

    // free() temporary labels
    free(labels);
    labels = NULL;

    new_table->loaded = 1;

    return new_table;
}

void mutateDT(void (*f)(column *my_column, TYPE t, int element_count), DataTable_t *table, int col)
{
    if (table == NULL || table->loaded == 0)
    {
        printf("|| Invalid table\n");
        return;
    }

    // Accomodate for projectDT()
    if (!(col >= table->labels_str && col <= table->labels_end))
    {
        printf("|| Invalid column\n");
        return;
    }

    /* Pass column TYPE so user-defined function can carry out different */
    /* operations based on whether column is of TYPE FLOAT or STRING     */
    switch ((table->labels[col])->label_type)
    {
        case FLOAT:
            for (int i = 0; i < table->element_count; i++)
            {
                (*f)(&(table->columns)[col], FLOAT, table->element_count);
            }
            
            break;

        case STRING:
            (*f)(&(table->columns)[col], STRING, table->element_count);

            break;
    }
}
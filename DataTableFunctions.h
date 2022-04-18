/* DataTable header file */


/* ----------------------------------- CONSTANTS ----------------------------------- */

#define CHUNK 50
#define CAT_BUF 5 // for mutateDT()


/* -------------------------------- TYPE DEFINITION -------------------------------- */

/* Column type definitions */
typedef double float_t;
typedef char *string_t;

/* Enumeration of column types so as to be usable in switch cases */
typedef enum
{
    FLOAT,
    STRING
} TYPE;

/* label (column name) struct */
typedef struct
{
    char *label_name; // for showDT()
    TYPE label_type;  // for switch cases, enumerated above
} label_t;

/* Column union for both doubles and strings columns to be assignable to the table */
typedef union
{
    float_t *f;
    string_t *s;
} column;

/* DataTable_t struct */
typedef struct
{
    /* Metadata */
    int loaded;
    label_t **labels; // Individual label_t's are expandable through additional memory allocation
    int labels_str;     // Accomodating for projectDT()
    int labels_end;     // Accomodating for projectDT()

    int label_count;
    int element_count;

    /* Data */
    column *columns;
} DataTable_t;


/* ------------------------------ FUNCTION PROTOTYPES ------------------------------ */

/* operation:       initialise a DataTable_t.                                        */
/* preconditions:   a label_t array with the required label names and types      */
/*                  is passed as a parameter, along with the label count.          */
/* postconditions:  the DataTable_t pointed to is assigned minimal memory            */
/*                  to be worked on by loadDT().                                     */
DataTable_t *initDT(label_t *labels, int label_count);


/* operation:       deinitialise a DataTable_t.                                      */
/* preconditions:   the address of a table pointer is passed as a parameter.         */
/* postconditions:  the DataTable_t pointed to by the pointer, its address passed    */
/*                  as a parameter, has its pointer attributes as well as the        */
/*                  table pointer itself freed and set to NULL so that their         */
/*                  previously assigned memory locations may be reused by malloc().  */
void deinitDT(DataTable_t **table);


/* operation:       load a .csv file into the DataTable_t pointed to by paramter     */
/* preconditions    the path to a valid .csv file is passed, as well as a pointer    */
/*                  to a DataTable_t on which initDT(), passed parameters            */
/*                  corresponding to the .csv, has been performed.                   */
/*                  The .csv must be delimited by semicolons (;) and column          */
/*                  elements must correctly correspond with the provided             */
/*                  label types.                                                   */
/* postconditions:  the DataTable_t pointed to by the pointer passed as a parameter  */
/*                  has its attributes allocated memory and populated according      */
/*                  to the provided .csv.                                            */
void loadDT(char path[], DataTable_t *table);


/* operation:       export the contents of a DataTable_t to a .csv file              */
/* preconditions:   a pointer to a valid DataTable_t on which initDT() and loadDT(). */
/*                  have been performed, as well as a valid .csv destination path    */
/*                  relative to the executable, are passed as parameters.            */
/* postconditions:  a .csv file is created at the provided destination.              */
void exportDT(DataTable_t *table, char path[]);


/* operation:       summarised display of the contents of a DataTable_t              */
/* preconditions:   a pointer to a valid DataTable_t on which initDT() and loadDT()  */
/*                  have been performed is passed as a parameter                     */
/* postconditions:  if the provided DataTable_t has more than 10 rows, the           */
/*                  first 10 rows as well as the last row are display in             */
/*                  stdout, else all its rows are displayed                          */
void showDT(DataTable_t *table);


/* operation:       projects part of/the entirety of a DataTable_t onto another      */
/*                  DataTable_t.                                                     */
/* preconditions:   a pointer to a valid DataTable_t on which initDT() and loadDT()  */
/*                  have been performed, as well as the column start and end index   */
/*                  and the row start and end index, m, n, x and y respectively and  */
/*                  all within the DataTable_t's range and in valid order, are       */
/*                  passed as a parameters                                           */
/* postconditions:  a pointer to the projected DataTable_t is returned, this         */
/*                  DataTable_t having labels_str and labels_end assigned        */
/*                  according to the values of m and n                               */
DataTable_t *projectDT(DataTable_t *table, int m, int n, int x, int y);


/* operation:       modifies a DataTable_t column according to a user-defined        */
/*                  function                                                         */
/*                  DataTable_t.                                                     */
/* preconditions:   the user-defined function must act within the allocated memory   */
/*                  buffer, CAT_BUF, if concatenating strings. This function, along  */
/*                  with a pointer to a DataTable_t on which initDT() and loadDT()   */
/*                  have been performed as well as a valid column number are passed  */
/*                  as parameters                                                    */
/*                  passed as a parameters                                           */
/* postconditions:  the specified DataTable_t column's element values are modified   */
void mutateDT(void (*f)(column *my_column, TYPE t, int element_count), DataTable_t *table, int col);

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#define NUM_CHILD_THREADS 27
#define SIZE 9
int tid_column[9];
int tid_row[9];
int tid_subGrid[9];

int sudukoPuzzle[SIZE][SIZE];
void *checkRow(void *);
void *gridCheck(void *);
void *checkCol(void *);

bool rowBooleans[SIZE];
bool colBooleans[SIZE];vi
bool gridBooleans[SIZE];

typedef struct{
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
    int thrID;
} validator;



int main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(stderr, "ERROR: This Program requires an [INPUT-FILE].txt\n");
    }

    int i, j, t =0, flag[9] = {0}, check;
    pthread_t tid[NUM_CHILD_THREADS];
    pthread_attr_t attr[NUM_CHILD_THREADS];
    validator *thrParam[NUM_CHILD_THREADS];
    FILE *puzzleText;
    puzzleText = fopen(argv[1], "r");


    /* Reads text file into a 2D array, Currently only accepts ints */
    if (puzzleText == NULL) {
        fprintf(stderr, "CANNOT OPEN THE FILE! \n");
        exit(1);
    } else {
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                fscanf(puzzleText, "%d", &sudukoPuzzle[i][j]);
            }
        }
    }


    /*prints the array 2D array out in a grid */
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            printf("%d\t", sudukoPuzzle[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    for (i = 0; i < NUM_CHILD_THREADS; i++) {
        pthread_attr_init(&(attr[i]));
    }

    for (i = 0; i < 9; i += 3) {
        for (j = 0; j < 9; j += 3, t++) {
            thrParam[t] = (validator *) malloc(sizeof(validator));
            thrParam[t]->topRow = i;
            thrParam[t]->bottomRow = i + 2;
            thrParam[t]->leftColumn = j;
            thrParam[t]->rightColumn = j + 2;
            thrParam[t]->thrID = t;
        }
    }

    for (i = 9; i < 18; i++) {
        thrParam[i] = (validator *) malloc(sizeof(validator));
        thrParam[i]->topRow = 0;
        thrParam[i]->bottomRow = 8;
        thrParam[i]->leftColumn = i - 9;
        thrParam[i]->rightColumn = i - 9;
    }

    for (i = 18; i < 27; i++) {
        thrParam[i] = (validator *) malloc(sizeof(validator));
        thrParam[i]->topRow = i - 18;
        thrParam[i]->bottomRow = i - 18;
        thrParam[i]->leftColumn = 0;
        thrParam[i]->rightColumn = 8;
    }

    for (t = 0; t < 9; t++) {
        pthread_create(&tid[t], &(attr[t]), gridCheck, (void *)thrParam[t]);
    }

    for (i = 9; i < 18; i++) {
        pthread_create(&tid[i], &(attr[i]), checkCol, (void *)thrParam[i]);
    }

    for (i = 18; i < 27; i++) {
        pthread_create(&tid[i], &(attr[i]), checkRow, (void *)thrParam[i]);
    }


    for (i = 0; i < NUM_CHILD_THREADS; i++) {
        pthread_join(tid[i],NULL);
    }

    for (i = 0; i < 9; i++) {
        printf("[%d]   Grid: <0x%08X>\t", i, tid_subGrid[i]);
        if (gridBooleans[i] == 1) {
            printf("<VALID!>\n");
        } else {
            printf("<INVALID!>\n");
        }
    }

    for (i = 0; i < 9; i++) {
        printf("[%d] Column: <0x%08X>\t", i, tid_column[i]);
        if (colBooleans[i] == 1) {
            printf("<VALID!>\n");
        } else {
            printf("<INVALID!>\n");
        }
    }

    for (i = 0; i < 9; i++) {
        printf("[%d]    Row: <0x%08X>\t", i, tid_row[i]);
        if (rowBooleans[i] == 1) {
            printf("<VALID!>\n");
        } else {
            printf("<INVALID!>\n");
        }
    }


    for (i = 0; i < 9; i++) {
        if (rowBooleans[i] & colBooleans[i] & gridBooleans[i] == true) {
            flag[i] = 1;
        } else {
            flag[i] = 0;
        }
    }

    for( i = 0; i < 9; i++) {
        if (flag[i]++) {
            check = 1;
        } else {
            check = 0;
            break;

        }
    }
    if (check == 1){
        printf("****************************************\n");
        printf("*****  SUDOKU PUZZLE:   <VALID!>   *****\n");
        printf("****************************************\n");
    }
    else{
        printf("****************************************\n");
        printf("*****  SUDUKO PUZZLE:  <INVALID!>  *****\n");
        printf("****************************************\n");
    }
}



void *gridCheck(void *data) {
    int digits[10] = {0};
    int i,j;
    validator *param = (validator *) data;
    int T_row = param->topRow;
    int B_row = param->bottomRow;
    int L_col;
    int R_col = param->rightColumn;
    int thr_ID = param->thrID;
    tid_subGrid[thr_ID] = (unsigned long)pthread_self();
    for (i = T_row; i <= B_row; i++) {
        L_col = param->leftColumn;
        for (j = L_col; j <= R_col; j++) {
            if (digits[sudukoPuzzle[i][j]]++ || sudukoPuzzle[i][j] > 9 || sudukoPuzzle[i][j] == 0) {
                printf("GRD_TID [ 0x%08X ] \t TRow: <%d>, \t BRow: <%d>, \t LCol: <%d>, \t RCol: <%d>, \t [INVALID!]\n", tid_subGrid[thr_ID], T_row, B_row, L_col, R_col);
                gridBooleans[thr_ID] = false;
                pthread_exit(0);
            }
        }
    }
    gridBooleans[thr_ID] = true;
    printf("GRD_TID [ 0x%08X ] \t TRow: <%d>, \t BRow: <%d>, \t LCol: <%d>, \t RCol: <%d>, \t [VALID!]\n", tid_subGrid[thr_ID], T_row, B_row, L_col, R_col);
    pthread_exit(0);
}


void *checkRow(void *data) {
    int digit[10] = {0};\
    int i;
    validator *param = (validator *) data;
    int T_row = param->topRow;
    int R_col = param->rightColumn;
    int L_col = param->leftColumn;
    int B_row = param->bottomRow;
    tid_row[T_row] = (unsigned long)pthread_self();
    for (i = L_col; i <= R_col; i++) {
        if(digit[sudukoPuzzle[T_row][i]]++ || sudukoPuzzle[T_row][i] == 0 || sudukoPuzzle[T_row][i] > 9){
            printf("ROW_TID [ 0x%08X ] \t TRow: <%d>, \t BRow: <%d>, \t LCol: <%d>, \t RCol: <%d>, \t [INVALID!]\n", tid_row[T_row], T_row, B_row, L_col, R_col);
            rowBooleans[T_row] = false;
            pthread_exit(0);
        }
    }
    printf("ROW_TID [ 0x%08X ] \t TRow: <%d>, \t BRow: <%d>, \t LCol: <%d>, \t RCol: <%d>, \t [VALID!]\n", tid_row[T_row], T_row, B_row, L_col, R_col);
    rowBooleans[T_row] = true;
    pthread_exit(0);
}



void *checkCol(void *data) {
    int digit[10];
    int i;
    validator *param = (validator *) data;
    int T_row = param->topRow;
    int B_row = param->bottomRow;
    int L_col = param->leftColumn;
    int R_col = param->rightColumn;
    tid_column[L_col] = (unsigned long) pthread_self();
    for (i = T_row; i <= B_row; i++) {
        if (digit[sudukoPuzzle[i][L_col]]++ || sudukoPuzzle[i][L_col] == 0 || sudukoPuzzle[i][L_col] > 9) {
            printf("COL_TID [ 0x%08X ] \t TRow: <%d>, \t BRow: <%d>, \t LCol: <%d>, \t RCol: <%d>, \t [INVALID!]\n", tid_column[L_col], T_row, B_row, L_col, R_col);
            colBooleans[L_col] = false;
            pthread_exit(0);
        }
    }
    printf("COL_TID [ 0x%08X ] \t TRow: <%d>, \t BRow: <%d>, \t LCol: <%d>, \t RCol: <%d>, \t [VALID!]\n", tid_column[L_col], T_row, B_row, L_col, R_col);
    colBooleans[L_col] = true;
    pthread_exit(0);
}

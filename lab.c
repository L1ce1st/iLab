#include<stdio.h>
#include <stdlib.h>
#include <math.h>

//----------------------------------------------------------------------------------------------------------------------
//! Program gets at entry .txt file with 2 columns: voltage and amperage. It returns .txt file with column resistance
//! and .txt file with out data
//!     ReadData_ERROR [code 100] - if the reading file could not be opened
//!    AnswerOut_ERROR [code 101] - if the entry data is 0 or one of the measures is bigger than limit value(const Max)
//! CountStrings_ERROR [code 102] - if the reading file is empty
// ---------------------------------------------------------------------------------------------------------------------
enum {
    ReadData_ERROR = 100, AnswerOut_ERROR, CountStrings_ERROR
};

double Sigma(double *R, double sum, int index, int measure);

double CalcData(double U, double I);

int ReadData(double *U, double *I, FILE *f1, int nStrings);

int CountStrings(FILE *);

int AnswerOut(FILE *f2, int nStrings, double *R, double *I, double *U);

void Resist(double *R, int nStrings, FILE *f3);

const int Max = 1000;

int main() {

    FILE *Data = NULL;
    FILE *Result = NULL;
    FILE *Laba = NULL;


    int nStrings = CountStrings(Data);
    if (nStrings == 0) {

        return CountStrings_ERROR;
    }
    double *R = (double *) calloc(nStrings, sizeof(double));
    double *I = (double *) calloc(nStrings, sizeof(double));
    double *U = (double *) calloc(nStrings, sizeof(double));

    int check = ReadData(U, I, Data, nStrings);
    if (check != 0) {
        printf("Cannot open file with data");

        return ReadData_ERROR;
    }
    check = AnswerOut(Result, nStrings, R, I, U);
    if (check != 0) {
        printf("Invalid data");

        return AnswerOut_ERROR;
    }
    Resist(R, nStrings, Laba);

    return 0;
}


int ReadData(double *U, double *I, FILE *f1, int nStrings) {

    f1 = fopen("Data.txt", "r");
    if (!f1) {

        return 1;
    }
    for (int i = 0; i <= nStrings; i++) {
        fscanf(f1, "%lg %lg\n", &U[i], &I[i]);
    }
    fclose(f1);

    return 0;
}

double CalcData(double U, double I) {

    double resistance = U / I;

    return resistance;
}

int CountStrings(FILE *f1) {

    f1 = fopen("Data.txt", "r");
    if (!f1){

        return 1;
        }
    int nStrings = 0;
    int letter = 0;
    for (;;) {
        letter = getc(f1);
        if (letter == EOF) {
            nStrings = nStrings - 1;
            break;
        }
        if (letter == '\n') {
            nStrings++;
        }
    }
    fclose(f1);

    return nStrings;
}

int AnswerOut(FILE *f2, int nStrings, double *R, double *I, double *U) {

    f2 = fopen("Result.txt", "w");
    for (int i = 0; i <= nStrings; i++) {
        if ((U[i] == 0) || (I[i] == 0) || (I[i] > Max) || (U[i] > Max)) {

            return 1;
        }
        R[i] = CalcData(U[i], I[i]);
        fprintf(f2, "%lg\n", R[i]);
    }
    fclose(f2);

    return 0;
}

void Resist(double *R, int nStrings, FILE *f3) {
    f3 = fopen("Laba.txt", "w");
    int index = 0;
    int measure = 0;
    double sum = 0;
    double pre = R[0] - 1;
    for (int i = 0; i <= nStrings; i++) {

        if (R[i] - pre > 1) {
            index++;
            fprintf(f3, "Rm%d= %.3lg Sigma%d= %.3lg\n", index, sum / (double) measure, index,
                    Sigma(R, sum, i, measure));
            measure = 0;
            sum = 0;
        }

        if (i == nStrings) {
            fprintf(f3, "Rm%d= %.3lg Sigma%d= %.3lg\n", index + 1, sum / (double) measure, index + 1,
                    Sigma(R, sum, i, measure));
        }
        pre = R[i];
        sum = sum + R[i];
        measure++;
    }
    fclose(f3);
}

double Sigma(double *R, double sum, int i, int measure) {
    double Rm = sum / (double) measure;
    double squaresum = 0;
    int begin = 0;
    if (measure == i)
        begin = 0;
    else
        begin = measure;

    for (int j = i - begin - 1; j <= i - 1; j++) {
        squaresum = squaresum + (R[j] - Rm) * (R[j] - Rm);
    }
    double sigma = sqrt(1 / (double) (measure - 1) / (double) measure * squaresum);

    return sigma;
}

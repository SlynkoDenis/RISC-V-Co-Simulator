#define ROWS 5
#define COLUMNS 10
#define SIZE 50
#define RESULT_SIZE 25


typedef struct {
    int numberOfRows;
    int numberOfColumns;
    int *elements;
} Matrix;

void MultiplyMatrices(Matrix *A, Matrix *B, Matrix *result) {
    if (A->numberOfColumns != B->numberOfRows) {
        return;
    }

    for (int i = 0; i < result->numberOfRows; ++i) {
        for (int j = 0; j < result->numberOfColumns; ++j) {
            int elem = 0;
            for (int k = 0; k < B->numberOfRows; ++k) {
                elem += A->elements[i * A->numberOfColumns + k] * B->elements[k * B->numberOfColumns + j];
            }
            result->elements[i * result->numberOfColumns + j] = elem;
        }
    }
}

int VerifyMatricesMultiplication(Matrix *result, Matrix *correct) {
    if (result->numberOfRows != correct->numberOfRows || result->numberOfColumns != correct->numberOfColumns) {
        return -1;
    }

    for (int i = 0; i < result->numberOfRows; ++i) {
        for (int j = 0; j < result->numberOfColumns; ++j) {
            if (result->elements[i * result->numberOfColumns + j] != correct->elements[i * result->numberOfColumns + j]) {
                return -1;
            }
        }
    }
    return 0;
}


int main() {
    int elements_a[SIZE] = {98, 32, 17, -58, 23, 57, -57, 36, 51, -81, 13, 69,
        42, -6, -85, 92, 35, -47, 71, -57, 82, -40, -47, -68, -100, 70, -84,
        99, -90, 13, 58, 19, -96, 39, 70, 42, -79, 13, 31, 73, 66, -79, 49,
        -70, -13, -99, -24, 55, -58, -86};
    Matrix matrix_a = {ROWS, COLUMNS, elements_a};
    int elements_b[SIZE] = {-15, -10, -47, 59, 2, -4, -39, -6, 34, -28, -90,
        -74, 53, 43, -75, 63, -89, 13, -12, 92, -71, -68, -81, -88, -13,
        38, -99, 80, 29, -76, -42, 61, -2, -58, 79, 92, -7, 74, -99, -90,
        39, -30, 25, 20, -68, -88, 17, 40, -92, -36};
    Matrix matrix_b = {COLUMNS, ROWS, elements_b};
    int elements_result[RESULT_SIZE] = {0};
    Matrix matrix_result = {ROWS, ROWS, elements_result};

    MultiplyMatrices(&matrix_a, &matrix_b, &matrix_result);

    int correct_elements[RESULT_SIZE] = {8574, -12167, -1141, 16140, -20237, 6893, -9358,
        11315, 24426, -7276, 16618, 7244, 12475, 5178, -15361, 6076, -11205, -4916,
        -8271, -5877, -959, 14139, -8580, 5127, 116};
    Matrix matrix_correct = {ROWS, ROWS, correct_elements};
    return VerifyMatricesMultiplication(&matrix_result, &matrix_correct);
}

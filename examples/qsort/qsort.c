#define N 100


void Swap(int *lhs, int *rhs) {
    int tmp = *lhs;
    *lhs = *rhs;
    *rhs = tmp;
}

void InsertionSort(int *first, int *last) {
    if (last - first < 2) {
        return;
    }

    for (int i = 1, end_index = last - first; i < end_index; ++i) {
        for (int j = i; j > 0; --j) {
            if (*(first + j) < *(first + j - 1)) {
                Swap((first + j), (first + j - 1));
            } else {
                break;
            }
        }
    }
}

int Median(int first, int second, int third) {
    if (first <= second) {
        return second <= third ? second : third;
    } else {
        return first <= third ? first : third;
    }
}

int *Partition(int *first, int *last, int pivot) {
    if (last - first < 2) {
        return first;
    }

    --last;
    while (last - first > 0) {
        if (*first < pivot && *last > pivot) {
            ++first;
            --last;
        } else if (*first < pivot) {
            ++first;
        } else if (*last > pivot) {
            --last;
        } else if (*first == pivot && *last != pivot) {
            Swap(first, last);
            ++first;
        } else if (*last == pivot && *first != pivot) {
            Swap(first, last);
            --last;
        } else if (*first == *last && *first == pivot) {
            int *ptr = first + 1;
            while (ptr != last && *ptr <= *first) {
                if (*ptr != *first) {
                    Swap(first, ptr);
                }
                ++first;
                ++ptr;
            }
            if (ptr == last) {
                return first;
            } else {
                ++first;
            }
        } else {
            Swap(first, last);
            ++first;
            --last;
        }
    }

    return first;
}

void QSort(int *first, int *last) {
    if (last - first <= 16) {
        return InsertionSort(first, last);
    }

    int pivot = Median(*first, *(last - 1), *(first + (last - first) / 2));
    int *mid = Partition(first, last, pivot);

    if (mid - first < last - mid) {
        QSort(first, mid);
        QSort(mid, last);
    } else {
        QSort(mid, last);
        QSort(first, mid);
    }
}

int VerifySort(int *first, int size) {
    for (int i = 1; i < size; ++i) {
        if (*(first + i) < *(first + i - 1)) {
            return -1;
        }
    }
    return 0;
}

int main() {
    int data[N] = {
        50, 96, -64, -74, -8, 34, -33, -89, 54, -25, 38,
        -70, -44, -44, -88, -81, -25, 50, -27, -3, -40,
        55, -65, 92, 22, 42, -57, 48, -70, 25, -65, -86,
        -92, 40, 27, -26, 49, -88, -75, -22, 26, 49, -14,
        -41, -49, 95, -94, -36, -81, -42, 50, -88, -77,
        -33, -68, 50, -85, 56, 30, -36, -28, 67, -50, -46,
        -83, 35, -16, -5, -60, 85, 96, -76, 39, 13, 17, 43,
        -38, -22, -42, -30, 43, 51, -79, -52, -38, -35, -67,
        54, 19, -60, 20, 44, 45, -78, 30, -9, -90, -12, 37, -37};
    QSort(data, data + N);
    return VerifySort(data, N); // must return 0
}

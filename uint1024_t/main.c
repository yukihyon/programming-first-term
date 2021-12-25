#include <stdio.h>
#include <stdlib.h>

struct numberInChars {
    unsigned char * array;
    int size;
};

struct numberInChars sum_chars(struct numberInChars x, struct numberInChars y) {
    struct numberInChars result;
    result.array = (char *) malloc(1024);
    result.size = x.size > y.size ? x.size : y.size;
    for (int i = 0; i < 1024; i++) {
        result.array[i] = 0;
    }
    int firstPointer = x.size - 1;
    int secondPointer = y.size - 1;
    int firstElement;
    int secondElement;
    int next = 0;
    for (int i = result.size - 1; i >= 0; i--) {
        firstElement = firstPointer < 0 ? 0 : x.array[firstPointer];
        secondElement = secondPointer < 0 ? 0 : y.array[secondPointer];
        result.array[i] = ((firstElement + secondElement) + next) % 10;
        next = (firstElement + secondElement + next) / 10;
        firstPointer--;
        secondPointer--;
    }
    if (next != 0) {
        for (int i = result.size; i >= 1; i--) {
            result.array[i] = result.array[i - 1];
        }
        result.array[0] = next;
        result.size += 1;
    }
    result.array = (char *) realloc(result.array, result.size);
    return result;
}

struct numberInChars mult_chars(struct numberInChars current, struct numberInChars base) {
    struct numberInChars multResult;
    multResult.array = (char *) malloc(1024);
    multResult.size = 0;
    struct numberInChars temporary;
    int move = 0;
    int next;
    for (int i = 2; i >= 0; i--) {
        temporary.array = (char *) malloc(1024);
        temporary.size = 0;
        next = 0;
        int firstElement;
        int secondElement;
        for (int j = current.size - 1; j >= 0; j--) {
            firstElement = current.array[j];
            secondElement = base.array[i];
            for (int s = temporary.size; s > 0; s--) {
                temporary.array[s] = temporary.array[s - 1];
            }
            temporary.array[0] = ((firstElement * secondElement) + next) % 10;
            next = ((firstElement * secondElement) + next) / 10;
            temporary.size += 1;
        }
        if (next != 0) {
            for (int s = temporary.size; s > 0; s--) {
                temporary.array[s] = temporary.array[s - 1];
            }
            temporary.array[0] = next;
            temporary.size += 1;
        }
        if (move > 0) {
            for (int j = 0; j < move; j++) {
                temporary.array[temporary.size + j] = 0;
            }
            temporary.size += move;
        }
        temporary.array = (char *) realloc(temporary.array, temporary.size);
        multResult = sum_chars(multResult, temporary);
        move += 1;
        free(temporary.array);
    }
    return multResult;
}

struct uint1024_t {
    unsigned char * array;
    int size;
};

struct uint1024_t from_uint(unsigned int x) {
    struct uint1024_t result;
    result.array = (unsigned char *) malloc(128);
    result.size = 0;
    while (x > 256) {
        for (int i = result.size; i > 0; i--) {
            result.array[i] = result.array[i - 1];
        }
        result.array[0] = x % 256;
        result.size += 1;
        x = x / 256;
    }
    for (int i = result.size; i > 0; i--) {
        result.array[i] = result.array[i - 1];
    }
    result.array[0] = x;
    result.size += 1;
    result.array = (unsigned char *) realloc(result.array, result.size);
    return result;
}

struct uint1024_t add_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t result;
    result.size = x.size > y.size ? x.size : y.size;
    result.array = (unsigned char *) malloc(result.size);
    int firstPointer = x.size - 1;
    int secondPointer = y.size - 1;
    unsigned char firstElement;
    unsigned char secondElement;
    int next = 0;
    for (int i = result.size - 1; i >= 0; i--) {
        firstElement = firstPointer < 0 ? 0 : x.array[firstPointer];
        secondElement = secondPointer < 0 ? 0 : y.array[secondPointer];
        result.array[i] = (firstElement + secondElement + next) % 256;
        next = (firstElement + secondElement + next) / 256;
        firstPointer--;
        secondPointer--;
    }
    if (next != 0) {
        result.array = (unsigned char *) realloc(result.array, result.size + 1);
        for (int i = result.size; i > 0; i--) {
            result.array[i] = result.array[i - 1];
        }
        result.array[0] = next;
        result.size += 1;
    }
    return result;
}

struct uint1024_t subtr_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t result;
    result.size = x.size > y.size ? x.size : y.size;
    result.array = (unsigned char *) malloc(result.size);
    int firstPointer = x.size - 1;
    int secondPointer = y.size - 1;
    unsigned char firstElement;
    unsigned char secondElement;
    int previous = 0;
    for (int i = result.size - 1; i >= 0; i--) {
        firstElement = firstPointer < 0 ? 0 : x.array[firstPointer];
        secondElement = secondPointer < 0 ? 0 : y.array[secondPointer];
        if (firstElement > secondElement) {
            result.array[i] = firstElement - secondElement - previous;
            previous = 0;
        } else {
            result.array[i] = (firstElement + 256) - secondElement;
            previous -= 1;
        }
        firstPointer--;
        secondPointer--;
    }
    if (previous != 0) {
        for (int i = 0; i < result.size; i++) {
            result.array[i] = result.array[i + 1];
        }
        result.array = (unsigned char *) realloc(result.array, result.size - 1);
        result.size -= 1;
    }
    return result;
}

struct uint1024_t mult_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t result;
    result.array = (unsigned char *) malloc(128);
    result.size = 0;
    struct uint1024_t temporary;
    int move = 0;
    int next;
    for (int i = y.size - 1; i >= 0; i--) {
        temporary.array = (unsigned char *) malloc(128);
        temporary.size = 0;
        next = 0;
        for (int j = x.size - 1; j >= 0; j--) {
            for (int k = temporary.size; k > 0; k--) {
                temporary.array[k] = temporary.array[k - 1];
            }
            temporary.array[0] = ((x.array[j] * y.array[i]) + next) % 256;
            next = ((x.array[j] * y.array[i]) + next) / 256;
            temporary.size += 1;
        }
        if (next != 0) {
            for (int i = temporary.size; i > 0; i--) {
                temporary.array[i] = temporary.array[i - 1];
            }
            temporary.array[0] = next;
            temporary.size += 1;
        }
        if (move > 0) {
            for (int j = 0; j < move; j++) {
                temporary.array[temporary.size + j] = 0;
            }
            temporary.size += move;
        }
        temporary.array = (unsigned char *) realloc(temporary.array, temporary.size);
        result = add_op(result, temporary);
        move += 1;
        free(temporary.array);
    }
    return result;
}

void scanf_value(struct uint1024_t* x) {
    x -> array = (unsigned char *) malloc(128);
    char * number = (char *) malloc(1024);
    int sizeOfNumber = 0;
    int character;
    while ((character = getchar()) != '\n') {
        number[sizeOfNumber] = character;
        sizeOfNumber++;
    }
    int digit = 0;
    x -> size = 0;
    while (1) {
        int dividend = 0;
        unsigned char * result = (unsigned char *) malloc(1024);
        int sizeOfResult = 0;
        for (int pointer = 0; pointer < sizeOfNumber; pointer++) {
            dividend = dividend * 10 + (number[pointer] - '0');
            if ((dividend >= 256) || (sizeOfResult != 0)) {
                result[sizeOfResult] = (dividend / 256) + '0';
                dividend = dividend % 256;
                sizeOfResult++;
            }
        }
        for (int i = x -> size; i > 0; i--) {
            x -> array[i] = x -> array[i - 1];
        }
        x -> array[0] = dividend;
        x -> size += 1;
        if (sizeOfResult == 0){
            free(result);
            break;
        }
        for (int i = 0; i < 1024; i++) {
            number[i] = result[i];
        }
        sizeOfNumber = sizeOfResult;
        digit++;
    }
    free(number);
    x -> array = (unsigned char *) realloc(x -> array, x -> size);
}

void printf_value(struct uint1024_t x) {
    struct numberInChars result;
    result.array = (unsigned char *) malloc(1024);
    result.size = 0;
    struct numberInChars current;
    struct numberInChars base;
    base.array = (unsigned char *) malloc(3);
    base.size = 3;
    base.array[0] = 2;
    base.array[1] = 5;
    base.array[2] = 6;
    int pow;
    int startNumber;
    for (int k = 0; k < x.size; k++) {
        startNumber = x.array[k];
        if (startNumber < 10) {
            current.size = 1;
            current.array = (unsigned char *) malloc(current.size);
            current.array[0] = startNumber;
        } else if ((startNumber >= 10) && (startNumber < 100)) {
            current.size = 2;
            current.array = (unsigned char *) malloc(current.size);
            current.array[0] = startNumber / 10;
            current.array[1] = startNumber % 10;
        } else {
            current.size = 3;
            current.array = (unsigned char *) malloc(current.size);
            for (int i = 2; i >= 0; i--) {
                current.array[i] = startNumber % 10;
                startNumber = startNumber / 10;
            }
        }
        pow = x.size - 1 - k;
        for (int j = 0; j < pow; j++) {
            current = mult_chars(current, base);
        }
        result = sum_chars(result, current);
        free(current.array);
    }
    for (int i = 0; i < result.size; i++) {
        printf("%d", result.array[i]);
    }
    printf("\n");
    free(result.array);
}

void printf_value_chars(struct uint1024_t x) {
    for (int i = 0; i < x.size; i++) {
        printf("%d ", x.array[i]);
    }
    printf("\n");
}

int main() {
    struct uint1024_t x;
    scanf_value(&x);
    printf_value(x);

    struct uint1024_t y;
    scanf_value(&y);
    printf_value(y);

    printf("sum: ");
    printf_value(add_op(x, y));
    printf("subtr: ");
    printf_value(subtr_op(x, y));
    printf("mult: ");
    printf_value(mult_op(x, y));

    struct uint1024_t z;
    printf("print uint number:");
    unsigned int number;
    scanf("%u", &number);
    printf_value((from_uint(number)));
}

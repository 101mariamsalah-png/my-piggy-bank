#include <stdio.h>
#include <stdint.h>

#define MSG_MAX 130U
#define ALPHABET 26U

static char message[MSG_MAX];
static char scratch[MSG_MAX];
static uint8_t shiftKey = 3;

/* --- Function Declarations --- */
static void readMessage(void);
static uint16_t textLength(const char *s);
static char shiftChar(char c, uint8_t key);
static void encode(void);
static void decode(void);
static void reverseText(char *s, uint16_t n);
static uint16_t countVowels(const char *s);
static uint16_t isPalindrome(const char *s);
static void letterHistogram(const char *s);
static void showMessage(void);

static void clearInput(void) {
    int character;
    while ((character = getchar()) != '\n' && character != EOF) {
    }
}

static int readInteger(int *value) {
    int result = scanf("%d", value);
    clearInput();
    return result;
}

static uint8_t isLetter(char c) {
    return (uint8_t)((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

static uint8_t isComparable(char c) {
    return (uint8_t)(isLetter(c) || (c >= '0' && c <= '9'));
}

static char lowerLetter(char c) {
    if (c >= 'A' && c <= 'Z') {
        c = (char)(c + ('a' - 'A'));
    }
    return c;
}

/* --- Function Implementations --- */

// 1. Read input safely without overflowing the array
static void readMessage(void) {
    int c;
    uint16_t i = 0;
    
    while ((c = getchar()) != '\n' && c != EOF) {
        if (i < MSG_MAX - 1) {
            message[i++] = (char)c;
        }
    }
    message[i] = '\0';
}

static uint16_t textLength(const char *s) {
    uint16_t len = 0U;
    while (len < UINT16_MAX && s[len] != '\0') {
        len++;
    }
    return len;
}

// 3. Shift a single character with wrap-around protection
static char shiftChar(char c, uint8_t key) {
    uint8_t normalizedKey = (uint8_t)(key % ALPHABET);

    if (c >= 'A' && c <= 'Z') {
        return (char)('A' + (c - 'A' + normalizedKey) % ALPHABET);
    } else if (c >= 'a' && c <= 'z') {
        return (char)('a' + (c - 'a' + normalizedKey) % ALPHABET);
    }

    return c;
}

// 4. Encode message using shiftKey
static void encode(void) {
    uint16_t len = textLength(message);
    for (uint16_t i = 0; i < len; i++) {
        scratch[i] = shiftChar(message[i], shiftKey);
    }
    scratch[len] = '\0';
}

// 5. Decode message by shifting forward by (26 - shiftKey)
static void decode(void) {
    uint8_t decodeKey = (ALPHABET - (shiftKey % ALPHABET)) % ALPHABET;
    uint16_t len = textLength(scratch);
    
    for (uint16_t i = 0; i < len; i++) {
        scratch[i] = shiftChar(scratch[i], decodeKey);
    }
    scratch[len] = '\0';
}

// 6. Reverse text in-place
static void reverseText(char *s, uint16_t n) {
    if (n == 0) return;
    
    uint16_t start = 0;
    uint16_t end = n - 1;
    
    while (start < end) {
        char temp = s[start];
        s[start] = s[end];
        s[end] = temp;
        start++;
        end--;
    }
}

static uint16_t countVowels(const char *s) {
    uint16_t count = 0U;
    uint16_t i;

    for (i = 0U; s[i] != '\0'; i++) {
        char c = lowerLetter(s[i]);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            count++;
        }
    }
    return count;
}

static uint16_t isPalindrome(const char *s) {
    uint16_t start = 0U;
    uint16_t end = textLength(s);

    while (start < end) {
        while (start < end && !isComparable(s[start])) {
            start++;
        }
        while (start < end && !isComparable(s[end - 1U])) {
            end--;
        }
        if (start < end && lowerLetter(s[start]) != lowerLetter(s[end - 1U])) {
            return 0U;
        }
        start++;
        if (end > 0U) {
            end--;
        }
    }
    return 1U;
}

static void letterHistogram(const char *s) {
    uint16_t counts[ALPHABET] = {0U};
    uint16_t i;

    for (i = 0U; s[i] != '\0'; i++) {
        char c = s[i];
        if (c >= 'a' && c <= 'z') {
            counts[c - 'a']++;
        } else if (c >= 'A' && c <= 'Z') {
            counts[c - 'A']++;
        }
    }
    for (i = 0U; i < ALPHABET; i++) {
        if (counts[i] > 0U) {
            uint16_t j;
            printf("%c: ", (char)('A' + i));
            for (j = 0U; j < counts[i]; j++) {
                putchar('*');
            }
            putchar('\n');
        }
    }
}

static void showMessage(void) {
    printf("\nMessage: %s\n", message);
    printf("Length: %u | Key: %u | Vowels: %u\n",
           textLength(message), shiftKey, countVowels(message));
}

int main(void) {
    int option = 0;

    printf("Enter secret message: ");
    readMessage();
    do {
        printf("\n===== SECRET MESSAGE MACHINE =====\n");
        printf("1. Show message\n2. Encode\n3. Decode scratch\n");
        printf("4. Reverse message\n5. Check palindrome\n6. Histogram\n0. Exit\n");
        printf("Choose an option: ");
        {
            int result = readInteger(&option);
            if (result == EOF) {
                printf("\nInput ended. Goodbye!\n");
                break;
            }
            if (result != 1) {
                printf("Please enter a number.\n");
                continue;
            }
        }
        switch (option) {
            case 1:
                showMessage();
                break;
            case 2:
                encode();
                printf("Encoded: %s\n", scratch);
                break;
            case 3:
                decode();
                printf("Decoded: %s\n", scratch);
                break;
            case 4:
                reverseText(message, textLength(message));
                printf("Reversed: %s\n", message);
                break;
            case 5:
                printf("Palindrome: %s\n", isPalindrome(message) ? "yes" : "no");
                break;
            case 6:
                letterHistogram(message);
                break;
            case 0:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid option.\n");
                break;
        }
    } while (option != 0);
    return 0;
}
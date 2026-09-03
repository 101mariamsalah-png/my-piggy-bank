
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STAT_MAX        10
#define LOW_WARNING     3
#define NAME_LEN        120

/* mood bits */
#define BIT_ASLEEP      0
#define BIT_HUNGRY      1
#define BIT_SAD         2
#define BIT_SICK        3

#define SET_BIT(reg, n)     ((reg) |= (uint8_t)(1U << (n)))
#define CLR_BIT(reg, n)     ((reg) &= (uint8_t)~(1U << (n)))
#define READ_BIT(reg, n)    (((reg) >> (n)) & 1U)

typedef struct {
    char name[NAME_LEN];
    uint8_t food;     /* 0..10 */
    uint8_t fun;      /* 0..10 */
    uint8_t energy;   /* 0..10 */
    uint8_t mood;     /* the bits above */
    uint16_t hours;   /* how long Fluffy has been alive */
} Pet_t;

static Pet_t fluffy;

/* Function Declarations */
static void hatchPet(void);
static void drawFace(void);
static void drawStat(const char *label, uint8_t value);
static void feed(void);
static void play(void);
static void sleepPet(void);
static void hourPasses(void);
static void updateMood(void);
static uint8_t isHappy(void);
static void petReport(void);

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

/* Function Implementations */

static void hatchPet(void) {
    strcpy(fluffy.name, "Fluffy");
    fluffy.food = STAT_MAX;
    fluffy.fun = STAT_MAX;
    fluffy.energy = STAT_MAX;
    CLR_BIT(fluffy.mood, BIT_ASLEEP);
    CLR_BIT(fluffy.mood, BIT_HUNGRY);
    CLR_BIT(fluffy.mood, BIT_SAD);
    CLR_BIT(fluffy.mood, BIT_SICK);
    fluffy.hours = 0;
}

static void drawFace(void) {
    printf("+---------+\n");
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("|  -   -  |\n");
        printf("|    z    |\n");
        printf("|   ---   |\n");
    } else if (READ_BIT(fluffy.mood, BIT_SICK)) {
        printf("|  x   x  |\n");
        printf("|    o    |\n");
        printf("|   ~~~   |\n");
    } else if (READ_BIT(fluffy.mood, BIT_SAD) || READ_BIT(fluffy.mood, BIT_HUNGRY)) {
        printf("|  o   o  |\n");
        printf("|    .    |\n");
        printf("|   (n)   |\n");
    } else {
        printf("|  o   o  |\n");
        printf("|    ^    |\n");
        printf("|  \\___/  |\n");
    }
    printf("+---------+\n");
}

static void drawStat(const char *label, uint8_t value) {
    printf("%-7s [", label);
    for (uint8_t i = 0; i < STAT_MAX; i++) {
        if (i < value) {
            printf("#");
        } else {
            printf(" ");
        }
    }
    printf("] %d/%d\n", value, STAT_MAX);
}

static void feed(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("Cannot feed a sleeping pet!\n");
        return;
    }
    if (fluffy.food == STAT_MAX) {
        SET_BIT(fluffy.mood, BIT_SICK);
    } else {
        fluffy.food += 3;
        if (fluffy.food > STAT_MAX) {
            fluffy.food = STAT_MAX;
        }
        updateMood();
    }
}

static void play(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("Cannot play with a sleeping pet!\n");
        return;
    }
    
    /* Increase Fun */
    fluffy.fun += 3;
    if (fluffy.fun > STAT_MAX) {
        fluffy.fun = STAT_MAX;
    }
    
    /* Decrease Energy safely */
    if (fluffy.energy >= 2) {
        fluffy.energy -= 2;
    } else {
        fluffy.energy = 0;
    }
}

static void sleepPet(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        CLR_BIT(fluffy.mood, BIT_ASLEEP);
    } else {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
    }
    CLR_BIT(fluffy.mood, BIT_SICK);
}

static void updateMood(void) {
    /* Low food means hungry */
    if (fluffy.food <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_HUNGRY);
    } else {
        CLR_BIT(fluffy.mood, BIT_HUNGRY);
    }

    /* Low fun means sad */
    if (fluffy.fun <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_SAD);
    } else {
        CLR_BIT(fluffy.mood, BIT_SAD);
    }

    /* No energy falls asleep automatically */
    if (fluffy.energy == 0) {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
    }
}

static void hourPasses(void) {
    if (fluffy.hours < UINT16_MAX) {
        fluffy.hours++;
    }

    /* Decrease Food safely */
    if (fluffy.food > 0) {
        fluffy.food--;
    }

    /* Decrease Fun safely */
    if (fluffy.fun > 0) {
        fluffy.fun--;
    }

    /* Energy logic based on sleep state */
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        fluffy.energy += 2;
        if (fluffy.energy > STAT_MAX) {
            fluffy.energy = STAT_MAX;
        }
    } else {
        if (fluffy.energy > 0) {
            fluffy.energy--;
        }
    }

    updateMood();
}

static uint8_t isHappy(void) {
    if (fluffy.food > LOW_WARNING && 
        fluffy.fun > LOW_WARNING && 
        fluffy.energy > LOW_WARNING && 
        !READ_BIT(fluffy.mood, BIT_ASLEEP) &&
        !READ_BIT(fluffy.mood, BIT_HUNGRY) && 
        !READ_BIT(fluffy.mood, BIT_SAD) && 
        !READ_BIT(fluffy.mood, BIT_SICK)) {
        return 1;
    }
    return 0;
}

static void petReport(void) {
    printf("=== CARE CARD ===\n");
    printf("Name: %s\n", fluffy.name);
    printf("Hours Alive: %u\n", fluffy.hours);
    
    drawStat("Food", fluffy.food);
    drawStat("Fun", fluffy.fun);
    drawStat("Energy", fluffy.energy);

    /* Print Mood byte in Binary */
    printf("Mood (Binary): ");
    for (int8_t i = 7; i >= 0; i--) {
        printf("%d", READ_BIT(fluffy.mood, i));
    }
    printf("\n");

    /* Status Sentence */
    printf("Status: ");
    if (READ_BIT(fluffy.mood, BIT_SICK)) {
        printf("Fluffy is sick and needs to sleep!\n");
    } else if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("Fluffy is sleeping...\n");
    } else if (READ_BIT(fluffy.mood, BIT_HUNGRY)) {
        printf("Fluffy needs food!\n");
    } else if (READ_BIT(fluffy.mood, BIT_SAD)) {
        printf("Fluffy wants to play!\n");
    } else {
        printf("Fluffy is doing great!\n");
    }
    printf("=================\n");
}

int main(void) {
    int option;

    hatchPet();
    do {
        printf("\n===== FLUFFY THE ROBOT PET =====\n");
        printf("1. Draw face\n");
        printf("2. Feed\n");
        printf("3. Play\n");
        printf("4. Sleep/wake\n");
        printf("5. Pass one hour\n");
        printf("6. Care report\n");
        printf("7. Check happiness\n");
        printf("0. Exit\n");
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
                drawFace();
                break;
            case 2:
                feed();
                break;
            case 3:
                play();
                break;
            case 4:
                sleepPet();
                break;
            case 5:
                hourPasses();
                break;
            case 6:
                petReport();
                break;
            case 7:
                printf("Fluffy is %s.\n", isHappy() ? "happy" : "not happy");
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
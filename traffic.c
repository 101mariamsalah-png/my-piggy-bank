#include <stdio.h>
#include <stdint.h>

#define TICKS_GREEN   5U
#define TICKS_YELLOW  2U
#define TICKS_RED     4U
#define QUEUE_BUSY    8U    /* more cars than this means "busy" */
#define LOG_LEN       20U

typedef enum { LIGHT_GREEN = 0, LIGHT_YELLOW, LIGHT_RED } LightState_t;

/* status bits */
#define BIT_NIGHT     0U
#define BIT_BUSY      1U
#define BIT_BLINK_ON  2U

#define SET_BIT(reg, n)    ((reg) |= (uint8_t)(1U << (n)))
#define CLR_BIT(reg, n)     ((reg) &= (uint8_t)~(1U << (n)))
#define TOGGLE_BIT(reg, n) ((reg) ^= (uint8_t)(1U << (n)))
#define READ_BIT(reg, n)   (((reg) >> (n)) & 1U)

static LightState_t light;
static uint8_t      status;      /* NIGHT / BUSY / BLINK_ON */
static uint8_t      ticksLeft;   /* time left in this colour */
static uint8_t      carsWaiting;
static uint32_t     carsPassed;
static char         logLine[LOG_LEN];
static uint32_t     totalTicks;

static void resetCrossing(void);
static uint8_t ticksFor(LightState_t s);
static LightState_t nextState(LightState_t s);
static void drawLight(void);
static void tick(void);
static void addCars(void);
static void toggleNight(void);
static void pushLog(char c);
static void showLog(void);
static void crossingReport(void);

static void clearInputBuffer(void) {
    int character;
    while ((character = getchar()) != '\n' && character != EOF) {
    }
}

/* Helper functions for log */
static void pushLog(char c) {
    for (uint8_t i = 0U; i < LOG_LEN - 1U; i++) {
        logLine[i] = logLine[i + 1];
    }
    logLine[LOG_LEN - 1] = c;
}

static void showLog(void) {
    printf("Log: ");
    for (uint8_t i = 0U; i < LOG_LEN; i++) {
        if (logLine[i] != '\0') {
            putchar(logLine[i]);
        }
    }
    printf("\n");
}

/* Core project functions */
static void resetCrossing(void) {
    light = LIGHT_RED;
    status = 0;
    carsWaiting = 0;
    carsPassed = 0;
    totalTicks = 0;
    ticksLeft = TICKS_RED;
    for (uint8_t i = 0U; i < LOG_LEN; i++) {
        logLine[i] = ' ';
    }
}

static uint8_t ticksFor(LightState_t s) {
    if (s == LIGHT_GREEN) {
        return READ_BIT(status, BIT_BUSY) ? (TICKS_GREEN + 2) : TICKS_GREEN;
    } else if (s == LIGHT_YELLOW) {
        return TICKS_YELLOW;
    } else {
        return TICKS_RED;
    }
}

static LightState_t nextState(LightState_t s) {
    if (s == LIGHT_GREEN) return LIGHT_YELLOW;
    if (s == LIGHT_YELLOW) return LIGHT_RED;
    return LIGHT_GREEN;
}

static void drawLight(void) {
    printf("\n--- TRAFFIC LIGHT ---\n");
    if (READ_BIT(status, BIT_NIGHT)) {
        printf("  ( ) Red\n");
        printf("  (%c) Yellow (BLINK)\n", READ_BIT(status, BIT_BLINK_ON) ? '*' : ' ');
        printf("  ( ) Green\n");
    } else {
        printf("  (%c) Red\n", (light == LIGHT_RED) ? 'O' : ' ');
        printf("  (%c) Yellow\n", (light == LIGHT_YELLOW) ? 'O' : ' ');
        printf("  (%c) Green\n", (light == LIGHT_GREEN) ? 'O' : ' ');
    }
    printf("Ticks Left: %d | Cars Waiting: %d\n", ticksLeft, carsWaiting);
    printf("---------------------\n");
}

static void tick(void) {
    totalTicks++;
    
    if (READ_BIT(status, BIT_NIGHT)) {
        TOGGLE_BIT(status, BIT_BLINK_ON);
        pushLog(READ_BIT(status, BIT_BLINK_ON) ? 'y' : ' ');
        return;
    }

    if (light == LIGHT_GREEN) {
        uint8_t passing = (carsWaiting >= 2) ? 2 : carsWaiting;
        carsWaiting -= passing;
        carsPassed += passing;
        
        if (carsWaiting <= QUEUE_BUSY) {
            CLR_BIT(status, BIT_BUSY);
        }
    }

    if (ticksLeft > 0) {
        ticksLeft--;
    }

    if (ticksLeft == 0) {
        light = nextState(light);
        ticksLeft = ticksFor(light);
    }

    if (light == LIGHT_GREEN) pushLog('G');
    else if (light == LIGHT_YELLOW) pushLog('Y');
    else pushLog('R');
}

static void addCars(void) {
    int input = 0;
    printf("Enter number of cars arriving: ");
    if (scanf("%d", &input) == 1 && input > 0) {
        carsWaiting += input;
        if (carsWaiting > QUEUE_BUSY) {
            SET_BIT(status, BIT_BUSY);
        }
    }
    clearInputBuffer();
}

static void toggleNight(void) {
    TOGGLE_BIT(status, BIT_NIGHT);
    if (READ_BIT(status, BIT_NIGHT)) {
        CLR_BIT(status, BIT_BLINK_ON);
    } else {
        light = LIGHT_RED;
        ticksLeft = ticksFor(LIGHT_RED);
    }
}

static void crossingReport(void) {
    printf("\n==== CROSSING REPORT ====\n");
    printf("Total Ticks: %u\n", totalTicks);
    printf("Cars Passed: %u\n", carsPassed);
    printf("Cars Waiting: %u\n", carsWaiting);
    printf("Night Mode: %s\n", READ_BIT(status, BIT_NIGHT) ? "ON" : "OFF");
    printf("Busy Mode: %s\n", READ_BIT(status, BIT_BUSY) ? "ON" : "OFF");
    printf("Status Byte (Hex): 0x%02X\n", status);
    
    printf("Status Byte (Binary): ");
    for (int i = 7; i >= 0; i--) {
        printf("%d", (status >> i) & 1);
    }
    printf("\n=========================\n");
}

int main(void) {
    int option;

    resetCrossing();
    do {
        printf("\n===== TRAFFIC LIGHT =====\n");
        printf("1. Show Light\n");
        printf("2. Advance One Tick\n");
        printf("3. Add Cars\n");
        printf("4. Toggle Night Mode\n");
        printf("5. Show Log\n");
        printf("6. Crossing Report\n");
        printf("7. Exit\n");
        printf("Select option: ");

        if (scanf("%d", &option) != 1) {
            printf("Invalid input! Exiting.\n");
            clearInputBuffer();
            break;
        }
        clearInputBuffer();

        switch (option) {
            case 1: drawLight(); break;
            case 2: tick(); break;
            case 3: addCars(); break;
            case 4: toggleNight(); break;
            case 5: showLog(); break;
            case 6: crossingReport(); break;
            case 7: printf("Goodbye!\n"); break;
            default: printf("Invalid choice, try again.\n"); break;
        }
    } while (option != 7);

    return 0;
}
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define COIN_KINDS  5U
#define TOY_COUNT   4U
#define NAME_LEN    16U

/* values in piastres */
static const uint16_t COIN_VALUE[COIN_KINDS] = {25U, 50U, 100U, 200U, 500U};

/* array to store count for each coin kind */
static uint16_t coinCount[COIN_KINDS];

typedef struct {
    char name[NAME_LEN];
    uint32_t price; /* in piastres */
} Toy_t;

static Toy_t shop[TOY_COUNT];

/* Function Prototypes */
static void seedBank(void);
static void addCoins(void);
static void takeCoins(void);
static uint32_t bankTotal(void);
static uint32_t sumCoins(const uint16_t *counts, uint8_t n);
static uint8_t biggestPile(void);
static void showBank(void);
static void drawBar(uint16_t value, uint16_t full, uint8_t width);
static void buyToy(void);
static void bankReport(void);

/* Seed initial values for bank and shop */
static void seedBank(void) {
    coinCount[0] = 4U;  /* 25pt */
    coinCount[1] = 2U;  /* 50pt */
    coinCount[2] = 5U;  /* 100pt */
    coinCount[3] = 1U;  /* 200pt */
    coinCount[4] = 0U;  /* 500pt */

    strncpy(shop[0].name, "Car", NAME_LEN);
    shop[0].price = 300U;

    strncpy(shop[1].name, "Robot", NAME_LEN);
    shop[1].price = 800U;

    strncpy(shop[2].name, "Puzzle", NAME_LEN);
    shop[2].price = 150U;

    strncpy(shop[3].name, "Doll", NAME_LEN);
    shop[3].price = 500U;
}

/* Recursive function to sum coins array - No loops allowed */
static uint32_t sumCoins(const uint16_t *counts, uint8_t n) {
    if (n == 0U) {
        return 0U;
    }
    return (uint32_t)counts[n - 1U] + sumCoins(counts, n - 1U);
}

static uint32_t bankTotal(void) {
    uint32_t total = 0U;
    uint8_t i;
    for (i = 0U; i < COIN_KINDS; i++) {
        total += (uint32_t)coinCount[i] * COIN_VALUE[i];
    }
    return total;
}

static void addCoins(void) {
    int coinIdx;
    int amount;

    printf("Enter coin index (0:25pt, 1:50pt, 2:100pt, 3:200pt, 4:500pt): ");
    if (scanf("%d", &coinIdx) != 1 || coinIdx < 0 || coinIdx >= (int)COIN_KINDS) {
        printf("Invalid coin type! No changes made.\n");
        return;
    }

    printf("Enter amount to add: ");
    if (scanf("%d", &amount) != 1 || amount <= 0) {
        printf("Invalid amount! No changes made.\n");
        return;
    }

    coinCount[coinIdx] += (uint16_t)amount;
    printf("Successfully added %d coins.\n", amount);
}

static void takeCoins(void) {
    int coinIdx;
    int amount;

    printf("Enter coin index (0:25pt, 1:50pt, 2:100pt, 3:200pt, 4:500pt): ");
    if (scanf("%d", &coinIdx) != 1 || coinIdx < 0 || coinIdx >= (int)COIN_KINDS) {
        printf("Invalid coin type! No changes made.\n");
        return;
    }

    printf("Enter amount to take: ");
    if (scanf("%d", &amount) != 1 || amount <= 0) {
        printf("Invalid amount! No changes made.\n");
        return;
    }

    if ((uint16_t)amount > coinCount[coinIdx]) {
        printf("Not enough coins! Refused and took nothing.\n");
    } else {
        coinCount[coinIdx] -= (uint16_t)amount;
        printf("Successfully removed %d coins.\n", amount);
    }
}

static uint8_t biggestPile(void) {
    uint8_t maxIdx = 0U;
    uint8_t i;
    for (i = 1U; i < COIN_KINDS; i++) {
        if (coinCount[i] > coinCount[maxIdx]) {
            maxIdx = i;
        }
    }
    return maxIdx;
}

static void drawBar(uint16_t value, uint16_t full, uint8_t width) {
    uint8_t filledLen;
    uint8_t i;

    if (full == 0U) {
        filledLen = 0U;
    } else if (value >= full) {
        filledLen = width;
    } else {
        filledLen = (uint8_t)(((uint32_t)value * width) / full);
    }

    for (i = 0U; i < filledLen; i++) {
        putchar('=');
    }
    for (i = filledLen; i < width; i++) {
        putchar('-');
    }
}

static void showBank(void) {
    uint8_t i;
    uint16_t maxCoins = 0U;

    for (i = 0U; i < COIN_KINDS; i++) {
        if (coinCount[i] > maxCoins) {
            maxCoins = coinCount[i];
        }
    }

    printf("\n--- Bank Status ---\n");
    for (i = 0U; i < COIN_KINDS; i++) {
        printf("%3upt | Count: %2u | [", COIN_VALUE[i], coinCount[i]);
        drawBar(coinCount[i], (maxCoins == 0U) ? 1U : maxCoins, 10U);
        printf("]\n");
    }
}

static void buyToy(void) {
    uint8_t i;
    int choice;
    uint32_t totalMoney = bankTotal();

    printf("\n--- Toy Shop ---\n");
    for (i = 0U; i < TOY_COUNT; i++) {
        printf("%u. %-10s - Price: %upt\n", i + 1U, shop[i].name, shop[i].price);
    }

    printf("Choose a toy to check (1-%u): ", TOY_COUNT);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > (int)TOY_COUNT) {
        printf("Invalid choice!\n");
        return;
    }

    i = (uint8_t)(choice - 1);
    if (totalMoney >= shop[i].price) {
        printf("You can buy it! Leftover money will be: %upt\n", totalMoney - shop[i].price);
    } else {
        printf("You cannot afford it! You need %upt more.\n", shop[i].price - totalMoney);
    }
}

static void bankReport(void) {
    uint32_t totalCoins = sumCoins(coinCount, COIN_KINDS);
    uint32_t totalMoney = bankTotal();
    uint8_t maxPileIdx = biggestPile();
    uint8_t affordableToys = 0U;
    uint8_t i;

    for (i = 0U; i < TOY_COUNT; i++) {
        if (totalMoney >= shop[i].price) {
            affordableToys++;
        }
    }

    printf("\n========== BANK REPORT ==========\n");
    printf("Total Money        : %u piastres\n", totalMoney);
    printf("Total Coins Count  : %u\n", totalCoins);
    printf("Tallest Coin Pile  : %upt (%u coins)\n", COIN_VALUE[maxPileIdx], coinCount[maxPileIdx]);
    printf("Affordable Toys    : %u out of %u\n", affordableToys, TOY_COUNT);
    printf("=================================\n");
}

int main(void) {
    int option = 0;

    seedBank();

    do {
        printf("\n===== MY PIGGY BANK =====\n");
        printf("1. Show Bank State\n");
        printf("2. Add Coins\n");
        printf("3. Take Coins\n");
        printf("4. Check Toy Shop\n");
        printf("5. Bank Report\n");
        printf("6. Exit\n");
        printf("Select option: ");

        if (scanf("%d", &option) != 1) {
            printf("Invalid input! Exiting.\n");
            break;
        }

        switch (option) {
            case 1: showBank(); break;
            case 2: addCoins(); break;
            case 3: takeCoins(); break;
            case 4: buyToy(); break;
            case 5: bankReport(); break;
            case 6: printf("Goodbye!\n"); break;
            default: printf("Invalid choice, try again.\n"); break;
        }
    } while (option != 6);

    return 0;
}
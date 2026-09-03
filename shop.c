#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define CANDY_KINDS 6U
#define BASKET_MAX 6U
#define NAME_LEN 20U

// --- 1. Data Structures ---
typedef struct {
    char name[NAME_LEN];
    uint16_t price;    /* in piastres */
    uint16_t stock;    /* how many are left on the shelf */
    uint16_t sold;     /* how many we sold today */
} Candy_t;

typedef struct {
    uint8_t candyId;
    uint8_t qty;
} Line_t;

// --- Static Global Variables ---
static Candy_t shelf[CANDY_KINDS];
static Line_t basket[BASKET_MAX];
static uint8_t basketLines = 0; /* how many slots of the basket are used */
static uint32_t cashDrawer = 0;

// --- 2. Function Definitions ---

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

static void openShop(void) {
    /* Initialize 6 candy kinds */
    strcpy(shelf[0].name, "Chocolate Bar");  shelf[0].price = 150; shelf[0].stock = 10; shelf[0].sold = 0;
    strcpy(shelf[1].name, "Gummy Bears");    shelf[1].price = 75;  shelf[1].stock = 15; shelf[1].sold = 0;
    strcpy(shelf[2].name, "Lollipop");        shelf[2].price = 25;  shelf[2].stock = 20; shelf[2].sold = 0;
    strcpy(shelf[3].name, "Bubble Gum");      shelf[3].price = 50;  shelf[3].stock = 8;  shelf[3].sold = 0;
    strcpy(shelf[4].name, "Toffee");          shelf[4].price = 100; shelf[4].stock = 5;  shelf[4].sold = 0;
    strcpy(shelf[5].name, "Sour Belt");       shelf[5].price = 200; shelf[5].stock = 12; shelf[5].sold = 0;

    basketLines = 0;
    cashDrawer = 0;
    printf("Shop is now OPEN!\n");
}

static void showShelf(void) {
    printf("\n--- CANDY SHELF ---\n");
    printf("ID | Name                 | Price (pt) | Stock\n");
    printf("----------------------------------------------\n");
    for (uint8_t i = 0; i < CANDY_KINDS; i++) {
        printf("%-2d | %-20s | %-10d | ", i, shelf[i].name, shelf[i].price);
        if (shelf[i].stock == 0) {
            printf("SOLD OUT\n");
        } else {
            printf("%d\n", shelf[i].stock);
        }
    }
}

static void addToBasket(void) {
    int inputId, inputQty;
    uint8_t id, qty;
    printf("Enter Candy ID to add: ");
    if (readInteger(&inputId) != 1 || inputId < 0 || inputId >= (int)CANDY_KINDS) {
        printf("Invalid Candy ID!\n");
        return;
    }
    id = (uint8_t)inputId;

    printf("Enter Quantity: ");
    if (readInteger(&inputQty) != 1 || inputQty <= 0 || inputQty > 255) {
        printf("Invalid quantity!\n");
        return;
    }
    qty = (uint8_t)inputQty;

    if (qty > shelf[id].stock) {
        printf("Not enough stock available!\n");
        return;
    }

    /* Check if already in basket */
    for (uint8_t i = 0; i < basketLines; i++) {
        if (basket[i].candyId == id) {
            if (basket[i].qty + qty > shelf[id].stock) {
                printf("Cannot add! Total requested exceeds stock.\n");
                return;
            }
            basket[i].qty += qty;
            printf("Updated quantity in basket!\n");
            return;
        }
    }

    /* Add as new line if basket isn't full */
    if (basketLines < BASKET_MAX) {
        basket[basketLines].candyId = id;
        basket[basketLines].qty = qty;
        basketLines++;
        printf("Added to basket!\n");
    } else {
        printf("Basket is full!\n");
    }
}

static void removeFromBasket(void) {
    if (basketLines == 0) {
        printf("Basket is empty!\n");
        return;
    }

    int inputLine;
    uint8_t lineIndex;
    printf("Enter line number to remove (0 to %d): ", basketLines - 1);
    if (readInteger(&inputLine) != 1 || inputLine < 0) {
        printf("Invalid line!\n");
        return;
    }
    lineIndex = (uint8_t)inputLine;

    if (lineIndex >= basketLines) {
        printf("Invalid line!\n");
        return;
    }

    /* Shift remaining lines up */
    for (uint8_t i = lineIndex; i < basketLines - 1; i++) {
        basket[i] = basket[i + 1];
    }
    basketLines--;
    printf("Item removed from basket.\n");
}

static uint32_t basketTotal(void) {
    uint32_t total = 0;
    for (uint8_t i = 0; i < basketLines; i++) {
        uint8_t id = basket[i].candyId;
        total += shelf[id].price * basket[i].qty;
    }
    return total;
}

static void showBasket(void) {
    printf("\n--- YOUR BASKET ---\n");
    if (basketLines == 0) {
        printf("Basket is empty.\n");
        printf("Total Cost: 0 pt\n");
        return;
    }
    for (uint8_t i = 0; i < basketLines; i++) {
        uint8_t id = basket[i].candyId;
        uint32_t lineCost = shelf[id].price * basket[i].qty;
        printf("Line %d: %s x%d @ %d pt each = %u pt\n", 
               i, shelf[id].name, basket[i].qty, shelf[id].price, lineCost);
    }
    printf("-------------------\nTotal Cost: %u pt\n", basketTotal());
}

static void giveChange(uint32_t change) {
    if (change == 0) {
        printf("No change, thank you!\n");
        return;
    }

    uint16_t coins[] = {500, 200, 100, 50, 25};
    printf("Change breakdown (%u pt):\n", change);

    for (int i = 0; i < 5; i++) {
        uint32_t count = change / coins[i];
        if (count > 0) {
            printf("- %u x %d pt\n", count, coins[i]);
            change %= coins[i];
        }
    }
    if (change > 0) {
        printf("Cannot make exact change; %u pt remains.\n", change);
    }
}

static void checkout(void) {
    uint32_t total = basketTotal();
    if (total == 0) {
        printf("Basket is empty. Nothing to checkout!\n");
        return;
    }

    int paid;
    printf("Total amount is %u pt. Enter amount handed over: ", total);
    if (readInteger(&paid) != 1 || paid < 0) {
        printf("Invalid payment! Basket remains untouched.\n");
        return;
    }

    if ((uint32_t)paid < total) {
        printf("Not enough money! Basket remains untouched.\n");
        return;
    }

    /* Deduct from shelf stock & update sales */
    for (uint8_t i = 0; i < basketLines; i++) {
        uint8_t id = basket[i].candyId;
        shelf[id].stock -= basket[i].qty;
        shelf[id].sold += basket[i].qty;
    }

    cashDrawer += total;
    uint32_t change = (uint32_t)paid - total;
    giveChange(change);

    /* Empty basket */
    basketLines = 0;
    printf("Checkout completed successfully!\n");
}

static uint8_t bestSeller(void) {
    uint8_t bestId = 0;
    uint16_t maxSold = shelf[0].sold;

    for (uint8_t i = 1; i < CANDY_KINDS; i++) {
        if (shelf[i].sold > maxSold) {
            maxSold = shelf[i].sold;
            bestId = i;
        }
    }
    return bestId;
}

static void dayReport(void) {
    uint32_t totalSoldItems = 0;
    printf("\n=== END OF DAY REPORT ===\n");
    printf("Money in Drawer: %u pt\n", cashDrawer);

    for (uint8_t i = 0; i < CANDY_KINDS; i++) {
        totalSoldItems += shelf[i].sold;
    }
    printf("Total candies sold: %u\n", totalSoldItems);

    uint8_t best = bestSeller();
    if (shelf[best].sold > 0) {
        printf("Best Seller: %s (%d sold)\n", shelf[best].name, shelf[best].sold);
    } else {
        printf("Best Seller: None sold today\n");
    }

    printf("Sold out items:\n");
    uint8_t anySoldOut = 0;
    for (uint8_t i = 0; i < CANDY_KINDS; i++) {
        if (shelf[i].stock == 0) {
            printf("- %s\n", shelf[i].name);
            anySoldOut = 1;
        }
    }
    if (!anySoldOut) printf("None\n");
}

int main(void) {
    int option;

    openShop();

    do {
        printf("\n===== CANDY SHOP =====\n");
        printf("1. Show shelf\n");
        printf("2. Add to basket\n");
        printf("3. Remove from basket\n");
        printf("4. Show basket\n");
        printf("5. Checkout\n");
        printf("6. Day report\n");
        printf("0. Exit\n");
        printf("Choose an option: ");

        int inputResult = readInteger(&option);
        if (inputResult == EOF) {
            printf("\nInput ended. Goodbye!\n");
            break;
        }
        if (inputResult != 1) {
            printf("Please enter a number.\n");
            continue;
        }

        switch (option) {
            case 1: showShelf(); break;
            case 2: addToBasket(); break;
            case 3: removeFromBasket(); break;
            case 4: showBasket(); break;
            case 5: checkout(); break;
            case 6: dayReport(); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid option.\n"); break;
        }
    } while (option != 0);

    return 0;
}
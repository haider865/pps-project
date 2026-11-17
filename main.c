#include <stdio.h>
#include <stdlib.h>

struct Checkpoint {
    int stage;
    char location[30];
    int time;
    int status;
};

struct Product {
    int id;
    int count;
    struct Checkpoint cp[10];
};

struct Product *products;
int total = 0;

struct Product *findProduct(int id) {
    int i;
    for (i = 0; i < total; i++) {
        if (products[i].id == id)
            return &products[i];
    }
    return NULL;
}

void addProduct() {
    int id;
    if (total == 20) {
        printf("List full\n");
        return;
    }
    printf("Enter ID: ");
    scanf("%d", &id);
    products[total].id = id;
    products[total].count = 0;
    total++;
    printf("Product added\n");
}

void addCheckpoint(struct Product *p) {
    struct Checkpoint *c;
    if (p->count == 10) {
        printf("Checkpoint list full\n");
        return;
    }
    c = &p->cp[p->count];
    printf("Stage (1=Factory 2=Warehouse 3=Distribution 4=Transit 5=Retailer): ");
    scanf("%d", &c->stage);
    printf("Location: ");
    scanf("%s", c->location);
    printf("Time in hours: ");
    scanf("%d", &c->time);
    printf("Status (1=Dispatched 2=In Transit 3=Delivered): ");
    scanf("%d", &c->status);
    p->count++;
    printf("Checkpoint added\n");
}

void viewHistory(struct Product *p) {
    int i;
    for (i = 0; i < p->count; i++) {
        printf("%d. Stage %d | %s | %d hrs | Status %d\n", i + 1,
               p->cp[i].stage, p->cp[i].location, p->cp[i].time, p->cp[i].status);
    }
}

void verifyChain(struct Product *p) {
    int i, problems = 0, now;
    if (p->count == 0) {
        printf("No checkpoints\n");
        return;
    }
    if (p->cp[0].stage != 1) {
        printf("Chain does not start at Factory\n");
        problems++;
    }
    for (i = 1; i < p->count; i++) {
        if (p->cp[i].stage > p->cp[i - 1].stage + 1) {
            printf("Skipped stage before checkpoint %d\n", i + 1);
            problems++;
        }
        if (p->cp[i].stage <= p->cp[i - 1].stage) {
            printf("Checkpoint %d is out of order\n", i + 1);
            problems++;
        }
        if (p->cp[i].time - p->cp[i - 1].time > 24) {
            printf("Delay before checkpoint %d\n", i + 1);
            problems++;
        }
    }
    if (p->cp[p->count - 1].status != 3) {
        printf("Enter current time in hours: ");
        scanf("%d", &now);
        if (now - p->cp[p->count - 1].time > 24) {
            printf("Missing update: no news for over 24 hours\n");
            problems++;
        }
    }
    if (problems == 0)
        printf("Chain is OK\n");
}

void saveFile() {
    FILE *f = fopen("tracking_log.txt", "w");
    int i, j;
    for (i = 0; i < total; i++) {
        fprintf(f, "Product %d\n", products[i].id);
        for (j = 0; j < products[i].count; j++) {
            fprintf(f, "  Stage %d, %s, %d hrs, Status %d\n",
                    products[i].cp[j].stage, products[i].cp[j].location,
                    products[i].cp[j].time, products[i].cp[j].status);
        }
    }
    fclose(f);
    printf("Saved\n");
}

int main() {
    int choice, id;
    struct Product *p;
    products = malloc(20 * sizeof(struct Product));

    do {
        printf("\n1.Add product\n2.Add checkpoint\n3.View history\n4.Verify chain\n5.Save\n6.Exit\nChoice: ");
        scanf("%d", &choice);
        if (choice == 1) {
            addProduct();
        } else if (choice >= 2 && choice <= 4) {
            printf("Enter ID: ");
            scanf("%d", &id);
            p = findProduct(id);
            if (p == NULL)
                printf("Not found\n");
            else if (choice == 2)
                addCheckpoint(p);
            else if (choice == 3)
                viewHistory(p);
            else
                verifyChain(p);
        } else if (choice == 5) {
            saveFile();
        }
    } while (choice != 6);

    saveFile();
    free(products);
    return 0;
}

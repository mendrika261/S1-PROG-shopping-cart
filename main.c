#include <stdio.h>
#include <stdlib.h>

typedef struct Customer Customer;
typedef struct Price Price;
typedef struct Product Product;
typedef struct BasketDetail BasketDetail;
typedef struct Basket Basket;
struct Customer {
    char* name;
    char* email;
};
Customer newCustomer(char* name, char* email) {
    Customer c;
    c.name = name;
    c.email = email;
    return c;
}
struct Price {
    double priceHT;
    double tax;
    double discount;
};
Price newPrice(double priceHT, double tax, double discount) {
    Price p;
    p.priceHT = priceHT;
    p.tax = tax;
    p.discount = discount;
    return p;
}
struct Product {
    char* designation;
    double stock;
    Price price;
};
Product newProduct(char* designation, int stock, Price price) {
    Product p;
    p.price = price;
    p.stock = stock;
    p.designation = designation;
    return p;
}
struct BasketDetail {
    Product product;
    double quantity;
    BasketDetail* before;
    BasketDetail* after;
};

struct Basket {
    Customer customer;
    BasketDetail* basketDetail;
};

/* Prix TTC */
double priceTTC(Price price) {
    return price.priceHT+(price.priceHT*price.tax/100);
}

/* Prix avec remise */
double FinalPrice(Price price) {
    return priceTTC(price)-(priceTTC(price)*price.tax/100);
}

/* Liste produit */
Product* productList() {
    Product* p = malloc(3*sizeof(Product));
    p[0] = newProduct("Coca Cola", 100, newPrice(1000, 20, 15));
    p[1] = newProduct("Fanta", 100, newPrice(1000, 20, 15));
    p[2] = newProduct("Bonbon Anglais", 100, newPrice(1000, 20, 15));
    return p;
}

/* afficher detail panier */
int checkAddBasketDetail(Product* product, double* quantity) {
    if(product->stock - *quantity < 0)
        *quantity = product->stock;
    if(*quantity == 0)
        return 0;
    return 1;
}
BasketDetail* newBasketDetail(Product* product, double quantity) {
    BasketDetail* basketDetail = malloc(sizeof(BasketDetail));
    basketDetail->product = *product;
    basketDetail->quantity = quantity;
    basketDetail->before = basketDetail;
    basketDetail->after = NULL;
    return basketDetail;
}
void addBasketDetail(BasketDetail* basketDetail, Product* product, double quantity) {
    int check = checkAddBasketDetail(product, &quantity);
    if(basketDetail->before == NULL && check == 1) {
        basketDetail->product = *product;
        basketDetail->quantity = quantity;
        basketDetail->after = NULL;
        basketDetail->before = basketDetail;
        product->stock -= quantity;
    } else if(check == 1) {
        BasketDetail* new = newBasketDetail(product, quantity);
        new->before = basketDetail->before;
        basketDetail->before->after = new;
        basketDetail->before = new;
        product->stock -= quantity;
    }
}
void addStartBasketDetail(BasketDetail* basketDetail, Product* product, double quantity) {
    BasketDetail* copy = malloc(sizeof(BasketDetail));

    copy->before = basketDetail;
    copy->after = basketDetail->after;
    copy->product = basketDetail->product;
    copy->quantity = basketDetail->quantity;

    basketDetail->after = copy;
    basketDetail->product = *product;
    basketDetail->quantity = quantity;
}
int insertBasketDetail(BasketDetail* basketDetail, Product* product, double quantity, int place) {
    int iPlace=1;
    int check = checkAddBasketDetail(product, &quantity);
    if(check == 1) {
        if(place == 1) {
            addStartBasketDetail(basketDetail, product, quantity);
            return 0;
        } else {
            BasketDetail *new = newBasketDetail(product, quantity);
            for (BasketDetail *iBD = basketDetail; iBD != NULL; iBD = iBD[0].after, iPlace++) {
                if (place == iPlace) { // 4 1 2 3
                    new->before = iBD->before;
                    new->after = iBD;
                    iBD->before->after = new;
                    iBD->before = new;
                    return 1;
                }
            }
        }
        addBasketDetail(basketDetail, product, quantity);
    }
    return 2;
}
void printCustomer(Customer customer) {
    printf("Nom: %s\n", customer.name);
    printf("Email: %s\n", customer.email);
}
void printBasket(BasketDetail* basketDetail) {
    for(BasketDetail* iBD=basketDetail; iBD!=NULL; iBD=iBD[0].after) {
        double priceF = FinalPrice(iBD[0].product.price);
      printf("%s (%s)\n\t", iBD[0].product.designation, iBD[0].before->product.designation);
      printf("(Ar%g*%g)\t\t\t\t", priceF,iBD[0].quantity);
      printf("Ar %g\n", priceF*iBD[0].quantity);
    }
}
void printInverseBasket(BasketDetail* basketDetail) {
    BasketDetail* iBD=basketDetail->before;
    do {
        double priceF = FinalPrice(iBD[0].product.price);
        printf("%s (%s)\n\t", iBD[0].product.designation, iBD[0].before->product.designation);
        printf("(Ar%g*%g)\t\t\t\t", priceF,iBD[0].quantity);
        printf("Ar %g\n", priceF*iBD[0].quantity);
        iBD=iBD[0].before;
    } while (iBD!=basketDetail->before);
}

int main() {
    Product* p = productList();

    BasketDetail* ba= malloc(sizeof (BasketDetail));
    Customer c = newCustomer("Mendrika Ramialison", "ramialison.mendrika@gmail.com");
    printCustomer(c);

    addBasketDetail(ba, &p[0], 2);
    addBasketDetail(ba, &p[1], 3);
    insertBasketDetail(ba, &p[2], 1, 1);

    printBasket(ba);
    return 0;
}
//
// Created by Nadav Barsheshet on 6/6/18.
//

#ifndef CLIONPROJECT_PRODUCTARGS_H
#define CLIONPROJECT_PRODUCTARGS_H


#include "Product.h"

class ProductArgs {
public:
    ProductArgs(int numOfProducts ,Product* products) : numOfProducts(numOfProducts), products(products){}
    virtual ~ProductArgs() = default;

    int getNumOfProducts() const {
        return numOfProducts;
    }

    Product *getProducts() const {
        return products;
    }

private:
    int numOfProducts;
    Product* products;
};


#endif //CLIONPROJECT_PRODUCTARGS_H

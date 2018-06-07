#ifndef FACTORY_H_
#define FACTORY_H_

#include <pthread.h>
#include <list>
#include "Product.h"
#include <map>

class ProduceArgs;//forward declaration;
class tryBuyOneArgs;//forward declaration;

class Factory{
public:
    Factory();
    ~Factory();
    
    void startProduction(int num_products, Product* products, unsigned int id);
    void produce(int num_products, Product* products);
    void finishProduction(unsigned int id);
    
    void startSimpleBuyer(unsigned int id);
    int tryBuyOne();
    int finishSimpleBuyer(unsigned int id);

    void startCompanyBuyer(int num_products, int min_value,unsigned int id);
    std::list<Product> buyProducts(int num_products);
    void returnProducts(std::list<Product> products,unsigned int id);
    int finishCompanyBuyer(unsigned int id);

    void startThief(int num_products,unsigned int fake_id);
    int stealProducts(int num_products,unsigned int fake_id);
    int finishThief(unsigned int fake_id);

    void closeFactory();
    void openFactory();
    
    void closeReturningService();
    void openReturningService();
    
    std::list<std::pair<Product, int>> listStolenProducts();
    std::list<Product> listAvailableProducts();

//private:

    bool factoryOpen;
    bool returningServiceOpen;
    pthread_mutexattr_t factoryAttr;
    pthread_mutex_t factoryLock;
    pthread_mutexattr_t retServiceAttr;
    pthread_mutex_t retServiceLock;

    int numOfThieves;
    int numOfThievesWaiting;
    pthread_cond_t thievesCond;

    int numOfCompanies;
    int numOfCompaniesWaiting;
    pthread_cond_t companyCond;

    int numOfSingleBuyers;
    pthread_cond_t singleBuyerCond;


    std::map<unsigned int, pthread_t> threadsMap;
    std::list<std::pair<Product, int>> stolenProductsList;
    std::list<Product> availableProductsList;

    static void* produceWrapper(void *pa);
    static void* tryBuyOneWrapper(void* tboa);
    std::map<unsigned int, ProduceArgs*> produceArgMap;
    std::map<unsigned int, tryBuyOneArgs*> tryBuyOneArgMap;
    std::map<unsigned int, int> singleBuyerSuccessfulBuys;

};


class ProduceArgs{
public:
    ProduceArgs(int numOfProducts ,Product* products,Factory* inst) : numOfProducts(numOfProducts), products(products), factory(inst){}
    virtual ~ProduceArgs() = default;

    int getNumOfProducts() const {
        return numOfProducts;
    }

    Product *getProducts() const {
        return products;
    }

    Factory *getFactory() const {
        return factory;
    }

private:
    int numOfProducts;
    Product* products;
    Factory* factory;
};


class tryBuyOneArgs{
public:
    tryBuyOneArgs(Factory *factory) : factory(factory) {}

    virtual ~tryBuyOneArgs() {}

    Factory *getFactory() const {
        return factory;
    }

private:
    Factory* factory;
};

class CompanyBuyProductArgs{
public:

private:
    int numOfProducts;
    int minValue;
    Factory* factory;
};



#endif // FACTORY_H_

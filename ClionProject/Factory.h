#ifndef FACTORY_H_
#define FACTORY_H_

#include <pthread.h>
#include <list>
#include "Product.h"
#include <map>

class ProduceArgs;//forward declaration;
class TryBuyOneArgs;//forward declaration;

class StealArgs;//forward declaration;

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

private:

    //Factory:
    bool factoryOpen;
    bool returningServiceOpen;
    pthread_mutexattr_t factoryAttr;
    pthread_mutex_t factoryLock;
    pthread_cond_t factoryCond;
    std::list<std::pair<Product, int>> stolenProductsList;
    std::list<Product> availableProductsList;
    std::map<unsigned int, pthread_t> threads;

    int numOfThieves;

    //Wrappers:
    static void* produceWrapper(void *pa);
    static void* tryBuyOneWrapper(void* factory);
    static void* companyBuyWrapper(void* ca);
    static void* stealWrapper(void * sa);




};


class ProduceArgs{
public:
    ProduceArgs(int numOfProducts ,Product* products,Factory* inst) : numOfProducts(numOfProducts),
                                                                      products(products), factory(inst){}
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


class CompanyArgs{
public:
    CompanyArgs(int numProducts, int minValue,int id, Factory *factory) : numProducts(numProducts), minValue(minValue),
                                                                    id(id),numReturned(0), factory(factory) {}

    virtual ~CompanyArgs() = default;

    int getNumProducts() const {
        return numProducts;
    }

    int getMinValue() const {
        return minValue;
    }

    std::list<Product> getList(){
        std::list<Product> returnedProducts;
        for(auto product : list){
            if(product.getValue() < minValue){
                returnedProducts.push_back(product);
            }
        }
        numReturned = returnedProducts.size();
        return returnedProducts;
    }

    Factory *getFactory() const {
        return factory;
    }

    void setList(std::list<Product> list) {
        CompanyArgs::list = list;
    }

    int getId() const {
        return id;
    }

    int getNumReturned() const {
        return numReturned;
    }


private:
    int numProducts;
    int minValue;
    int id;
    int numReturned;
    std::list<Product> list;
    Factory* factory;
};



class StealArgs{
public:
    StealArgs(int numProducts, unsigned int fakeId, Factory *factory) : numProducts(numProducts), fakeId(fakeId),
                                                                        factory(factory) {}

    virtual ~StealArgs() = default;

    int getNumProducts() const {
        return numProducts;
    }

    unsigned int getFakeId() const {
        return fakeId;
    }

    Factory *getFactory() const {
        return factory;
    }

private:
    int numProducts;
    unsigned int fakeId;
    Factory* factory;
};



#endif // FACTORY_H_

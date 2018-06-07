#include "Factory.h"

void* produceWraper(void* pa);

Factory::Factory() :
                    factoryOpen(false),
                    returningServiceOpen(false),
                    numOfThieves(0),
                    numOfThievesWaiting(0),
                    numOfCompanies(0),
                    numOfCompaniesWaiting(0),
                    numOfSingleBuyers(0)
{
    pthread_mutexattr_init(&factoryAttr);
    pthread_mutexattr_settype(&factoryAttr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&factoryLock, &factoryAttr);

    pthread_mutexattr_init(&retServiceAttr);
    pthread_mutexattr_settype(&retServiceAttr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&retServiceLock, &retServiceAttr);

    pthread_cond_init(&thievesCond,NULL);
    pthread_cond_init(&companyCond,NULL);
    pthread_cond_init(&singleBuyerCond,NULL);

}

Factory::~Factory(){
    for(auto pair : produceArgMap){
        delete pair.second;
    }
    for(auto pair : tryBuyOneArgMap){
        delete pair.second;
    }
}

void* Factory::produceWrapper(void *pa) {
    ProduceArgs* arg = (ProduceArgs*)pa;
    arg->getFactory()->produce(arg->getNumOfProducts(),arg->getProducts());
}

void *Factory::tryBuyOneWrapper(void *tboa) {
    tryBuyOneArgs* arg = (tryBuyOneArgs*)tboa;
    arg->getFactory()->tryBuyOne();
}


void Factory::startProduction(int num_products, Product* products,unsigned int id){
    produceArgMap[id] = new ProduceArgs(num_products,products,this);
    pthread_create(&threadsMap[id], NULL, produceWrapper, produceArgMap[id]);

}

void Factory::produce(int num_products, Product* products){

}

void Factory::finishProduction(unsigned int id){
    pthread_cancel(threadsMap[id]);
}

void Factory::startSimpleBuyer(unsigned int id){
    tryBuyOneArgMap[id] = new tryBuyOneArgs(this);
    pthread_create(&threadsMap[id],NULL,tryBuyOneWrapper,tryBuyOneArgMap[id]);
}

int Factory::tryBuyOne(){
    return -1;
}

int Factory::finishSimpleBuyer(unsigned int id){
    pthread_cancel(threadsMap[id]);
    return singleBuyerSuccessfulBuys.find(id) != singleBuyerSuccessfulBuys.end() ? singleBuyerSuccessfulBuys[id] : -1;
}

void Factory::startCompanyBuyer(int num_products, int min_value,unsigned int id){
}

std::list<Product> Factory::buyProducts(int num_products){
    return std::list<Product>();
}

void Factory::returnProducts(std::list<Product> products,unsigned int id){
}

int Factory::finishCompanyBuyer(unsigned int id){
    return 0;
}

void Factory::startThief(int num_products,unsigned int fake_id){
}

int Factory::stealProducts(int num_products,unsigned int fake_id){
    return 0;
}

int Factory::finishThief(unsigned int fake_id){
    return 0;
}

void Factory::closeFactory(){
}

void Factory::openFactory(){
}

void Factory::closeReturningService(){
}

void Factory::openReturningService(){
}

std::list<std::pair<Product, int>> Factory::listStolenProducts(){
    return std::list<std::pair<Product, int>>();
}

std::list<Product> Factory::listAvailableProducts(){
    return std::list<Product>();
}






#include "Factory.h"


#define FINISH_THREAD(id) {\
    void* res;\
    pthread_join(threads[id],&res);\
    int* to_delete = (int*)res;\
    int to_return = *to_delete;\
    threads.erase(id);\
    delete to_delete;\
    return to_return;\
}

const int SINGLE_BUYER_COULDNT_BUY = -1;


Factory::Factory() :
                    factoryOpen(true),
                    returningServiceOpen(true),
                    numOfThieves(0)
{
    pthread_mutexattr_init(&factoryAttr);
    pthread_mutexattr_settype(&factoryAttr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&factoryLock, &factoryAttr);
    pthread_cond_init(&factoryCond,NULL);
}

Factory::~Factory(){
    pthread_mutex_destroy(&factoryLock);
    pthread_cond_destroy(&factoryCond);
    pthread_mutexattr_destroy(&factoryAttr);
}

void* Factory::produceWrapper(void *pa) {
    ProduceArgs* arg = (ProduceArgs*)pa;
    arg->getFactory()->produce(arg->getNumOfProducts(),arg->getProducts());
    delete arg;
    return NULL;
}

void *Factory::tryBuyOneWrapper(void *factory) {
    Factory* arg = (Factory*)factory;
    return new int(arg->tryBuyOne());
}

void *Factory::companyBuyWrapper(void *ca) {
    CompanyArgs* arg = (CompanyArgs*)ca;
    arg->setList(arg->getFactory()->buyProducts(arg->getNumProducts()));
    arg->getFactory()->returnProducts(arg->getList(),arg->getId());
    int numOfProductsReturned = arg->getNumReturned();
    delete arg;
   // int* pNumOfProductsReturned = new int(numOfProductsReturned);
    return new int(numOfProductsReturned);
}

void *Factory::stealWrapper(void *sa) {
    StealArgs* arg = (StealArgs*)sa;
    int numOfStolenProducts = arg->getFactory()->stealProducts(arg->getNumProducts(),arg->getFakeId());
    delete arg;
    return new int(numOfStolenProducts);
}

void Factory::startProduction(int num_products, Product* products,unsigned int id){
    ProduceArgs* pa = new ProduceArgs(num_products,products,this);
    pthread_create(&threads[id], NULL, produceWrapper, pa);
}

void Factory::produce(int num_products, Product* products){
    pthread_mutex_lock(&factoryLock);

    for (int i = 0; i < num_products; ++i) {
        availableProductsList.push_back(products[i]);
    }
    pthread_cond_broadcast(&factoryCond);
    pthread_mutex_unlock(&factoryLock);
}

void Factory::finishProduction(unsigned int id){
    pthread_join(threads[id],NULL);
    threads.erase(id);
}

void Factory::startSimpleBuyer(unsigned int id){
    pthread_t t;
    threads.insert(std::pair<unsigned int,pthread_t>(id,t));
   pthread_create(&threads.find(id)->second,NULL,tryBuyOneWrapper,this);
}

int Factory::tryBuyOne(){

    if(!pthread_mutex_trylock(&factoryLock)){
        if(!factoryOpen || !availableProductsList.size()){
            pthread_mutex_unlock(&factoryLock);
            return SINGLE_BUYER_COULDNT_BUY;
        }
        //buy product:
        Product product = availableProductsList.front();
        availableProductsList.pop_front();
        pthread_mutex_unlock(&factoryLock);
        return product.getId();
    }
    return SINGLE_BUYER_COULDNT_BUY;
}

int Factory::finishSimpleBuyer(unsigned int id){
    void* res;
    pthread_t t = threads[id];
    threads.erase(id);
    pthread_join(t,&res);
    int* to_delete = (int*)res;
    int to_return = *to_delete;
    //threads.erase(id);
    delete to_delete;
    return to_return;
    //FINISH_THREAD(id)
}

void Factory::startCompanyBuyer(int num_products, int min_value,unsigned int id){
    CompanyArgs* ca = new CompanyArgs(num_products,min_value,id,this);
    pthread_create(&threads[id],NULL,companyBuyWrapper,ca);
}

std::list<Product> Factory::buyProducts(int num_products){
    pthread_mutex_lock(&factoryLock);
    while(numOfThieves > 0|| !factoryOpen || availableProductsList.size() < num_products){
        pthread_cond_wait(&factoryCond,&factoryLock);
    }
    std::list<Product> boughtItems;
    int count = 0;
    for(auto product : availableProductsList){
        if(count == num_products) break;
        boughtItems.push_back(product);
        count++;
    }
    while (count){
        availableProductsList.pop_front();
        count--;
    }
    pthread_mutex_unlock(&factoryLock);
    return boughtItems;
}

void Factory::returnProducts(std::list<Product> products ,unsigned int id){
    if(!products.size()) return;
    pthread_mutex_lock(&factoryLock);
    while (numOfThieves > 0 || !factoryOpen || !returningServiceOpen){
        pthread_cond_wait(&factoryCond,&factoryLock);
    }
    for(auto product : products){
        availableProductsList.push_back(product);
    }
    pthread_cond_broadcast(&factoryCond);
    pthread_mutex_unlock(&factoryLock);
}

int Factory::finishCompanyBuyer(unsigned int id){

    void* res;
    pthread_join(threads[id],&res);
    int* to_delete = (int*)res;
    int to_return = *to_delete;
    threads.erase(id);
    delete to_delete;
    return to_return;

    //FINISH_THREAD(id)

}

void Factory::startThief(int num_products,unsigned int fake_id){
    StealArgs* sa = new StealArgs(num_products,fake_id,this);
    //numOfThieves++;
    pthread_create(&threads[fake_id],NULL,stealWrapper,sa);
}

int Factory::stealProducts(int num_products,unsigned int fake_id){
    pthread_mutex_lock(&factoryLock);
    numOfThieves++;
    while (!factoryOpen){
        pthread_cond_wait(&factoryCond,&factoryLock);
    }
    int numOfProductStolen = 0;
    if(availableProductsList.size()){
        for(auto product : availableProductsList){
            if(numOfProductStolen == num_products) break;
            stolenProductsList.push_back(std::pair<Product,int>(product,fake_id));
            numOfProductStolen++;
        }
        int tmpNumOfStolen = numOfProductStolen;
        while (tmpNumOfStolen--) availableProductsList.pop_front();
    }
    if(!(--numOfThieves)) pthread_cond_broadcast(&factoryCond);
    pthread_mutex_unlock(&factoryLock);
    return numOfProductStolen;
}

int Factory::finishThief(unsigned int fake_id){
    FINISH_THREAD(fake_id)
}

void Factory::closeFactory(){
    pthread_mutex_lock(&factoryLock);
    factoryOpen = false;
    pthread_mutex_unlock(&factoryLock);
}

void Factory::openFactory(){
    pthread_mutex_lock(&factoryLock);
    factoryOpen = true;
    pthread_cond_broadcast(&factoryCond);
    pthread_mutex_unlock(&factoryLock);
}

void Factory::closeReturningService(){
    pthread_mutex_lock(&factoryLock);
    returningServiceOpen = false;
    pthread_mutex_unlock(&factoryLock);
}

void Factory::openReturningService(){
    pthread_mutex_lock(&factoryLock);
    returningServiceOpen = true;
    pthread_cond_broadcast(&factoryCond);
    pthread_mutex_unlock(&factoryLock);
}

std::list<std::pair<Product, int>> Factory::listStolenProducts(){
    pthread_mutex_lock(&factoryLock);
    std::list<std::pair<Product, int>> toReturn = stolenProductsList;
    pthread_mutex_unlock(&factoryLock);
    return toReturn;
}

std::list<Product> Factory::listAvailableProducts(){
    pthread_mutex_lock(&factoryLock);
    std::list<Product> toReturn = availableProductsList;
    pthread_mutex_unlock(&factoryLock);
    return toReturn;
}










#include <iostream>
#include <map>

/* this function is run by the second thread */
void *inc_x(void *x_void_ptr)
{

/* increment x to 100 */
    int *x_ptr = (int *)x_void_ptr;
    while(++(*x_ptr) < 100);

    printf("x increment finished\n");

/* the function must return something - NULL will do */
    return NULL;

}

int main()
{

    int x = 0, y = 0;
    std::map<unsigned int, pthread_t> map;
/* show the initial values of x and y */
    printf("x: %d, y: %d\n", x, y);

/* this variable is our reference to the second thread */
    //map[1];

    //pthread_t inc_x_thread;

/* create a second thread which executes inc_x(&x) */
    if(pthread_create(&map[1], NULL, inc_x, &x)) {

        fprintf(stderr, "Error creating thread\n");
        return 1;

    }
/* increment y to 100 in the first thread */
    while(++y < 100);

    printf("y increment finished\n");

/* wait for the second thread to finish */
    if(pthread_join(map[1], NULL)) {

        fprintf(stderr, "Error joining thread\n");
        return 2;

    }

    double arr[] = {1,2,3,4,5,6,7};
    std::cout << "size of array is: " << sizeof(arr)/ sizeof(*arr) << std::endl;
    std::cout << "array at index 7 is: " << arr[7] << std::endl;
    std::cout << "array at index 8 is: " << arr[8] << std::endl;
    std::cout << "array at index 100 is: " << arr[100] << std::endl;

/* show the results - x is now 100 thanks to the second thread */
    printf("x: %d, y: %d\n", x, y);

    return 0;

}
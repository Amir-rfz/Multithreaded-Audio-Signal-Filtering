#include "thread.hpp"

void create_threads(void *(*func)(void *), const std::vector<float>& input_audio, std::vector<float>& output_audio, std::vector<std::atomic<bool>>& ready_flags)
{
    
    pthread_t threads_l[NUMBER_OF_THREADS];
    int rc;
    long t;
    for (t = 0; t < NUMBER_OF_THREADS; t++)
    {
        ThreadArgs* args = new ThreadArgs(t ,input_audio, output_audio, ready_flags);
        rc = pthread_create(&threads_l[t], NULL, func, args);
        if (rc)
        {
            std::cout << "Error :unable to create thread" << rc << std::endl;
            exit(-1);
        }
    }
    for (t = 0; t < NUMBER_OF_THREADS; t++)
    {
        rc = pthread_join(threads_l[t], NULL);
        if (rc)
        {
            std::cout << "Error :unable to join threads" << rc << std::endl;
            exit(-1);
        }
    }
}
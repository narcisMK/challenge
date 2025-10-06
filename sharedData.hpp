#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <semaphore.h>

struct  sharedData{
    int counter;
    sem_t semaphore;
};

inline sharedData*  get_share_memory(const std::string& sh_name, const std::string& sem_name){
    int shm_fd = shm_open(sh_name.c_str(), O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1 ){
        std::print(stderr,"Failed to create shared memory");
        return nullptr;
    }
    ftruncate(shm_fd, sizeof(sharedData));

    void* ptr = mmap(nullptr, sizeof(sharedData),
                     PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::print(stderr,"Failed to map shared memory");
        return nullptr;
    }

    auto* data = static_cast<sharedData*>(ptr);
    data->counter = 1;

    sem_t* sem = sem_open(sem_name.c_str(), O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        std::print(stderr,"Failed to initialize semaphore");
        munmap(data, sizeof(sharedData));
        shm_unlink(sh_name.c_str());
        return nullptr;
    }

    return data;
}

inline void free_share_memory(const std::string& name, sharedData* data) {
    sem_destroy(&data->semaphore);
    munmap(data, sizeof(sharedData));
    shm_unlink(name.c_str());
}
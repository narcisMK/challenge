#include <iostream>
#include <chrono>
#include <thread>
#include "sharedData.hpp"

constexpr int MAX_COUNT = 10;
const std::string SHM_NAME = "/nargess_shm";
const std::string SEM_NAME = "/nargess_sem";

void run_process(const std::string& role, sharedData* data) {
    while (true) {
        sem_wait(&data->semaphore);

        if (data->counter >= MAX_COUNT) {
            sem_post(&data->semaphore);
            break;
        }

        std::println("[{}] Received: {} " ,role,data->counter);
        ++data->counter;
        std::println("[{}] Sending: {} " ,role, data->counter);
        sem_post(&data->semaphore);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    sharedData* data = get_share_memory(SHM_NAME,SEM_NAME);
    if (!data){
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        std::print(stderr,"Failed to fork process");
        free_share_memory(SHM_NAME, data);
        return 1;
    }

    if (pid == 0) {
        run_process("Receiver",data);
    } else {
        std::println("[Initiator] Starting with: {} " ,data->counter);

        run_process("Initiator",data);

        wait(nullptr);
        free_share_memory(SHM_NAME, data);
    }

    std::println("Program finished.");
    return 0;
}

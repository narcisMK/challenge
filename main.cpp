#include <iostream>
#include <chrono>
#include <thread>
#include "sharedData.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

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

//        std::println("[{}] Received: {} " ,role,data->counter);
        spdlog::info("[{}] Received: {} " ,role,data->counter);
        ++data->counter;
//        std::println("[{}] Sending: {} " ,role, data->counter);
        spdlog::info("[{}] Sending: {} " ,role, data->counter);
        sem_post(&data->semaphore);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("challenge_log.txt", true);

    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    spdlog::set_default_logger(logger);

    sharedData* data = get_share_memory(SHM_NAME,SEM_NAME);
    if (!data){
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        spdlog::error("Failed to fork process");
        free_share_memory(SHM_NAME, data);
        return 1;
    }

    if (pid == 0) {
        run_process("Receiver",data);
    } else {
        spdlog::info("[Initiator] Starting with: {} " ,data->counter);

        run_process("Initiator",data);

        wait(nullptr);
        free_share_memory(SHM_NAME, data);
    }

    spdlog::info("Program finished.");
    return 0;
}

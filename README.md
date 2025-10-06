This project demonstrates inter-process communication (IPC) using POSIX shared memory and semaphores in C++23. Two processes — an initiator and a receiver — exchange a counter value through shared memory, synchronized using a semaphore.



# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the executable
./Challenge

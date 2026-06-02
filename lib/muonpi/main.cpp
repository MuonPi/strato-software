#include "muonpi.h"
#include <iostream>

int main() {
    auto strato_muonpi = std::make_unique<MUONPI>();
    strato_muonpi->start();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "connected: " << (strato_muonpi->isConnected() ? "true" : "false") << std::endl;
    }
    return EXIT_SUCCESS;
}
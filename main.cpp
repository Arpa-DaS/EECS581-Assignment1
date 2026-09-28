#include <iostream>
#include <string>

#include "ipv4.h"

int main()
{
    std::string line;

    while (true) {
        std::cout << "Enter a string (or 'END' to quit): ";
        if (!std::getline(std::cin, line)) {
            // End of input stream (e.g. Ctrl-D / piped file) - exit cleanly.
            std::cout << "\n";
            break;
        }
        if (line == "END") {
            break;
        }

        unsigned long address = 0;
        int port = -1;

        if (extractIPv4(line, address, port)) {
            std::cout << "Extracted IPv4 address: "
                      << ((address >> 24) & 0xFF) << '.'
                      << ((address >> 16) & 0xFF) << '.'
                      << ((address >> 8) & 0xFF) << '.'
                      << (address & 0xFF)
                      << " (decimal value: " << address << ", port: ";
            if (port == -1) {
                std::cout << "none";
            } else {
                std::cout << port;
            }
            std::cout << ")\n";
        } else {
            std::cout << "Invalid input: no valid IPv4 address found\n";
        }
    }

    std::cout << "Program terminated." << std::endl;
    return 0;
}

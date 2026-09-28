// Unit tests for extractIPv4. Build: make test
#include <iostream>
#include <string>

#include "ipv4.h"

struct Case {
    const char* input;
    bool expectFound;
    unsigned long expectAddr;
    int expectPort;
    const char* why;
};

static const Case cases[] = {
    // ---- From the assignment's sample run ----
    {"connecting to 192.168.1.1 now", true, 3232235777UL, -1, "sample: plain address in text"},
    {"server=10.0.0.255:8080end", true, 167772415UL, 8080, "sample: port, garbage after"},
    {"192a168.1.1.1", true, 2818638081UL, -1, "sample: letter splits token"},
    {"192.168.1.1.", false, 0, -1, "sample: trailing period"},
    {"Connection from 192.168.1.1 refused", true, 3232235777UL, -1, "sample"},
    {"192.168.01.1", false, 0, -1, "sample: leading zero"},
    {"1.2.3.4:99999", false, 0, -1, "sample: port > 65535"},
    {"12.34.56", false, 0, -1, "sample: three octets"},
    {"no number here", false, 0, -1, "sample: no digits"},

    // ---- Boundaries / numeric limits ----
    {"0.0.0.0", true, 0UL, -1, "all-zero address"},
    {"255.255.255.255", true, 4294967295UL, -1, "max address"},
    {"255.255.255.255:65535", true, 4294967295UL, 65535, "max address and port"},
    {"1.2.3.4:0", true, 16909060UL, 0, "port 0 is valid"},
    {"256.1.1.1", false, 0, -1, "octet 256"},
    {"1.2.3.256", false, 0, -1, "last octet 256"},
    {"1.2.3.4:65536", false, 0, -1, "port 65536"},
    {"1234.1.1.1", false, 0, -1, "4-digit octet"},
    {"1.2.3.4:123456", false, 0, -1, "6-digit port"},
    {"1.2.3.99999999999999999999999", false, 0, -1, "huge octet must not overflow"},
    {"1.2.3.4:99999999999999999999999", false, 0, -1, "huge port must not overflow"},

    // ---- Leading zeros ----
    {"00.1.1.1", false, 0, -1, "octet 00"},
    {"1.2.3.04", false, 0, -1, "last octet 04"},
    {"1.2.3.4:080", false, 0, -1, "port 080"},
    {"1.2.3.4:00", false, 0, -1, "port 00"},
    {"10.20.30.40", true, 169090600UL, -1, "zeros that are not leading"},

    // ---- Structural errors ----
    {"1..2.3", false, 0, -1, "empty octet"},
    {"1.2.3.4.5", false, 0, -1, "five octets"},
    {".1.2.3.4", false, 0, -1, "leading period"},
    {"1.2.3.4:", false, 0, -1, "colon with no port"},
    {"1.2.3.4::80", false, 0, -1, "double colon"},
    {"1.2.3.4:80:", false, 0, -1, "second colon after port"},
    {"1.2.3.4:80.", false, 0, -1, "period after port"},
    {"1.2.3:4", false, 0, -1, "colon before fourth octet"},
    {":1.2.3.4", false, 0, -1, "colon glued before address"},
    {"1.2.3.4.:80", false, 0, -1, "period then colon"},
    {".", false, 0, -1, "lone period"},
    {"", false, 0, -1, "empty line"},

    // ---- Garbage handling / multiple candidates ----
    {"a1.2.3.4b", true, 16909060UL, -1, "letters on both sides"},
    {"ip: 1.2.3.4", true, 16909060UL, -1, "colon separated by space"},
    {"ip:1.2.3.4", false, 0, -1, "colon touching address makes token invalid"},
    {"1.2.3 then 5.6.7.8", true, 84281096UL, -1, "invalid first, valid second"},
    {"1.2.3.4 and 5.6.7.8", true, 16909060UL, -1, "two valid: first wins"},
    {"1.2.3.4-5.6.7.8", true, 16909060UL, -1, "dash separates tokens"},
    {"at 10:30:00 host 172.16.0.1:22", true, 2886729729UL, 22, "time stamp is an invalid token"},
    {"1.2.3.4:80 5.6.7.8", true, 16909060UL, 80, "port kept with first"},
    {"x\t8.8.8.8\n", true, 134744072UL, -1, "tabs/newlines are garbage"},
};

int main()
{
    int passed = 0;
    int failed = 0;

    for (const Case& c : cases) {
        // Pre-load with junk to confirm the function always sets outputs.
        unsigned long addr = 12345;
        int port = 777;
        bool found = extractIPv4(c.input, addr, port);

        bool ok = found == c.expectFound && addr == c.expectAddr && port == c.expectPort;
        if (ok) {
            ++passed;
        } else {
            ++failed;
            std::cout << "FAIL: \"" << c.input << "\" (" << c.why << ")\n"
                      << "   expected found=" << c.expectFound << " addr=" << c.expectAddr
                      << " port=" << c.expectPort << "\n"
                      << "   got      found=" << found << " addr=" << addr
                      << " port=" << port << "\n";
        }
    }

    std::cout << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

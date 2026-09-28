#include "ipv4.h"

#include <cctype>
#include <cstddef>

namespace {

// Only digits, '.', and ':' can ever be part of a candidate token.
bool isTokenChar(char c)
{
    return std::isdigit(static_cast<unsigned char>(c)) || c == '.' || c == ':';
}

// Parses one numeric field starting at s[pos], stopping at the first non-digit
// or at 'end'. Digits are accumulated by hand (no library conversion).
// Rejects: empty field, more than maxDigits digits, a leading zero on a
// multi-digit value, or a value above maxValue.
// On return, pos points just past the last digit consumed.
bool parseField(const std::string& s, std::size_t& pos, std::size_t end,
                int maxDigits, unsigned long maxValue, unsigned long& value)
{
    const std::size_t start = pos;
    value = 0;

    while (pos < end && std::isdigit(static_cast<unsigned char>(s[pos]))) {
        // Check the digit count BEFORE accumulating so the value can never
        // grow large enough to overflow, no matter how long the digit run is.
        if (static_cast<int>(pos - start) >= maxDigits) {
            return false;
        }
        value = value * 10 + static_cast<unsigned long>(s[pos] - '0');
        ++pos;
    }

    const std::size_t len = pos - start;
    if (len == 0) return false;                  // empty field
    if (len > 1 && s[start] == '0') return false; // disallowed leading zero
    if (value > maxValue) return false;           // out of range
    return true;
}

// Validates that the ENTIRE token s[begin, end) matches
//     octet '.' octet '.' octet '.' octet [ ':' port ]
// No partial matches: every character of the token must be consumed.
bool validateToken(const std::string& s, std::size_t begin, std::size_t end,
                   unsigned long& address, int& port)
{
    std::size_t pos = begin;
    unsigned long addr = 0;

    for (int i = 0; i < 4; ++i) {
        if (i > 0) {
            if (pos >= end || s[pos] != '.') return false;
            ++pos;
        }
        unsigned long octet = 0;
        if (!parseField(s, pos, end, 3, 255, octet)) return false;
        addr = addr * 256 + octet;
    }

    int p = -1;
    if (pos < end) {
        // The only thing allowed after the fourth octet is ':' + a valid port.
        if (s[pos] != ':') return false;          // e.g. a fifth '.'
        ++pos;
        unsigned long portValue = 0;
        if (!parseField(s, pos, end, 5, 65535, portValue)) return false;
        if (pos != end) return false;             // e.g. a second ':' or trailing '.'
        p = static_cast<int>(portValue);
    }

    address = addr;
    port = p;
    return true;
}

} // namespace

bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort)
{
    outAddress = 0;
    outPort = -1;

    std::size_t i = 0;
    const std::size_t n = str.size();

    while (i < n) {
        // Skip garbage.
        if (!isTokenChar(str[i])) {
            ++i;
            continue;
        }

        // Take the maximal run of token characters as one candidate.
        const std::size_t begin = i;
        while (i < n && isTokenChar(str[i])) {
            ++i;
        }

        unsigned long addr = 0;
        int port = -1;
        if (validateToken(str, begin, i, addr, port)) {
            outAddress = addr;
            outPort = port;
            return true;                          // first valid candidate wins
        }
        // Invalid candidate: discard it whole and keep scanning.
    }

    return false;
}

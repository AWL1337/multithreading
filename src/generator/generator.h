#pragma once
#include <random>

const static std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
class Generator {
public:
    Generator();

    int generateInt(int min, int max);
    double generateDouble(double min, double max);
    std::string generateString(size_t length, const std::string& charset = CHARSET);
    std::string generateName();

private:
    std::mt19937 rng;
    std::vector<std::string> names;
};
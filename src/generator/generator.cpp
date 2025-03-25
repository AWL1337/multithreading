#include "generator.h"
#include <ctime>

Generator::Generator()
    : rng(static_cast<unsigned int>(std::time(nullptr))) {
    names = {
        "Aleksandr", "Aleksei", "Andrei", "Anton", "Bogdan", "Vadim", "Vasilii", "Viktor",
        "Vitalii", "Vladimir", "Vladislav", "Gennadii", "Georgii", "Daniil", "Denis", "Dmitrii",
        "Evgenii", "Egor", "Ivan", "Igor", "Kirill", "Konstantin", "Lev", "Maksim",
        "Matvei", "Mikhail", "Nikita", "Nikolai", "Oleg", "Pavel", "Petr", "Roman",
        "Sergei", "Stanislav", "Stepan", "Timofei", "Timur", "Fedor", "Iurii", "Iaroslav",
        "Aleksandra", "Alina", "Anastasiia", "Anna", "Valentina", "Valeriia", "Varvara", "Vasilisa",
        "Veronika", "Viktoriia", "Galina", "Daria", "Diana", "Evgeniia", "Ekaterina", "Elena",
        "Elizaveta", "Zhanna", "Zlata", "Inna", "Irina", "Kira", "Kristina", "Larisa",
        "Lidiia", "Liubov", "Margarita", "Marina", "Mariia", "Nadezhda", "Nataliia", "Oksana",
        "Olesia", "Olga", "Polina", "Svetlana", "Sofiia", "Taisiia", "Tatiana", "Iuliia"
    };
}

int Generator::generateInt(const int min, const int max) {
    std::uniform_int_distribution dist(min, max);
    return dist(rng);
}

double Generator::generateDouble(const double min, const double max) {
    std::uniform_real_distribution dist(min, max);
    return dist(rng);
}

std::string Generator::generateString(const size_t length, const std::string &charset) {
    std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);
    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        result += charset[dist(rng)];
    }

    return result;
}

std::string Generator::generateName() {
    std::uniform_int_distribution<size_t> dist(0, names.size() - 1);
    return names[dist(rng)];
}

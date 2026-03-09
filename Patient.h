#ifndef PATIENT_H
#define PATIENT_H

#include <string>

struct Patient {
    int id;
    std::string name;
    int age;
    std::string medicalHistory;
};

#endif // PATIENT_H

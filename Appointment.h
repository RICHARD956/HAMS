#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

struct Appointment {
    int appointmentId;
    int patientId;
    int doctorId;
    std::string date;
    std::string time;
};

#endif // APPOINTMENT_H

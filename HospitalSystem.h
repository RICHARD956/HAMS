#ifndef HOSPITALSYSTEM_H
#define HOSPITALSYSTEM_H

#include <vector>
#include <string>

#include "Patient.h"
#include "Doctor.h"
#include "Appointment.h"
#include "CryptoUtils.h"  // encryption/hashing helpers

// define USE_OPENSSL when compiling with OpenSSL available

class HospitalSystem {
public:
    HospitalSystem();
    ~HospitalSystem();

    // operations exposed to the UI
    void addPatient();
    void addDoctor();
    void bookAppointment();
    void listPatients() const;
    void listDoctors() const;
    void listAppointments() const;
    void listNotifications() const;
    void cancelAppointment();

    // programmatic helpers (useful for testing)
    void addPatient(const std::string &name, int age);
    void addDoctor(const std::string &name, const std::string &specialty);
    void bookAppointment(int patientId, int doctorId,
                         const std::string &date, const std::string &time);
    void addBilling(int patientId, double amount);
    void listBillings() const;
    void payBilling(int billId);

    // advanced features
    std::vector<Patient> searchPatientsByName(const std::string &name) const;
    std::vector<Doctor> searchDoctorsBySpecialty(const std::string &spec) const;
    bool doctorAvailable(int doctorId, const std::string &date, const std::string &time) const;
    void exportDataCSV(const std::string &filename) const;

    bool authenticateAdmin(const std::string &password) const;
    void changeAdminPassword(const std::string &newPassword);

    // persistence
    void saveData();
    void loadData();

private:
    std::vector<Patient> patients;
    std::vector<Doctor> doctors;
    std::vector<Appointment> appointments;
    int nextPatientId;
    int nextDoctorId;
    int nextAppointmentId;

    // billing records
    struct Billing {
        int billId;
        int patientId;
        double amount;
        bool paid;
    };
    std::vector<Billing> billings;
    int nextBillId;

    // notification storage
    struct Notification {
        int patientId;
        std::string message;
    };
    std::vector<Notification> notifications;

    // admin credentials (stored as sha256 hash)
    std::string adminPasswordHash;

    void pause() const;

    // notification helpers
    void sendNotification(int patientId, const std::string &message);

    // billing helpers (private)
    Billing *findBilling(int billId);

    // helper for export
    std::string escapeCsv(const std::string &s) const;

    // encryption helpers (delegates to CryptoUtils)
    bool encryptString(const std::string &plaintext, std::vector<unsigned char> &ciphertext);
    bool decryptString(const std::vector<unsigned char> &ciphertext, std::string &plaintext);
};

#endif // HOSPITALSYSTEM_H

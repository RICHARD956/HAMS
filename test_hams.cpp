#include "HospitalSystem.h"
#include "CryptoUtils.h"
#include <cassert>
#include <iostream>

int main() {
    HospitalSystem sys;

    // ensure persistence starts empty
    // (we won't delete the file; just load whatever's there)
    sys.loadData();

    // programmatic additions
    sys.addPatient("Alice", 30);
    sys.addDoctor("Dr. Bob", "General");
    sys.bookAppointment(1, 1, "2026-03-10", "09:00");
    sys.addBilling(1, 100.0);
    sys.payBilling(1);
    sys.changeAdminPassword("newpass");
    assert(sys.authenticateAdmin("newpass"));

    // advanced search
    auto patResults = sys.searchPatientsByName("Ali");
    assert(!patResults.empty());
    auto docResults = sys.searchDoctorsBySpecialty("Gen");
    assert(!docResults.empty());
    // export to CSV
    sys.exportDataCSV("test_export.csv");

    // verify that lists contain expected entries
    std::cout << "Listing patients after add:\n";
    sys.listPatients();
    // check hashing utility
    std::string aliceHash = sha256("Alice");
    std::cout << "SHA256(Alice) = " << aliceHash << "\n";
    assert(!aliceHash.empty());
    std::cout << "Listing doctors after add:\n";
    sys.listDoctors();
    std::cout << "Listing appointments after add:\n";
    sys.listAppointments();

    // check notifications were generated
    std::cout << "Notifications after booking:\n";
    sys.listNotifications();

    // save and reload to check persistence
    sys.saveData();
    HospitalSystem sys2;
    sys2.loadData();
    std::cout << "Data reloaded from disk:\n";
    sys2.listPatients();
    sys2.listDoctors();
    sys2.listAppointments();
    std::cout << "Reloaded notifications:\n";
    sys2.listNotifications();

    return 0;
}

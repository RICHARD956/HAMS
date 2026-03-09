#include "HospitalSystem.h"
#include <iostream>
#include <limits>

int main() {
    HospitalSystem sys;
    sys.loadData();

    // require admin login before continuing
    std::string pwd;
    std::cout << "Enter admin password: ";
    std::getline(std::cin, pwd);
    if (!sys.authenticateAdmin(pwd)) {
        std::cout << "Authentication failed, exiting.\n";
        return 1;
    }

    bool running = true;
    while (running) {
        std::cout << "\n--- Hospital Appointment Management System (admin) ---\n";
        std::cout << "1. Add patient\n";
        std::cout << "2. Add doctor\n";
        std::cout << "3. Book appointment\n";
        std::cout << "4. List patients\n";
        std::cout << "5. List doctors\n";
        std::cout << "6. List appointments\n";
        std::cout << "7. Cancel appointment\n";
        std::cout << "8. List notifications\n";
        std::cout << "9. Add billing record\n";
        std::cout << "10. List billings\n";
        std::cout << "11. Pay bill\n";
        std::cout << "12. Change admin password\n";
        std::cout << "13. Search patients\n";
        std::cout << "14. Search doctors\n";
        std::cout << "15. Export data to CSV\n";
        std::cout << "16. Exit\n";
        std::cout << "Choose an option: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
            case 1: sys.addPatient(); break;
            case 2: sys.addDoctor(); break;
            case 3: sys.bookAppointment(); break;
            case 4: sys.listPatients(); break;
            case 5: sys.listDoctors(); break;
            case 6: sys.listAppointments(); break;
            case 7: sys.cancelAppointment(); break;
            case 8: sys.listNotifications(); break;
            case 9: {
                int pid; double amt;
                std::cout << "Patient ID: "; std::cin >> pid;
                std::cout << "Amount: "; std::cin >> amt;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                sys.addBilling(pid, amt);
                break;
            }
            case 10: sys.listBillings(); break;
            case 11: {
                int bid;
                std::cout << "Bill ID: "; std::cin >> bid;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                sys.payBilling(bid);
                break;
            }
            case 12: {
                std::string npw;
                std::cout << "New admin password: ";
                std::getline(std::cin, npw);
                sys.changeAdminPassword(npw);
                break;
            }
            case 13: {
                std::string name;
                std::cout << "Enter patient name fragment: ";
                std::getline(std::cin, name);
                auto results = sys.searchPatientsByName(name);
                std::cout << "Matches:\n";
                for (auto &p : results) {
                    std::cout << "ID:"<<p.id<<" Name:"<<p.name<<" Age:"<<p.age<<"\n";
                }
                break;
            }
            case 14: {
                std::string spec;
                std::cout << "Enter specialty fragment: ";
                std::getline(std::cin, spec);
                auto results = sys.searchDoctorsBySpecialty(spec);
                std::cout << "Matches:\n";
                for (auto &d : results) {
                    std::cout << "ID:"<<d.id<<" Name:"<<d.name<<" Specialty:"<<d.specialty<<"\n";
                }
                break;
            }
            case 15: {
                std::string file;
                std::cout << "File name: ";
                std::getline(std::cin, file);
                sys.exportDataCSV(file);
                break;
            }
            case 16:
                running = false;
                sys.saveData();
                std::cout << "Data saved. Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice.\n";
        }
    }
    return 0;
}

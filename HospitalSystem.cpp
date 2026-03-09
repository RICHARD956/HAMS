#include "HospitalSystem.h"
#include "CryptoUtils.h"
#include <iostream>
#include <limits>
#include <fstream>
#include <sstream>

// NOTE: CryptoUtils encapsulates OpenSSL usage for AES and hashing

using namespace std;

// fixed key/iv for demonstration only (used when OpenSSL enabled)
#ifdef USE_OPENSSL
static const unsigned char AES_KEY[32] = {
    0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
    0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4
};
static const unsigned char AES_IV[16] = {0x00};
#endif

HospitalSystem::HospitalSystem()
    : nextPatientId(1), nextDoctorId(1), nextAppointmentId(1), nextBillId(1) {
    // default admin password is "admin" hashed
    adminPasswordHash = sha256("admin");
}

HospitalSystem::~HospitalSystem() {
}

void HospitalSystem::pause() const {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool HospitalSystem::encryptString(const string &plaintext, vector<unsigned char> &ciphertext) {
    return encryptAES256(plaintext, ciphertext);
}

bool HospitalSystem::decryptString(const vector<unsigned char> &ciphertext, string &plaintext) {
    return decryptAES256(ciphertext, plaintext);
}

void HospitalSystem::addPatient() {
    Patient p;
    p.id = nextPatientId++;
    cout << "Enter patient name: ";
    getline(cin, p.name);
    cout << "Enter patient age: ";
    cin >> p.age;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter medical history (brief): ";
    getline(cin, p.medicalHistory);
    patients.push_back(p);
    cout << "Patient added with ID " << p.id << "\n";
}

void HospitalSystem::addPatient(const std::string &name, int age) {
    Patient p;
    p.id = nextPatientId++;
    p.name = name;
    p.age = age;
    p.medicalHistory = "";
    patients.push_back(p);
}

void HospitalSystem::addDoctor() {
    Doctor d;
    d.id = nextDoctorId++;
    cout << "Enter doctor name: ";
    getline(cin, d.name);
    cout << "Enter specialty: ";
    getline(cin, d.specialty);
    doctors.push_back(d);
    cout << "Doctor added with ID " << d.id << "\n";
}

void HospitalSystem::addDoctor(const std::string &name, const std::string &specialty) {
    Doctor d;
    d.id = nextDoctorId++;
    d.name = name;
    d.specialty = specialty;
    doctors.push_back(d);
}

void HospitalSystem::bookAppointment() {
    if (patients.empty() || doctors.empty()) {
        cout << "Need at least one patient and one doctor to book appointments.\n";
        return;
    }
    int pid, did;
    cout << "Enter patient ID: ";
    cin >> pid;
    cout << "Enter doctor ID: ";
    cin >> did;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    bool validP = false;
    bool validD = false;
    for (auto &p : patients) if (p.id == pid) validP = true;
    for (auto &d : doctors) if (d.id == did) validD = true;

    if (!validP || !validD) {
        cout << "Invalid patient or doctor ID.\n";
        return;
    }

    Appointment a;
    a.appointmentId = nextAppointmentId++;
    a.patientId = pid;
    a.doctorId = did;
    cout << "Enter date (YYYY-MM-DD): ";
    getline(cin, a.date);
    cout << "Enter time (HH:MM): ";
    getline(cin, a.time);
    appointments.push_back(a);
    cout << "Appointment booked with ID " << a.appointmentId << "\n";

    // send notification to patient about the new appointment
    std::ostringstream msg;
    msg << "Your appointment (ID " << a.appointmentId << ") with doctor " << did
        << " is scheduled for " << a.date << " at " << a.time << ".";
    sendNotification(pid, msg.str());
}

void HospitalSystem::bookAppointment(int patientId, int doctorId,
                                    const string &date, const string &time) {
    // assume caller has validated ids and availability
    if (!doctorAvailable(doctorId, date, time)) {
        cout << "Doctor not available at that time.\n";
        return;
    }
    Appointment a;
    a.appointmentId = nextAppointmentId++;
    a.patientId = patientId;
    a.doctorId = doctorId;
    a.date = date;
    a.time = time;
    appointments.push_back(a);
}
void HospitalSystem::listPatients() const {
    cout << "Patients:\n";
    for (auto &p : patients) {
        std::string hash = sha256(p.name);
        cout << "ID:" << p.id << " Name:" << p.name << " Age:" << p.age
             << " History:" << p.medicalHistory
             << " Hash:" << hash << "\n";
    }
}

void HospitalSystem::listDoctors() const {
    cout << "Doctors:\n";
    for (auto &d : doctors) {
        cout << "ID:" << d.id << " Name:" << d.name << " Specialty:" << d.specialty << "\n";
    }
}

void HospitalSystem::listAppointments() const {
    cout << "Appointments:\n";
    for (auto &a : appointments) {
        cout << "ApptID:" << a.appointmentId << " PatientID:" << a.patientId
             << " DoctorID:" << a.doctorId << " Date:" << a.date << " Time:" << a.time << "\n";
    }
}

std::vector<Patient> HospitalSystem::searchPatientsByName(const std::string &name) const {
    std::vector<Patient> results;
    for (auto &p : patients) {
        if (p.name.find(name) != std::string::npos) results.push_back(p);
    }
    return results;
}

std::vector<Doctor> HospitalSystem::searchDoctorsBySpecialty(const std::string &spec) const {
    std::vector<Doctor> results;
    for (auto &d : doctors) {
        if (d.specialty.find(spec) != std::string::npos) results.push_back(d);
    }
    return results;
}

bool HospitalSystem::doctorAvailable(int doctorId, const std::string &date, const std::string &time) const {
    for (auto &a : appointments) {
        if (a.doctorId == doctorId && a.date == date && a.time == time) return false;
    }
    return true;
}

std::string HospitalSystem::escapeCsv(const std::string &s) const {
    std::string out;
    bool needQuote = s.find(',') != std::string::npos || s.find('"') != std::string::npos;
    if (needQuote) out += '"';
    for (char c : s) {
        if (c == '"') out += """";
        else out += c;
    }
    if (needQuote) out += '"';
    return out;
}

void HospitalSystem::exportDataCSV(const std::string &filename) const {
    std::ofstream fout(filename);
    if (!fout) {
        cout << "Unable to open " << filename << " for writing.\n";
        return;
    }
    fout << "Patients\n";
    fout << "ID,Name,Age,History\n";
    for (auto &p : patients) {
        fout << p.id << "," << escapeCsv(p.name) << "," << p.age << "," << escapeCsv(p.medicalHistory) << "\n";
    }
    fout << "\nDoctors\n";
    fout << "ID,Name,Specialty\n";
    for (auto &d : doctors) {
        fout << d.id << "," << escapeCsv(d.name) << "," << escapeCsv(d.specialty) << "\n";
    }
    fout << "\nAppointments\n";
    fout << "ApptID,PatientID,DoctorID,Date,Time\n";
    for (auto &a : appointments) {
        fout << a.appointmentId << "," << a.patientId << "," << a.doctorId << "," << a.date << "," << a.time << "\n";
    }
    fout.close();
    cout << "Data exported to " << filename << "\n";
}

void HospitalSystem::sendNotification(int patientId, const std::string &message) {
    // store notification and also print it to console to simulate delivery
    Notification n{patientId, message};
    notifications.push_back(n);
    // look up patient name for nicer output
    std::string name = "(unknown)";
    for (auto &p : patients) {
        if (p.id == patientId) {
            name = p.name;
            break;
        }
    }
    cout << "[Notification] to " << name << " (ID " << patientId << "): " << message << "\n";
}

void HospitalSystem::addBilling(int patientId, double amount) {
    Billing b{nextBillId++, patientId, amount, false};
    billings.push_back(b);
    cout << "Created bill ID " << b.billId << " for patient " << patientId << " amount " << amount << "\n";
}

void HospitalSystem::listBillings() const {
    cout << "Billing records:\n";
    for (auto &b : billings) {
        cout << "BillID:" << b.billId << " PatientID:" << b.patientId
             << " Amount:" << b.amount << " Paid:" << (b.paid?"yes":"no") << "\n";
    }
}

HospitalSystem::Billing *HospitalSystem::findBilling(int billId) {
    for (auto &b : billings) {
        if (b.billId == billId) return &b;
    }
    return nullptr;
}

void HospitalSystem::payBilling(int billId) {
    Billing *b = findBilling(billId);
    if (!b) {
        cout << "Bill not found.\n";
        return;
    }
    if (b->paid) {
        cout << "Bill already paid.\n";
        return;
    }
    b->paid = true;
    cout << "Bill " << billId << " marked as paid.\n";
}

bool HospitalSystem::authenticateAdmin(const std::string &password) const {
    return sha256(password) == adminPasswordHash;
}

void HospitalSystem::changeAdminPassword(const std::string &newPassword) {
    adminPasswordHash = sha256(newPassword);
    cout << "Admin password changed.\n";
}

void HospitalSystem::listNotifications() const {
    cout << "Notifications:\n";
    for (auto &n : notifications) {
        cout << "PatientID:" << n.patientId << " Message:" << n.message << "\n";
    }
}

void HospitalSystem::cancelAppointment() {
    int aid;
    cout << "Enter appointment ID to cancel: ";
    cin >> aid;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for (auto it = appointments.begin(); it != appointments.end(); ++it) {
        if (it->appointmentId == aid) {
            int pid = it->patientId;
            appointments.erase(it);
            cout << "Appointment " << aid << " canceled.\n";

            // notify patient
            std::ostringstream msg;
            msg << "Your appointment (ID " << aid << ") has been canceled.";
            sendNotification(pid, msg.str());
            // create billing for cancellation fee
            addBilling(pid, 20.0);
            return;
        }
    }
    cout << "Appointment ID not found.\n";
}

void HospitalSystem::saveData() {
    ostringstream oss;
    oss << nextPatientId << "\n" << nextDoctorId << "\n" << nextAppointmentId << "\n" << nextBillId << "\n";
    oss << patients.size() << "\n";
    for (auto &p : patients) {
        oss << p.id << '\n' << p.name << '\n' << p.age << '\n' << p.medicalHistory << '\n';
    }
    oss << doctors.size() << "\n";
    for (auto &d : doctors) {
        oss << d.id << '\n' << d.name << '\n' << d.specialty << '\n';
    }
    oss << appointments.size() << "\n";
    for (auto &a : appointments) {
        oss << a.appointmentId << '\n' << a.patientId << '\n' << a.doctorId << '\n'
            << a.date << '\n' << a.time << '\n';
    }
    // save billing
    oss << billings.size() << "\n";
    for (auto &b : billings) {
        oss << b.billId << '\n' << b.patientId << '\n' << b.amount << '\n' << b.paid << "\n";
    }
    // save notifications as well
    oss << notifications.size() << "\n";
    for (auto &n : notifications) {
        oss << n.patientId << '\n' << n.message << "\n";
    }
    // save admin password hash
    oss << adminPasswordHash << "\n";

    string plaintext = oss.str();

    vector<unsigned char> encrypted;
    if (!encryptString(plaintext, encrypted)) {
        cerr << "Error encrypting data.\n";
        return;
    }
    ofstream fout("hams_data.bin", ios::binary);
    if (!fout) return;
    fout.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
    fout.close();
}

void HospitalSystem::loadData() {
    ifstream fin("hams_data.bin", ios::binary);
    if (!fin) return;
    vector<unsigned char> encrypted((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    fin.close();
    if (encrypted.empty()) return;
    string plaintext;
    if (!decryptString(encrypted, plaintext)) {
        cerr << "Error decrypting data.\n";
        return;
    }
    istringstream iss(plaintext);
    size_t size;
    iss >> nextPatientId >> nextDoctorId >> nextAppointmentId >> nextBillId;
    iss >> size;
    patients.clear();
    for (size_t i = 0; i < size; ++i) {
        Patient p;
        iss >> p.id;
        iss.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(iss, p.name);
        iss >> p.age;
        iss.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(iss, p.medicalHistory);
        patients.push_back(p);
    }
    iss >> size;
    doctors.clear();
    for (size_t i = 0; i < size; ++i) {
        Doctor d;
        iss >> d.id;
        iss.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(iss, d.name);
        getline(iss, d.specialty);
        doctors.push_back(d);
    }
    iss >> size;
    appointments.clear();
    for (size_t i = 0; i < size; ++i) {
        Appointment a;
        iss >> a.appointmentId >> a.patientId >> a.doctorId;
        iss.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(iss, a.date);
        getline(iss, a.time);
        appointments.push_back(a);
    }
    // load billing
    iss >> size;
    billings.clear();
    for (size_t i = 0; i < size; ++i) {
        Billing b;
        iss >> b.billId >> b.patientId >> b.amount >> b.paid;
        billings.push_back(b);
    }
    // load notifications
    iss >> size;
    notifications.clear();
    for (size_t i = 0; i < size; ++i) {
        Notification n;
        iss >> n.patientId;
        iss.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(iss, n.message);
        notifications.push_back(n);
    }
    // load admin hash
    iss.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(iss, adminPasswordHash);
}

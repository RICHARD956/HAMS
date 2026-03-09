# Hospital Appointment Management System (HAMS)

A simple C++ console application that allows adding patients/doctors and booking appointments, with data persisted to an encrypted file using OpenSSL.

## Features

- Add/list patients and doctors (with medical history)
- Book, list, and cancel appointments with doctor availability checking
- Search patients by name and doctors by specialty
- Billing records with ability to create, list, and mark bills paid
- Admin authentication with password hashing and change support
- Notifications for appointment events
- Export entire dataset to CSV for reporting
- Data saved to `hams_data.bin` with AES-256-CBC encryption (when OpenSSL available)
- Modular project structure with headers and source files

## Prerequisites

- C++17-compatible compiler (g++ / clang++ / MSVC)
- OpenSSL installed (headers and `libcrypto` library)

## Building

A `Makefile` is provided for convenience. You might need to set `OPENSSL_INC` and `OPENSSL_LIB` environment variables or edit the Makefile to point at your install:

```sh
cd c:\Users\USER\Desktop\HAMS
# example if OpenSSL is installed in C:\OpenSSL-Win64
set OPENSSL_INC=C:\OpenSSL-Win64\include
set OPENSSL_LIB=C:\OpenSSL-Win64\lib
make
```

Alternatively, compile manually:

```sh
g++ -std=c++17 main.cpp HospitalSystem.cpp -o HAMS.exe \
    -I "C:\Path\To\OpenSSL\include" \
    -L "C:\Path\To\OpenSSL\lib" -lcrypto
```

For Visual Studio Code, use the provided `.vscode/tasks.json` to build with `Ctrl+Shift+B`.

## Running

```sh
./HAMS.exe
```

> On Windows: `HAMS.exe`

## Notes

- The encryption key and IV in this example are hardcoded for demonstration. In production, obtain them securely.
- Ensure `libcrypto` DLL is on your `PATH` or next to the executable.

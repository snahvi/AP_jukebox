# Qt Login System

A complete login system built with Qt framework that includes user registration, login, and password recovery functionality.

## Features

- **User Registration**: Create new accounts with username, password, and security question
- **User Login**: Secure login with username and password
- **Password Recovery**: Reset password using security question
- **Secure Password Storage**: Passwords are hashed using SHA-256
- **Persistent Data**: User data is stored in `users.txt` file

## Building the Project

### Prerequisites
- Qt 5 or Qt 6
- CMake 3.16 or higher
- C++ compiler with C++17 support

### Build Steps
1. Navigate to the login directory
2. Create a build directory: `mkdir build && cd build`
3. Configure with CMake: `cmake ..`
4. Build the project: `cmake --build .`

## Usage

### Running the Application
After building, run the executable:
- Linux/macOS: `./login`
- Windows: `login.exe`

### Main Features

1. **Main Login Page**: 
   - Click "Sign Up" to create a new account
   - Click "Sign In" to log into an existing account

2. **Registration**:
   - Enter username, password, and security answer (favorite city)
   - Password must be at least 6 characters
   - Username must be unique

3. **Login**:
   - Enter username and password
   - Click "forgot password" to reset password

4. **Password Recovery**:
   - Enter username and security answer
   - Enter new password when prompted

## File Structure

- `main.cpp` - Application entry point
- `loginpage.h/cpp/ui` - Main login window
- `signin.h/cpp/ui` - Login dialog
- `signuppage.h/cpp/ui` - Registration dialog
- `forgotpassword.h/cpp/ui` - Password recovery dialog
- `accountmanager.h/cpp` - Backend user management
- `user.h/cpp` - User data model
- `users.txt` - User data storage file

## Security Features

- Passwords are hashed using SHA-256 before storage
- Security answers are also hashed
- Input validation for all fields
- Secure password entry (hidden characters)

## Data Storage

User data is stored in `users.txt` in the following format:
```
username::hashedPassword::hashedSecurityAnswer
```

The file is automatically created when the first user registers. 
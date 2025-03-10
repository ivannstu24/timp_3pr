#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "httplib.h"

using namespace std;

struct User {
    string username;
    string password;
};

struct PasswordService {
    string service;
    string password;
};

unordered_map<string, vector<PasswordService>> passwordsDB;
vector<User> users;

void load_users() {
    ifstream file("/Users/ivanmerzov/Desktop/PasswordManager/PasswordManager/user.csv");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string username, password;
        getline(ss, username, ',');
        getline(ss, password, ',');
        users.push_back({username, password});
    }
}

void save_user(const string& username, const string& password) {
    ofstream file("/Users/ivanmerzov/Desktop/PasswordManager/PasswordManager/user.csv", ios::app);
    file << username << "," << password << "\n";
}

bool user_exists(const string& username) {
    for (const auto& user : users) {
        if (user.username == username){
            return true;
        }
    }
    return false;
}

bool check_credentials(const string& username, const string& password) {
    for (const auto& user : users) {
        if (user.username == username && user.password == password){
            return true;
        }
    }
    return false;
}

void load_passwords() {
    ifstream file("/Users/ivanmerzov/Desktop/PasswordManager/PasswordManager/passwords.csv");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string username, service, password;
        getline(ss, username, ',');
        getline(ss, service, ',');
        getline(ss, password, ',');
        passwordsDB[username].push_back({service, password});
    }
}

void save_password(const string& username, const string& service, const string& password) {
    ofstream file("/Users/ivanmerzov/Desktop/PasswordManager/PasswordManager/passwords.csv", ios::app);
    file << username << "," << service << "," << password << "\n";
    passwordsDB[username].push_back({service, password});
}

int main() {
    load_users();
    load_passwords();
    httplib::Server svr;
    svr.set_base_dir("/Users/ivanmerzov/Desktop/PasswordManager/PasswordManager/static");
    svr.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        string username = req.get_param_value("username");
        string password = req.get_param_value("password");

        if (user_exists(username)) {
            res.status = 409;
            res.set_content("User already exists", "text/plain");
            return;
        }
        save_user(username, password);
        users.push_back({username, password});
        res.set_content("Registration successful", "text/plain");
    });

    svr.Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        string username = req.get_param_value("username");
        string password = req.get_param_value("password");

        if (check_credentials(username, password)) {
            res.set_content("Login successful", "text/plain");
        } else {
            res.status = 401;
            res.set_content("Invalid credentials", "text/plain");
        }
    });

    svr.Post("/save_password", [](const httplib::Request& req, httplib::Response& res) {
        string username = req.get_param_value("username");
        string service = req.get_param_value("service");
        string password = req.get_param_value("password");
        if (username.empty()) {
            res.status = 403;
            res.set_content("Unauthorized", "text/plain");
            return;
        }
        save_password(username, service, password);
        res.set_content("Password saved", "text/plain");
    });

    svr.Get("/get_passwords", [](const httplib::Request& req, httplib::Response& res) {
        string username = req.get_param_value("username");

        if (username.empty() || passwordsDB.find(username) == passwordsDB.end()) {
            res.status = 403;
            res.set_content("Не авторизован или нет паролей", "text/plain");
            return;
        }
        ostringstream response;
        for (const auto& entry : passwordsDB[username]) {
            response << entry.service << ": " << entry.password << "\n";
        }
        res.set_content(response.str(), "text/plain");
    });
    svr.listen("localhost", 8080);
}


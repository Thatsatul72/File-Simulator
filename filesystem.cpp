#include <iostream>
#include <map>
#include <vector>
#include <string>
using namespace std;

// File class to store file details
class File {
public:
    string name;
    string content;

    File(string name) : name(name), content("") {}

    void writeContent(const string& data) {
        content += data;
    }

    void readContent() const {
        if (content.empty()) {
            cout << "File is empty.\n";
        } else {
            cout << "Content of " << name << ": " << content << "\n";
        }
    }
};

// Directory class to store directories and files
class Directory {
public:
    string name;
    map<string, File*> files;
    map<string, Directory*> subdirectories;

    Directory(string name) : name(name) {}

    ~Directory() {
        for (auto& pair : files) delete pair.second;
        for (auto& pair : subdirectories) delete pair.second;
    }

    void createFile(const string& fileName) {
        if (files.find(fileName) != files.end()) {
            cout << "File already exists.\n";
        } else {
            files[fileName] = new File(fileName);
            cout << "File " << fileName << " created.\n";
        }
    }

    void deleteFile(const string& fileName) {
        if (files.find(fileName) == files.end()) {
            cout << "File not found.\n";
        } else {
            delete files[fileName];
            files.erase(fileName);
            cout << "File " << fileName << " deleted.\n";
        }
    }

    void createDirectory(const string& dirName) {
        if (subdirectories.find(dirName) != subdirectories.end()) {
            cout << "Directory already exists.\n";
        } else {
            subdirectories[dirName] = new Directory(dirName);
            cout << "Directory " << dirName << " created.\n";
        }
    }

    void listContents() const {
        cout << "Directory: " << name << "\n";
        cout << "Files:\n";
        for (const auto& pair : files) {
            cout << "  " << pair.first << "\n";
        }
        cout << "Subdirectories:\n";
        for (const auto& pair : subdirectories) {
            cout << "  " << pair.first << "\n";
        }
    }

    Directory* navigateTo(const string& dirName) {
        if (subdirectories.find(dirName) != subdirectories.end()) {
            return subdirectories[dirName];
        } else {
            cout << "Directory not found.\n";
            return nullptr;
        }
    }
};

// Main function to interact with the simulated file system
int main() {
    Directory root("root");
    Directory* currentDir = &root;
    string command;

    cout << "File System Simulation\n";
    cout << "Commands: mkdir <name>, touch <name>, rm <name>, ls, cd <name>, back, write <file> <data>, read <file>, exit\n";

    while (true) {
        cout << currentDir->name << "> ";
        getline(cin, command);
        string cmd, arg1, arg2;
        size_t pos = command.find(' ');
        if (pos != string::npos) {
            cmd = command.substr(0, pos);
            command.erase(0, pos + 1);
            pos = command.find(' ');
            if (pos != string::npos) {
                arg1 = command.substr(0, pos);
                arg2 = command.substr(pos + 1);
            } else {
                arg1 = command;
            }
        } else {
            cmd = command;
        }

        if (cmd == "mkdir") {
            currentDir->createDirectory(arg1);
        } else if (cmd == "touch") {
            currentDir->createFile(arg1);
        } else if (cmd == "rm") {
            currentDir->deleteFile(arg1);
        } else if (cmd == "ls") {
            currentDir->listContents();
        } else if (cmd == "cd") {
            Directory* newDir = currentDir->navigateTo(arg1);
            if (newDir) currentDir = newDir;
        } else if (cmd == "back") {
            if (currentDir != &root) currentDir = &root;  // Simplified, only navigates back to root
            else cout << "Already at root directory.\n";
        } else if (cmd == "write") {
            if (currentDir->files.find(arg1) != currentDir->files.end()) {
                currentDir->files[arg1]->writeContent(arg2);
            } else {
                cout << "File not found.\n";
            }
        } else if (cmd == "read") {
            if (currentDir->files.find(arg1) != currentDir->files.end()) {
                currentDir->files[arg1]->readContent();
            } else {
                cout << "File not found.\n";
            }
        } else if (cmd == "exit") {
            break;
        } else {
            cout << "Unknown command.\n";
        }
    }

    return 0;
}

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <bitset>
using namespace std;

// File class with permissions
class File {
public:
    string name;
    string content;
    bitset<3> permissions; // Permissions: 0 -> execute, 1 -> write, 2 -> read

    File(string name) : name(name), content(""), permissions(0b111) {}

    void writeContent(const string& data) {
        if (permissions[1]) { // Check write permission
            content += data;
            cout << "Content written to " << name << ".\n";
        } else {
            cout << "Write permission denied for file " << name << ".\n";
        }
    }

    void readContent() const {
        if (permissions[2]) { // Check read permission
            if (content.empty()) {
                cout << "File is empty.\n";
            } else {
                cout << "Content of " << name << ": " << content << "\n";
            }
        } else {
            cout << "Read permission denied for file " << name << ".\n";
        }
    }

    void setPermissions(const string& perm) {
        if (perm.size() == 3) {
            permissions = bitset<3>(perm);
            cout << "Permissions set to " << permissions << " for file " << name << ".\n";
        } else {
            cout << "Invalid permission format. Use a 3-bit string (e.g., 111).\n";
        }
    }
};

// Directory class to manage subdirectories and files
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
            cout << "  " << pair.first << " [Permissions: " << pair.second->permissions << "]\n";
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

// Persistent storage helper functions
void saveFileSystem(Directory* root, const string& path) {
    ofstream file(path);
    if (!file.is_open()) {
        cout << "Failed to save the file system.\n";
        return;
    }

    function<void(Directory*, string)> save = [&](Directory* dir, string dirPath) {
        for (const auto& pair : dir->files) {
            file << "FILE " << dirPath + "/" + pair.first << " " << pair.second->permissions << " " << pair.second->content << "\n";
        }
        for (const auto& pair : dir->subdirectories) {
            file << "DIR " << dirPath + "/" + pair.first << "\n";
            save(pair.second, dirPath + "/" + pair.first);
        }
    };

    save(root, "root");
    file.close();
    cout << "File system saved.\n";
}

void loadFileSystem(Directory* root, const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cout << "No saved file system found.\n";
        return;
    }

    map<string, Directory*> dirMap;
    dirMap["root"] = root;

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string type, fullPath, permissions, content;
        iss >> type >> fullPath;
        size_t pos = fullPath.find_last_of('/');
        string parentPath = fullPath.substr(0, pos);
        string name = fullPath.substr(pos + 1);

        if (type == "DIR") {
            dirMap[fullPath] = new Directory(name);
            dirMap[parentPath]->subdirectories[name] = dirMap[fullPath];
        } else if (type == "FILE") {
            iss >> permissions;
            getline(iss, content);
            File* newFile = new File(name);
            newFile->permissions = bitset<3>(permissions);
            newFile->content = content.empty() ? "" : content.substr(1); // Remove leading space
            dirMap[parentPath]->files[name] = newFile;
        }
    }

    file.close();
    cout << "File system loaded.\n";
}

// Main function
int main() {
    Directory root("root");
    Directory* currentDir = &root;
    string command;

    // Load file system
    loadFileSystem(&root, "filesystem_data.txt");

    cout << "Extended File System Simulation\n";
    cout << "Commands: mkdir <name>, touch <name>, rm <name>, ls, cd <name>, back, write <file> <data>, read <file>, chmod <file> <perm>, save, exit\n";

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
            if (currentDir != &root) currentDir = &root; // Simplified: back to root
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
        } else if (cmd == "chmod") {
            if (currentDir->files.find(arg1) != currentDir->files.end()) {
                currentDir->files[arg1]->setPermissions(arg2);
            } else {
                cout << "File not found.\n";
            }
        } else if (cmd == "save") {
            saveFileSystem(&root, "filesystem_data.txt");
        } else if (cmd == "exit") {
            saveFileSystem(&root, "filesystem_data.txt");
            break;
        } else {
            cout << "Unknown command.\n";
        }
    }

    return 0;
}

/*root> mkdir projects
Directory projects created.

root> cd projects
projects> 

projects> touch file1.txt
File file1.txt created.

projects> write file1.txt Hello
Content written to file1.txt.

projects> read file1.txt
Content of file1.txt: Hello

projects> ls
Directory: projects
Files:
  file1.txt [Permissions: 111]
Subdirectories:

projects> back
root> 

root> ls
Directory: root
Files:
Subdirectories:
  projects

root> chmod projects/file1.txt 110
Permissions set to 110 for file projects/file1.txt.

root> cd projects
projects> write file1.txt MoreData
Write permission denied for file file1.txt.

projects> read file1.txt
Content of file1.txt: Hello

projects> chmod file1.txt 111
Permissions set to 111 for file file1.txt.

projects> write file1.txt MoreData
Content written to file1.txt.

projects> read file1.txt
Content of file1.txt: HelloMoreData

projects> save
File system saved.

projects> exit
File system saved.
*/
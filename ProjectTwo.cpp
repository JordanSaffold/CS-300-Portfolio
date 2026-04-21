#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Structure to hold course information
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;

    Course() {
        courseNumber = "";
        courseTitle = "";
    }
};

// Node structure for the binary search tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(const Course& aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Binary Search Tree class
class BinarySearchTree {
private:
    Node* root;

    void addNode(Node* node, const Course& course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            }
            else {
                addNode(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            }
            else {
                addNode(node->right, course);
            }
        }
    }

    void inOrder(Node* node) const {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            inOrder(node->right);
        }
    }

    Course search(Node* node, const string& courseNumber) const {
        if (node == nullptr) {
            return Course();
        }

        if (node->course.courseNumber == courseNumber) {
            return node->course;
        }

        if (courseNumber < node->course.courseNumber) {
            return search(node->left, courseNumber);
        }
        else {
            return search(node->right, courseNumber);
        }
    }

    void destroyTree(Node* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        destroyTree(root);
    }

    void Clear() {
        destroyTree(root);
        root = nullptr;
    }

    void Insert(const Course& course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            addNode(root, course);
        }
    }

    void PrintCourseList() const {
        inOrder(root);
    }

    Course Search(const string& courseNumber) const {
        return search(root, courseNumber);
    }
};

// Trim whitespace from a string
string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }

    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// Convert string to uppercase
string toUpperCase(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return text;
}

// Load course data from file into BST
bool loadCourses(const string& fileName, BinarySearchTree& bst) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: could not open file: " << fileName << endl;
        return false;
    }

    bst.Clear();

    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        string token;
        vector<string> parts;

        while (getline(ss, token, ',')) {
            parts.push_back(trim(token));
        }

        // Each valid course line should have at least course number and title
        if (parts.size() < 2) {
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(parts[0]);
        course.courseTitle = parts[1];

        for (size_t i = 2; i < parts.size(); ++i) {
            if (!parts[i].empty()) {
                course.prerequisites.push_back(toUpperCase(parts[i]));
            }
        }

        bst.Insert(course);
    }

    file.close();
    return true;
}

// Print one course and its prerequisites
void printCourseInformation(const Course& course) {
    if (course.courseNumber.empty()) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            cout << course.prerequisites[i];
            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

int main() {
    BinarySearchTree courseTree;
    int choice = 0;
    bool dataLoaded = false;

    // Tries the most likely file names first
    vector<string> possibleFiles = {
        "ABCU_Advising_Program_Input.csv",
        "CS 300 ABCU_Advising_Program_Input.csv"
    };

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << endl;
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";

        cin >> choice;

        switch (choice) {
        case 1: {
            bool loaded = false;

            // Try known filenames first
            for (size_t i = 0; i < possibleFiles.size(); ++i) {
                if (loadCourses(possibleFiles[i], courseTree)) {
                    loaded = true;
                    dataLoaded = true;
                    cout << "Courses loaded successfully." << endl;
                    break;
                }
            }

            // If those fail, ask user for the exact path
            if (!loaded) {
                cin.ignore();
                string customPath;
                cout << "Enter the file name or full file path: ";
                getline(cin, customPath);

                if (loadCourses(customPath, courseTree)) {
                    loaded = true;
                    dataLoaded = true;
                    cout << "Courses loaded successfully." << endl;
                }
            }

            break;
        }

        case 2: {
            if (!dataLoaded) {
                cout << "Please load the data structure first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                courseTree.PrintCourseList();
            }
            break;
        }

        case 3: {
            if (!dataLoaded) {
                cout << "Please load the data structure first." << endl;
            }
            else {
                string courseKey;
                cout << "What course do you want to know about? ";
                cin >> courseKey;

                courseKey = toUpperCase(courseKey);
                Course foundCourse = courseTree.Search(courseKey);
                printCourseInformation(foundCourse);
            }
            break;
        }

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}
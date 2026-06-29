//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Jamarion Wilson
// Version     : 1.0
// Description : ABCU Advising Assistance Program
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//============================================================================
// Course structure
//============================================================================

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

//============================================================================
// Tree node structure
//============================================================================

struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

//============================================================================
// Binary Search Tree class
//============================================================================

class BinarySearchTree {
private:
    Node* root;

    void addNode(Node* node, Course course) {
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

    void inOrder(Node* node) {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseNumber << ", "
                 << node->course.courseTitle << endl;
            inOrder(node->right);
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

    void Insert(Course course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            addNode(root, course);
        }
    }

    void InOrder() {
        inOrder(root);
    }

    Course Search(string courseNumber) {
        Node* current = root;

        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return current->course;
            }
            else if (courseNumber < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        Course emptyCourse;
        return emptyCourse;
    }

    bool IsEmpty() {
        return root == nullptr;
    }
};

//============================================================================
// Helper functions
//============================================================================

string trim(string value) {
    value.erase(value.begin(), find_if(value.begin(), value.end(),
        [](unsigned char ch) {
            return !isspace(ch);
        }));

    value.erase(find_if(value.rbegin(), value.rend(),
        [](unsigned char ch) {
            return !isspace(ch);
        }).base(), value.end());

    return value;
}

string toUpperCase(string value) {
    transform(value.begin(), value.end(), value.begin(),
        [](unsigned char ch) {
            return toupper(ch);
        });

    return value;
}

// This split function handles both comma-separated and tab-separated files.
vector<string> splitLine(string line) {
    vector<string> tokens;
    string token;

    for (char& ch : line) {
        if (ch == '\t') {
            ch = ',';
        }
    }

    stringstream ss(line);

    while (getline(ss, token, ',')) {
        token = trim(token);

        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

bool courseExists(const vector<string>& courseNumbers, string courseNumber) {
    for (string number : courseNumbers) {
        if (number == courseNumber) {
            return true;
        }
    }

    return false;
}

//============================================================================
// Load course data from file
//============================================================================

bool loadCourses(string fileName, BinarySearchTree& courses) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Could not open file." << endl;
        return false;
    }

    vector<string> lines;
    vector<string> courseNumbers;
    string line;

    while (getline(file, line)) {
        if (!line.empty()) {
            vector<string> tokens = splitLine(line);

            if (tokens.size() < 2) {
                cout << "Error: Invalid file format." << endl;
                file.close();
                return false;
            }

            courseNumbers.push_back(toUpperCase(tokens[0]));
            lines.push_back(line);
        }
    }

    file.close();

    for (string currentLine : lines) {
        vector<string> tokens = splitLine(currentLine);

        Course course;
        course.courseNumber = toUpperCase(tokens[0]);
        course.courseTitle = tokens[1];

        for (unsigned int i = 2; i < tokens.size(); ++i) {
            string prerequisite = toUpperCase(tokens[i]);

            if (!courseExists(courseNumbers, prerequisite)) {
                cout << "Error: Prerequisite " << prerequisite
                     << " does not exist as a course." << endl;
                return false;
            }

            course.prerequisites.push_back(prerequisite);
        }

        courses.Insert(course);
    }

    return true;
}

//============================================================================
// Display course information
//============================================================================

void printCourseInformation(BinarySearchTree& courses, string courseNumber) {
    courseNumber = toUpperCase(courseNumber);

    Course course = courses.Search(courseNumber);

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

        for (unsigned int i = 0; i < course.prerequisites.size(); ++i) {
            cout << course.prerequisites[i];

            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }

        cout << endl;
    }
}

//============================================================================
// Main
//============================================================================

int main() {
    BinarySearchTree courses;
    string fileName;
    string courseNumber;
    int choice = 0;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << endl;
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        }

        switch (choice) {
        case 1:
            cin.ignore(1000, '\n');

            cout << "Enter file name: ";
            getline(cin, fileName);

            courses.Clear();

            if (loadCourses(fileName, courses)) {
                dataLoaded = true;
                cout << "Course data loaded successfully." << endl;
            }
            else {
                dataLoaded = false;
                cout << "Course data failed to load." << endl;
            }
            break;

        case 2:
            if (!dataLoaded) {
                cout << "Please load course data first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                courses.InOrder();
            }
            break;

        case 3:
            if (!dataLoaded) {
                cout << "Please load course data first." << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                cin >> courseNumber;

                printCourseInformation(courses, courseNumber);
            }
            break;

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
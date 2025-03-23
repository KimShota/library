//============================================================================
// Name         : lcms.cpp
// Author       : Shota Matsumoto
// Version      : 1.0
// Date Created : 11/04/2024
// Date Modified: 11/07/2024
// Description  : File that allows for book and category managemet, borrower tracking, and import and export. 
//============================================================================
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <vector>
#include "tree.h"
#include "myvector.h"
#include "borrower.h"
#include "book.h"
#include "lcms.h"

using namespace std;

//Constructor
LCMS::LCMS(string name){
    //Create a library tree
    libTree = new Tree(name);
}

//Deconstructor
LCMS::~LCMS(){
    //Deallocate the memory space for the library tree
    delete libTree;
    //Dynamically deallocate the memory space for borrowers, deleting each borrower 
    for (int i = 0; i < borrowers.size(); i++){
        delete borrowers[i];
    }
}

//Function that imports books from CSV file
int LCMS::import(std::string path) {
    //Open the file at the provided path 
    std::ifstream inputFile(path);

    //If file cannot be opened, display the error message 
    if (!inputFile.is_open()) {
        std::cerr << "We can't open the file you have provided me with, which is " << path << std::endl;
        return -1; //Return -1 if the file cannot be oepned 
    }

    std::string line;
    getline(inputFile, line); //Skip the header line
    int bookCount = 0; //Counter for imported books 

    //Read each line in the given file 
    while (getline(inputFile, line)) {
        //Stream to parse line
        std::istringstream inputString(line);
        //Vector that is going to store parsed fields for each book's attribute
        std::vector<std::string> bookData;
        std::string field;
        bool insideQuotes = false; //Create a flag to handle quoted areas
        //Variable that is going to store the current field 
        std::string currentField;

        //Parse the line character by character to handle fields with some quotes
        for (size_t i = 0; i < line.size(); ++i) {
            char ch = line[i]; 
            if (ch == '\"') {
                insideQuotes = !insideQuotes; //Toggle the state of quote by adding ! 
            } else if (ch == ',' && !insideQuotes) {
                //Add the currentField to bookData if comma is encountered outside quotes
                bookData.push_back(currentField);
                //Clear currentField in order to start a new field
                currentField.clear();
            } else {
                //Add character to current field if it is inside the quotes or is not a comma 
                currentField += ch;
            }
        }
        if (!currentField.empty()) {
            //If not empty, then append the last field to bookData 
            bookData.push_back(currentField);
        }

        //Create an if-statement to check if the line includes the required number of bookData
        if (bookData.size() < 7) {
            std::cerr << "Error has occured. Invalid format in line: " << line << std::endl;
            continue; //Skip this line if it does not have enough fields 
        }

        //Assign each book's atttribute from the bookData
        std::string title = bookData[0];
        std::string author = bookData[1];
        std::string isbn = bookData[2];
        std::string publicationYear = bookData[3];
        std::string category = bookData[4];
        int totalCopies = 0;
        int availableCopies = 0;

        //Change totalCopies and availableCopies to integers
        try {
            totalCopies = std::stoi(bookData[5]);
            availableCopies = std::stoi(bookData[6]);
        } catch (const std::invalid_argument&) {
            std::cerr << "Error has occured. Invalid total or available copies in line: " << line << std::endl;
            continue; //Skip this line if it fails the conversion 
        }

        //Convert publicationYear to a integer
        int pubYearInteger = 0; //Initialize pubYearInteger to 0
        try {
            pubYearInteger = std::stoi(publicationYear);
        } catch (const std::exception&) {
            std::cerr << "Error: Invalid publication year in line: " << line << std::endl;
            continue; //If it fails the conversion then skip this line
        }

        //Create a new Book object with parsed attributes
        Book* newBook = new Book(title, author, isbn, pubYearInteger, totalCopies, availableCopies);
        bookCount++; //Increment the bookcount by 1

        //Process category path so as to add nested categories inside the library tree
        std::stringstream categoryStream(category); //Stream to parse category path
        std::string categoryToken; //String variable to store each cateogry level
        //Create a pointer called currentNode to start from the root of the library tree
        Node* currentNode = libTree->getRoot();

        //Iterate through each category level 
        while (getline(categoryStream, categoryToken, '/')) {
            //Create pointer called childNode to get child node for current category level
            Node* childNode = libTree->getChild(currentNode, categoryToken);
            //If category level does not exist, then make it 
            if (!childNode) {
                libTree->insert(currentNode, categoryToken); //Add new category into the tree
                childNode = libTree->getChild(currentNode, categoryToken); //Get category node that just got newly created 
            }
            //Move to the next level in category path 
            currentNode = childNode;
        }

        //Append the book to the last category node using push_back function from myvector.h
        currentNode->books.push_back(newBook);
        //Update book count
        currentNode->bookCount++; 

        //Update the book count inside all the parent nodes
        Node* parentNode = currentNode->parent;
        while (parentNode) {
            parentNode->bookCount++; //Increment bookCount by one for each parent node
            parentNode = parentNode->parent; //Move to the next parent
        }
    }

    //Display the amount of books that have been imported 
    std::cout << bookCount << " records have been imported successfully." << std::endl;
    inputFile.close(); //Close the file 
    return bookCount; //Return the amount of books 
}

//Function to export all books to the given file
void LCMS::exportData(std::string path) {
    //Open the output file 
    std::ofstream outputFile(path);

    //If the file cannot be opened, then print out the error message 
    if (!outputFile.is_open()) {
        std::cerr << "We can't open the provided file, which is " << path << std::endl;
        return;
    }

    //Write the header row to the file 
    outputFile << "Title,Author,ISBN,Publication Year,Total Copies,Available Copies\n";

    //Create stack to hold nodes in order to traverse the library tree
    MyVector<Node*> nodes_stack;
    //Start from the root node of the library tree
    nodes_stack.push_back(libTree->getRoot());

    //Loop untill we do not find any nodes in the stack 
    while (!nodes_stack.empty()) {
        //Take the last node from the stack 
        Node* currentNode = nodes_stack.back();
        //Remove the last node from the stack 
        nodes_stack.erase(nodes_stack.size() - 1);

        //Iterate through all books in the current node 
        for (size_t i = 0; i < currentNode->books.size(); ++i) {
            //Create a book pointer 
            Book* book = currentNode->books[i];
            //Write the details of the book into the file
            outputFile << book->title << ","
                       << book->author << ","
                       << book->isbn << ","
                       << book->publication_year << ","
                       << book->total_copies << ","
                       << book->available_copies << "\n";
        }

        //Use for loop to push all children of the currentNode onto the stack 
        for (size_t i = 0; i < currentNode->children.size(); ++i) {
            nodes_stack.push_back(currentNode->children[i]);
        }
    }

    //Close the file 
    outputFile.close();
    std::cout << "Data has been exported successfully to: " << path << std::endl;
}

//Function to find all the books in the specified category 
void LCMS::findAll(string category) {
    //Create a node called categoryNode for the specified node 
    Node* categoryNode = libTree->getNode(category);
    //If categoryNode is not found, then
    if (categoryNode == nullptr){
        //Print out an error message indicating that category is not found
        std::cerr << "Category is not found!" << std::endl; 
        //Exit the function
        return; 
    }
    //Display all the books in the category and its subcategories by calling printAll function on specific category 
    libTree->printAll(categoryNode); 
}

//Function to find the book with the specified title 
void LCMS::findBook(string bookTitle){
    //Create a pointer called current that points to the root of the library tree
    Node* current = libTree->getRoot();
    //Declare boolean variable and set it to false so that we can know if the book is found or not 
    bool flag = false;
    
    //Iterate through all the children to search for a book 
    for (int i = 0; i < current->children.size(); i++){
        //Create a book pointer and call the findbook function on each child to search for the book with the specified title 
        Book* book = libTree->findBook(current, bookTitle);
        //If the book is found, then
        if (book){
            //Display the details of the book
            book->display();
            //Set flag to be true
            flag = true;
            //Exit the loop
            break;
        }
    }

    //If no book was found, then print an error message saying that the book was not found
    if (!flag){
        cout << "The book was not found!" << endl;
    }
}

//Function to add a new book to the catalog 
void LCMS::addBook() {
    //Declare variables to store the details of the book 
    std::string title, author, isbn, publicationYear, category; 
    int totalCopies, availableCopies;

    //Ask user input for each detail of the book 
    cout << "Enter Title: ";
    std::getline(cin, title);
    cout << "Enter Author: ";
    std::getline(cin, author);
    cout << "Enter ISBN: ";
    std::getline(cin, isbn);
    cout << "Enter Publication Year: ";
    std::getline(cin, publicationYear);
    cout << "Enter Category: ";
    std::getline(cin, category);
    cout << "Enter Total Copies: ";
    cin >> totalCopies;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  //Clear the newline
    cout << "Enter Available Copies: ";
    cin >> availableCopies;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  //Clear the newline

    //Convert the publication year from string to an integer 
    int pubYearInteger;
    try {
        pubYearInteger = std::stoi(publicationYear);
    } catch (...) {
        //If the conversion fails, then print out an error message indicating that statue and exit the program
        cout << "Invalid publication year entered. Please enter a number." << endl;
        return;
    }

    //Create a book object called newBook with the details inputted by user
    Book* newBook = new Book(title, author, isbn, pubYearInteger, totalCopies, availableCopies);

    //Create a node called categoryNode
    Node* categoryNode = libTree->getNode(category);
    //If the category does not exist, then 
    if (categoryNode == nullptr) {
        //Create a new category 
        cout << "Category '" << category << "' not found. Creating new category." << endl;
        categoryNode = libTree->createNode(category);
    }

    //Append a new book to the book list of the provided category 
    categoryNode->books.push_back(newBook);
    //Update the book count in the library tree
    libTree->updateBookCount(categoryNode, 1);

    cout << title << " has been successfully added to the catalog." << endl;
}

//Function to edit details of the book 
void LCMS::editBook(std::string bookTitle) {
    //Create a node called currentNode to start from the root of the library tree
    Node* currentNode = libTree->getRoot();
    //Create a book object to find the book with the specified title 
    Book* book = libTree->findBook(currentNode, bookTitle);

    //If the book is found then 
    if (book) {
        int choice;
        do {
            //Ask user input for which detail of the book they want to edit 
            cout << "\nWhich detail would you like to edit?" << std::endl;
            cout << "1. Title" << endl;
            cout << "2. Author" << endl;
            cout << "3. ISBN" << endl;
            cout << "4. Publication Year" << endl;
            cout << "5. Total Copies" << endl;
            cout << "6. Available Copies" << endl;
            cout << "7. Exit" << endl;
            cout << "Enter the number of the field you want to edit: ";
            cin >> choice;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  //Clear the input buffer

            //Use switch to handle each situation
            switch (choice) {
                case 1: {
                    std::string newTitle;
                    std::cout << "Enter new title: ";
                    std::getline(std::cin, newTitle); //Obtain new title 
                    if (!newTitle.empty()) book->title = newTitle; //Update the title if not empty 
                    std::cout << "Title is now updated!" << std::endl;
                    break;
                }
                case 2: {
                    std::string newAuthor;
                    std::cout << "Enter new author: ";
                    std::getline(std::cin, newAuthor); //Get the new author's name 
                    if (!newAuthor.empty()) book->author = newAuthor; //Update author detail if not empty 
                    std::cout << "Author is now updated!" << std::endl;
                    break;
                }
                case 3: {
                    std::string newISBN;
                    std::cout << "Enter new ISBN: ";
                    std::getline(std::cin, newISBN); //Get new ISBN 
                    if (!newISBN.empty()) book->isbn = newISBN; //Update ISBN if not empty 
                    std::cout << "ISBN is now updated!" << std::endl;
                    break;
                }
                case 4: {
                    int newPublicationYear;
                    std::cout << "Enter new publication year: ";
                    std::cin >> newPublicationYear; //Get user input for publication year 
                    if (std::cin) book->publication_year = newPublicationYear; 
                    std::cout << "Publication year is now updated!" << std::endl;
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //Clear the input buffer
                    break;
                }
                case 5: {
                    int newTotalCopies;
                    std::cout << "Enter new total copies: ";
                    std::cin >> newTotalCopies; //Get user input for new total copies 
                    if (std::cin) book->total_copies = newTotalCopies;
                    std::cout << "Total copies are now updated!" << std::endl;
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  //Clear the input buffer 
                    break;
                }
                case 6: {
                    int newAvailableCopies;
                    std::cout << "Enter new available copies: ";
                    std::cin >> newAvailableCopies; //Get user input for new available copies 
                    if (std::cin) book->available_copies = newAvailableCopies;
                    std::cout << "Available copies are now updated!" << std::endl;
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  //Clear the input buffer 
                    break;
                }
                case 7: 
                    std::cout << "Exiting the edit menu." << std::endl;
                    break; //Exit the function 
                default: //If the user input is invalid
                    std::cout << "Invalid choice. Please enter a number between 1 and 7." << std::endl;
                    break;
            }
        } while (choice != 7); //Continue until user puts 7 for their input 
    } else {
        //If the book cannot be found then display an error message 
        std::cerr << "Book cannot be found." << std::endl;
    }
}

//Function to borrow books 
void LCMS::borrowBook(string bookTitle) {
    //Declare string variables called name and id 
    string name, id;
    //Create a node called currentNode to start from the root of the library tree
    Node* currentNode = libTree->getRoot();
    //Create a Book objected called book to find the book with the specific title 
    Book* book = libTree->findBook(currentNode, bookTitle);

    //If book is found and there are available copies, then
    if (book && book->available_copies > 0) {  
        //Ask user input for their name and id 
        cout << "Enter Borrower's name: ";
        getline(cin, name);
        cout << "Enter Borrower's id: ";
        getline(cin, id);

        //Create a Borrower object called borrower and set it to nullptr 
        Borrower* borrower = nullptr;
        //Iterate through each borrower to see if the borrower already exists or not
        for (int i = 0; i < borrowers.size(); i++) {
            //If borrower's name and id matches the user input, then
            if (borrowers[i]->name == name && borrowers[i]->id == id) {
                //Set the pointer to the given borrower and exit the loop 
                borrower = borrowers[i];
                break;
            }
        }

        //If borrower does not exist, then
        if (!borrower) {
            //Create a new borrower with name and id
            borrower = new Borrower(name, id);
            //Add the newly created borrower to the list of borrowers
            borrowers.push_back(borrower);
        }

        //Add the borrower to the specified book's list of current borrowers
        book->currentBorrowers.push_back(borrower);
        //Add the book to the borrower's list of books that they have already borrowed
        borrower->books_borrowed.push_back(book);
        //Decrement the available copies by one
        book->available_copies--;

        cout << "Book '" << bookTitle << "' has been successfully issued to " << name << " (ID: " << id << ")." << endl;
    } else {
        //If there is no such book or no available copies, then display an error message indicating that status 
        cerr << "Book not found or no copies available!" << endl;
    }
}

//Function to return book
void LCMS::returnBook(string bookTitle) {
    //Create a new node called currentnode to start from the root of the library tree
    Node* currentNode = libTree->getRoot();
    //Create a Book objected called book to find the book with the specific title
    Book* book = libTree->findBook(currentNode, bookTitle);

    //If the book is found, then
    if (book) {
        //Ask user input for name and id 
        string name;
        string id;
        cout << "Enter borrower's name: ";
        getline(cin, name);
        cout << "Enter borrower's id: ";
        getline(cin, id);

        //Create a flag to check if the borrower was found or not
        bool flag = false;  

        //Search for the borrower in the current borrowers list for the book
        for (int i = 0; i < book->currentBorrowers.size(); i++) {
            Borrower* borrower = book->currentBorrowers[i];
            
            //If name and id match, then
            if (borrower->name == name && borrower->id == id) {
                //Remove the borrower from the book's list of current borrowers by calling erase function 
                book->currentBorrowers.erase(i);

                //Increment the available copies of the book by one
                book->available_copies++;

                //Use for loop to find and remove the book from the borrower's books_borrowed list
                for (int j = 0; j < borrower->books_borrowed.size(); j++) {
                    if (borrower->books_borrowed[j] == book) {
                        borrower->books_borrowed.erase(j);
                        break;
                    }
                }

                cout << "Book has been successfully returned." << endl;
                flag = true;  //Set the flag to true to indicate that the book has been successfully returned. 
                break;
            }
        }

        //If borrower information doesn't match, display an error message
        if (!flag) {
            cerr << "Borrower's information does not match any current borrower for this book." << endl;
        }
    } else {
        cerr << "Book cannot be found!" << endl;
    }
}

//Function to list all the current borrowers in the list of the specified book 
void LCMS::listCurrentBorrowers(string bookTitle) {
    //Create a node to start from the root of the tree
    Node* root = libTree->getRoot();
    //Create a book object to find a book with specified title
    Book* book = libTree->findBook(root, bookTitle);
    
    //If book is found, then
    if (book) {
        //Iterate through each borrower and print their names and ids 
        for (int i = 0; i < book->currentBorrowers.size(); i++) {
            cout << i << " " << book->currentBorrowers[i]->name << " (ID: " << book->currentBorrowers[i]->id << ")" << endl;
        }
    } else {
        //If not, then print out an error message 
        cout << "Book cannot be found!" << endl;
    }
}

//Function to display all the borrowers including the past ones of the specified book 
void LCMS::listAllBorrowers(string bookTitle) {
    //Create a node to start from the root of the tree
    Node* root = libTree->getRoot();
    //Create a book object to find a book with specified title
    Book* book = libTree->findBook(root, bookTitle);
    
    //If book is found, then
    if (book) {
        cout << "All borrowers of " << bookTitle << ":" << endl;
        //Iterate through each borrower and display their names and ids 
        for (int i = 0; i < book->allBorrowers.size(); i++) {
            cout << book->allBorrowers[i]->name << " (ID: " << book->allBorrowers[i]->id << ")" << endl;
        }
    } else {
        //If not, then print out an error message
        cout << "Book cannot be found!" << endl;
    }
}

//Function to display all the books that a specified borrower borrows
void LCMS::listBooks(string borrower_name_id) {
    //Parse the input string to separate name and id
    stringstream ss(borrower_name_id);
    string name, id;
    getline(ss, name, ',');
    getline(ss, id);

    //Eliminate any whitespace from name and id
    name.erase(0, name.find_first_not_of(" \t\n\r"));
    name.erase(name.find_last_not_of(" \t\n\r") + 1);
    id.erase(0, id.find_first_not_of(" \t\n\r"));
    id.erase(id.find_last_not_of(" \t\n\r") + 1);

    cout << "Books borrowed by " << name << " (ID: " << id << ") are listed below:" << endl;

    //Initialize and set the flag to false
    bool flag = false;
    //Iterate through each borrower in the list 
    for (int i = 0; i < borrowers.size(); i++) {
        //if borrower's name and if mathces the user input, then
        if (borrowers[i]->name == name && borrowers[i]->id == id) {
            //List all the books that they borrowed
            borrowers[i]->listBooks();
            //Set the flag to be true to indicate that the specified borrower exists
            flag = true;
            //Exit the loop 
            break;
        }
    }

    //If the specified borrower does not exist, then display an error message 
    if (!flag) {
        cerr << "Borrower with name '" << name << "' and ID '" << id << "' cannot be found!" << endl;
    }
}


//Function to remove the book from catalog 
void LCMS::removeBook(string bookTitle) {
    //Create a node called currentNode to start from the root of the tree
    Node* currentNode = libTree->getRoot();

    //Create a Book object called book to search for the specific book in the library tree
    Book* book = libTree->findBook(currentNode, bookTitle);

    //If book is found, 
    if (book) {
        //Ask for confirmation
        string confirm;
        cout << "Are you sure you want to delete the book '" << bookTitle << "' from the catalog? (yes/no): ";
        getline(cin, confirm);

        //If user input is yes, then
        if (confirm == "yes") {
            //Initialize stack 
            MyVector<Node*> stack;
            //Add root node to the stack 
            stack.push_back(currentNode);

            //Initialize flag to false
            bool flag = false;

            //Create a while loop to locate and remove book
            while (!stack.empty() && !flag) {
                //Remove the last node from the stack 
                Node* node = stack[stack.size() - 1];
                stack.erase(stack.size() - 1);

                //Use for loop to search for the book in the node's list of the books
                for (int i = 0; i < node->books.size(); i++) {
                    //If title matches then
                    if (node->books[i]->title == bookTitle) {
                        //Deelte the book object and remove it from the books vector as well
                        delete node->books[i];
                        node->books.erase(i);
                        flag = true; //Set the flag to be true to indicate that the elimination of the book succeeded

                        Node* parentNode = node;
                        while (parentNode) {
                            parentNode->bookCount--; //Decrement bookCount by one 
                            parentNode = parentNode->parent; //Move to its parent node 
                        }
                        cout << "Book '" << bookTitle << "' has been removed from the catalog." << endl;
                        break; //Exit the loop 
                    }
                }

                //If the book was not found and removed from the node, then
                if (!flag) {
                    //Iterate through each child of the node to add them to the stack 
                    for (int i = 0; i < node->children.size(); i++) {
                        stack.push_back(node->children[i]);
                    }
                }
            }

            //If the book was not removed, then print out an error message 
            if (!flag) {
                cerr << "Failed to remove the book from the catalog." << endl;
            }
        } else {
            //If the user decides to calcel removing the book, then print out an message
            cout << "Book removal has been canceled." << endl;
        }
    } else {
        //If the book was not found in the tree, then print out an error 
        cerr << "Book cannot be found!" << endl;
    }
}

//Function to add category 
void LCMS::addCategory(string category) {
    //Create a new cateogry node in the library tree
    libTree->createNode(category);
    cout << "Category has been added!" << endl;
}

//Function to find the specified category 
void LCMS::findCategory(string category) {
    //Use getNode to find the node corresponding to the full path
    Node* categoryNode = libTree->getNode(category);

    //If categoryNode is found, then display message saying that
    if (categoryNode) {
        cout << "Category '" << category << "' is found!" << endl;
    } else {
        cerr << "Category '" << category << "' is not found!" << endl;
    }
}

//Function to remove the specified category 
void LCMS::removeCategory(string category) {
    //Create a node called categoryNode for the specified category 
    Node* categoryNode = libTree->getNode(category);

    //If the category node was found and it has a parent, then
    if (categoryNode && categoryNode->parent) {
        //Declare booksRemove to count the number of books in the category
        int booksRemove = categoryNode->books.size();  

        //Create a stack to hold nodes
        MyVector<Node*> nodes_stack;
        //use push_back function to initialize the stack with the categoryNode
        nodes_stack.push_back(categoryNode);
        
        //Loop until stack is empty
        while (!nodes_stack.empty()) {
            //Get the node from the stack 
            Node* currentNode = nodes_stack.back();
            //Remove the last node of the stack 
            nodes_stack.erase(nodes_stack.size() - 1);
            
            //Add the book count of the node to booksRemove
            booksRemove += currentNode->books.size();

            //Iterate through each child to add them into the stack by using push_back function 
            for (int i = 0; i < currentNode->children.size(); i++) {
                nodes_stack.push_back(currentNode->children[i]);
            }
        }

        //Update the book count in the parent node
        libTree->updateBookCount(categoryNode->parent, -booksRemove);

        //Remove the category ndoe by calling the remove function 
        libTree->remove(categoryNode->parent, categoryNode->name);
        cout << "Category '" << category << "' removed!" << endl;
    } else if (!categoryNode) {
        //If the category node was not found, then display an error message
        cerr << "Category '" << category << "' not found!" << endl;
    } else {
        //If the category node is root, then do not remove 
        cerr << "Cannot remove the root category!" << endl;
    }
}


//Function to edit the specified category 
void LCMS::editCategory(string category) {
    //Create a node for the specified category 
    Node* oldCategoryNode = libTree->getNode(category);
    //If the category node was found, then
    if (oldCategoryNode) {
        //Ask user input for a new category name 
        string newCategory;
        cout << "Enter new category name: ";
        getline(cin, newCategory);

        //Create a node called newCategoryNode with the updated name 
        Node* newCategoryNode = libTree->createNode(newCategory);

        //For loop to move all the books from the old category to new one
        for (int i = 0; i < oldCategoryNode->books.size(); i++) {
            //Create a book object to get each book from the old category 
            Book* book = oldCategoryNode->books[i];
            //Add the book to the new category 
            newCategoryNode->books.push_back(book);
        }


        //Remove the old category by calling remove function 
        libTree->remove(oldCategoryNode, category);

        cout << "Category is now updated to " << newCategory << "!" << endl;
    } else {
        //If category cannot be found, then display an error message 
        cerr << "Category cannot be found!" << endl;
    }
}

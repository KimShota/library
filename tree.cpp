//============================================================================
// Name         : tree.cpp
// Author       : Shota Matsumoto
// Version      : 1.0
// Date Created : 11/02/2024 
// Date Modified: 11/07/2024
// Description  : Implementation of a tree structure 
//============================================================================
#include <iostream>
#include<fstream>
#include<string>
#include "myvector.h"
#include "book.h"
#include "tree.h"
using namespace std;

//Constructor
Node::Node(string name){
    this->name = name; //Set the name of the node 
    this->bookCount = 0; //Initialize bookCount to 0
    this->parent = NULL; //Set parent to NULL
}

//Function to obtain the category path for node 
string Node::getCategory(Node* node){
    //Base case
    //If the current node is a root, then return its name
    if(!node->parent){
        return node->name;
    }
    //Recursively call getCategory function to move to the current node's parent, while printing the name of the current node
    return node->parent->getCategory(node->parent) + "/" + node->name;
}

//Deconstructor 
Node::~Node(){
    //Iterate through each child node and deallocate the memory for them
    for (int i = 0; i < children.size(); i++){
        delete children[i];
    }
}

//==========================================================

//Constructor 
Tree::Tree(string rootName){
    //Initialize the root with the provided name
    root = new Node(rootName);
}

//Deconstructor 
Tree::~Tree(){
    //Delete the node to delete the whole tree
    delete root;
}

//Getter function for the root node 
Node* Tree::getRoot(){
    return root;
}

//Function to check if the node is the last child 
bool Tree::isLastChild(Node *ptr){
    //Check if ptr has its parent or not. If it does not have its parent, then return false since the root cannot be a child node
    if (ptr->parent == NULL){
        return false;
    }
    //Create a node called parent and set it to the parent of the current node
    Node* parent = ptr->parent;
    //Check if the current node is the last child node or not
    return parent->children[parent->children.size() - 1] == ptr;
}

//Function to insert a new child with the specific node to the given parent node 
void Tree::insert(Node* node, string name){
    //Create a node called child with the specified name 
    Node* child = new Node(name);
    //Set its parent to the specific node 
    child->parent = node;
    //Add the child to the parent's children vector 
    node->children.push_back(child);
}

//Function to remove a specific child node 
void Tree::remove(Node* node, string child_name){
    //Iterate through each child to find the child node with the provided name
    for (int i = 0; i < node->children.size(); i++){
        if (node->children[i]->name == child_name){
            delete node->children[i]; //Deallocate the memory space for the specific child
            node->children.erase(i); //Remove the pointer that points to the deleted node from the children vector
            break; //Stop
        }
    }
}

//Function to check if the node is root or not 
bool Tree::isRoot(Node* node){
    //Return true if the node is root 
    return node==root;
}

//Function to locate a ndoe based on the path 
Node* Tree::getNode(string path) {
    //Create a new node called current and set it to root 
    Node* current = root;
    //Set the position to 0
    size_t pos = 0;
    string categoryName;

    //Loop through each category in the path by splitting with '/'
    while ((pos = path.find('/')) != string::npos) {
        //Get the name of the next category 
        categoryName = path.substr(0, pos); 
        //Set boolean variable called found to false 
        bool found = false;

        //Iterate and search through current node's children for matching
        for (int i = 0; i < current->children.size(); i++) {
            if (current->children[i]->name == categoryName) {
                //Move to the child node that is fonud to be matching
                current = current->children[i]; 
                //Set found to be true because the node is found 
                found = true;
                //Stop looping
                break;
            }
        }

        //If the given category is not found, then return nullptr 
        if (!found) return nullptr;

        //Remove the category from the path 
        path.erase(0, pos + 1);
    }

    //If path is empty  
    if (!path.empty()) {
        bool found = false;
        //Iterate through children of the current node to locate the last category
        for (int i = 0; i < current->children.size(); i++) {
            //If the name of the child matches path, then
            if (current->children[i]->name == path) {
                //Move to the child that is found to be matching 
                current = current->children[i];
                //Set found to be true 
                found = true;
                //Stop looping
                break;
            }
        }

        //If the final category part cannot be found, then return nullptr
        if (!found) return nullptr;
    }

    //Return the node 
    return current;
}

//Function to create a new node based upon the given path
Node* Tree::createNode(string path){
    //Create a new node called current to start from the root 
    Node* current = root;
    //Set the position for splitting path
    size_t pos = 0;
    string categoryName;
    
    //Use while loop to traverse each level in the path being splitted by '/'
    while((pos = path.find('/')) != string::npos){
        //Get the next category 
        categoryName = path.substr(0, pos);
        //Create a new node called childNode to get the child node with the matching name 
        Node* childNode = getChild(current, categoryName);
        
        //If the child node does not exist, then
        if (!childNode){
            //Insert a new node 
            insert(current, categoryName);
            //Get referece to the new child node 
            childNode = getChild(current, categoryName);
        }
        
        //Move to the child node for next level
        current = childNode;
        //Delete the processed parts from the given path 
        path.erase(0, pos + 1);
    }

    Node* childNode = getChild(current, path);
    //If the child node does not exist, then
    if (!childNode){
        //Insert a new node 
        insert(current, path);
        //Obtain reference to the newly added child node
        childNode = getChild(current, path);
    }
        
    current = childNode;
    //Return current
    return current;
}

//Function to return the child node with the specified name 
Node* Tree::getChild(Node* ptr, string childname){
    //Iterate through each child node
    for (int i = 0; i < ptr->children.size(); i++){
        //If the name of the current node is same as the name of the child node, then it will return the pointer to that child node
        if (ptr->children[i]->name == childname){
            return ptr->children[i];
        }
    }
    //If no child with the given name is found, then return NULL
    return NULL;
}

//Function to update the book count for node and its parent 
void Tree::updateBookCount(Node* ptr, int offset){
    //Adjust the bookCount with the provided offset 
    ptr->bookCount += offset;
    //Recursively call the function itself to update the parent's book count 
    if(ptr->parent != nullptr){updateBookCount(ptr->parent, offset);}
}

//Function to find the book with the specified title 
Book* Tree::findBook(Node *node, string bookTitle){
    //Iterate through books in the node using for loop
    for (int i = 0; i < node->books.size(); i++){
        //If the title matches 
        if (node->books[i]->title == bookTitle){
            return node->books[i]; //Return the book 
        }
    }
    //Recursively call the function itself to iterate through the child nodes 
    for (int i = 0; i < node->children.size(); i++){
        Book* book = findBook(node->children[i], bookTitle);
        if (book != nullptr){return book;} //Return book 
    }
    //Return nullptr if book is not found
    return nullptr;
}

//Function to remove the book from the specific node
bool Tree::removeBook(Node* node,string bookTitle){
    //Iterate through all the books in the node
    for (int i = 0; i < node->books.size(); i++){
        //If title matches
        if (node->books[i]->title == bookTitle){
            //Deallocate memory space for book 
            delete node->books[i];
            //Remove the book from the books vector 
            node->books.erase(i);
            //Return true if the book is successfully removed 
            return true;
        }
    }
    
    //Return false if the book is not found 
    return false;
}

//Function to display all the books inside the node and its children nodes 
void Tree::printAll(Node *node){
    //Iterate through each book in the node 
    for (int i = 0; i < node->books.size(); i++){
        //Display each node by calling display function 
        node->books[i]->display();
    }
    
    //Recursively call the function itself to print all the books in the child nodes 
    for (int i = 0; i < node->children.size(); i++){
        printAll(node->children[i]);
    }
}

//Function to export all the books in the node and its children
int Tree::exportData(Node* node, std::ofstream& file) {
    //Initialize the bookCount variable of type integer to 0
    int bookCount = 0;
    //Iterate through each book in the node
    for (int i = 0; i < node->books.size(); i++) {
        file << node->books[i]->title << ", " << node->books[i]->author << ", " << node->books[i]->isbn << ", " << node->books[i]->publication_year << ", " << node->books[i]->total_copies << ", " << node->books[i]->available_copies << std::endl;
        bookCount++; //Increment the book cout by one
    }

    //Iterate through each book in the child nodes 
    for (int i = 0; i < node->children.size(); i++) {
        bookCount += exportData(node->children[i], file); //Add count from the child nodes 
    }
    
    //Return the book count 
    return bookCount;
}

//Function to check if the tree is empty or not 
bool Tree::isEmpty(){
    //Return true if the root is NULL or if the roots of both the children and books vectors are empty
    return root == NULL || (root->children.empty() && root->books.empty()); 
}

void Tree::print()	//Print all categories/sub-categories in a tree format
{
    //Call the helper function to print the tree 
	print_helper("","",root);
}

void Tree::print_helper(string padding, string pointer,Node *node) //helper method for the print method
{
    //If the node is not nullptr, then
    if (node != nullptr) 
    {
        //Print the node's name and book count
        cout <<padding<<pointer<<node->name<<"("<<node->bookCount<<")"<<endl;

		if(node!=root)	padding+=(isLastChild(node)) ? "   " : "│  ";

        for(int i=0; i<node->children.size(); i++)	//remove the file/folder from original path
		{
			string marker = isLastChild(node->children[i]) ? "└──" : "├──";
			print_helper(padding,marker, node->children[i]);
		}
    }
}
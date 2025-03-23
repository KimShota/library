//============================================================================
// Name         : borrower.cpp
// Author       : Shota Matsumoto
// Version      : 1.0
// Date Created : 11/02/2024
// Date Modified: 11/06/2024
// Description  : Borrower class 
//============================================================================S
#include <iostream>
#include "borrower.h"

//Constructor
Borrower::Borrower(const std::string name, std::string id){
    this->name = name; //Set the name of the borrower 
    this->id = id; //Set the id of the borrower 
}

//Function to display all the books borrowed by a certain borrower
void Borrower::listBooks(){
    //Display the borrower's name and ID 
    std::cout << "Books borrowed by " << name << "(" << id << ")" << std::endl;
    //Loop through the list of the books that are borrowed by the borrower 
    for (int i = 0; i < books_borrowed.size(); i++){
        std::cout << i + 1 << ": "; //Display the position of the books in the list 
        books_borrowed[i]->display(); //Call display function of the Book object to show the details of the book
    }
}

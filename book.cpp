//============================================================================
// Name         : book.cpp
// Author       : Shota Matsumoto
// Version      : 1.0
// Date Created : 11/02/2024
// Date Modified: 11/06/2024
// Description  : Book Class to represent details about boooks and diaplay them
//============================================================================S
#include <iostream>
#include "book.h"
#include "lcms.h"
using namespace std; 

//Constructor
Book::Book(std::string title, std::string author, std::string isbn, int publication_year, int total_copies, int available_copies) {
    //Assign parameter values to member variables 
    this->title = title; //Set book title 
    this->author = author; //Set book author 
    this->isbn = isbn; //Set book ISBN
    this->publication_year = publication_year; //Set book's publication year 
    this->total_copies = total_copies; //Set total copies of book 
    this->available_copies = available_copies; //Set available copies of book
}

//Function to display details of book 
void Book::display(){
    //Display all the details of the book, including title, author, ISBN, publication year, total copies, and available copies. 
    cout << "Title: " << title << std::endl;
    cout << "Author: " << author << std::endl;
    cout << "ISBN: " << isbn << std::endl;
    cout << "Publication Year: " << publication_year << std::endl;
    cout << "Total copies: " << total_copies << std::endl;
    cout << "Available copies: " << available_copies << std::endl;
}



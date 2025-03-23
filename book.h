//============================================================================
// Name         : book.h
// Author       : Shota Matsumoto
// Version      : 1.0
// Date Created : 11/02/2024
// Date Modified: 11/06/2024
// Description  : header file of book.cpp
//============================================================================
#ifndef _BOOK_H
#define _BOOK_H
#include <string>
#include "myvector.h"

class Borrower;

class Book
{
	private:
		std::string title;
		std::string author;
		std::string isbn;
		int publication_year;
		int total_copies;
		int available_copies;
		MyVector<Borrower*> currentBorrowers;  
    	MyVector<Borrower*> allBorrowers;     

	public:
		Book(std::string title, std::string author, std::string isbn, int publication_year,int total_copies, int available_copies);
		void display(); // display details of a book (see output of command findbook)
		friend class Tree;
		friend class Node;
		friend class LCMS;
		friend class Borrower;
};

#endif
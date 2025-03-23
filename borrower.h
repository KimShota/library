//============================================================================
// Name         : borrower.h
// Author       : Shota Matsumoto
// Version      : 1.0
// Date Created : 11/02/2024
// Date Modified: 11/06/2024
// Description  : Header file for borrower.cpp
//============================================================================S

#ifndef _BORROWER_H
#define _BORROWER_H

#include <string>
#include "myvector.h"
#include "book.h"

class Borrower {
	private:
		std::string name;
		std::string id;
		MyVector<Book*> books_borrowed;  
	public:
		Borrower(const std::string name, std::string id);
		void listBooks();
		friend class LCMS;
		friend class Tree;
		friend class Book;
};

#endif
//============================================================================
// Name         : tree.h
// Author       : Shota Matsumoto
// Version      : 1.0
// Date Created : 11/4/2024
// Date Modified: 11/08/2024
// Description  : header file for tree.cpp
//============================================================================
#ifndef _TREE_H
#define _TREE_H
#include<string>
#include "myvector.h"
#include "book.h"
using namespace std;

class Node
{
	private:
		string name;				//name of the Node
		MyVector<Node*> children; 
    	MyVector<Book*> books;    
		unsigned int bookCount;
		Node* parent; 				//link to the parent 

	public:
		//constructor to create an empty node (category/sub-category)
		Node(string name);

		// return category of a node (e.g. "Computer Science/Operating Systems")
		// where "Operating Systems" is the name of node and "Computer Science"
		// is the name of the name of the parent node.
		string getCategory(Node* node);
		
		//deletes a node and clear/clean all its vectors
		~Node();	

	public:
		friend class Tree;
		friend class LCMS;
};
//==========================================================
class Tree
{
	private:
		Node *root;				//root of the Tree
		
	public:	 	//Required methods
		Tree(string rootName);	
		~Tree();
		Node* getRoot();
		bool isLastChild(Node *ptr);	//given a pointer to node, the method should determine that the node is the last child in the children vector or not
		void insert(Node* node,string name);			//insert a new child to a given node of of the tree
		void remove(Node* node,string child_name);		//remove a specific child from a given node of the tree
		bool isRoot(Node* node); 						//return true if the given node is the root, false otherwise
		Node* getNode(string path);						//given a path (category/sub-category/sub-category/..) the method should return the Node if found, false otherwise
		Node* createNode(string path);					//Create a node on a given path, e.g. category/sub-category/sub-category/...
		Node* getChild(Node *ptr, string childname);	//given a node and name of a child, the method returns pointer to the child node if exist, nullptr otherwise
		void updateBookCount(Node *ptr, int offset);	//update a books count by an offset e.g. +1/-1
		Book* findBook(Node *node, string bookTitle);	//find a book in a given node, returns nullptr the book is not found
		bool removeBook(Node* node,string bookTitle);   //remove a book from a given node
		void printAll(Node *node);					    //printAll books of a node and it children recursively (see output of findAll command)
		void print();			                        //Print all categories/sub-categories of a the tree. see output of list command (please use the implementation given below)
		void print_helper(string padding, string pointer,Node *node); // helper method for the print() (please use the implementation given below)
		int exportData(Node *node,ofstream& file);		//Export all books of a given node and its children to a specific file.
		bool isEmpty();									//return true if the tree is empty false otherwise
};
#endif

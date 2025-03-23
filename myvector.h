//============================================================================
// Name         : myvector.h
// Author       : Shota Matsumoto
// Version      : 1.0
// Date Created : 11/02/2024
// Date Modified: 11/08/2024
// Description  : Vector implementation in C++
//============================================================================
#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <stdexcept>

template <typename T>
class MyVector {
	private:
		T *data;						//pointer to int(array) to store elements
		int v_size;						//current size of vector (number of elements in vector)
		int v_capacity;					//capacity of vector

		void resize(){
			v_capacity = v_capacity == 0 ? 1 : v_capacity * 2; 
			T* newData = new T[v_capacity]; 
			for (int i = 0; i < v_size; i++){
				newData[i] = data[i]; 
			}
			delete[] data; 
			data = newData; 
		}

	public:
		MyVector();						//No argument constructor
		MyVector(int cap);				//One Argument Constructor
		MyVector(const MyVector& other);		//Copy Constructor
		~MyVector();					//Destructor

		void push_back(T element);		//Add an element at the end of vector
		void insert(int index, T element); //Add an element at the index 
		void erase(int index);			//Removes an element from the index
		T& operator[](int index);		//return reference of the element at index
		T& at(int index); 				//return reference of the element at index
		const T& front();				//Returns reference of the first element in the vector
		const T& back();				//Returns reference of the Last element in the vector
		int size() const;				//Return current size of vector
		int capacity() const;			//Return capacity of vector
		bool empty() const; 			//Return true if the vector is empty, False otherwise
		void shrink_to_fit();			//Reduce vector capacity to fit its size
};
//Constructor with no argument 
template <typename T>
MyVector<T>::MyVector() : v_size(0), v_capacity(1) {
	//Allocate the memory space for the vector with the initial capacity of 1
	data = new T[v_capacity]; 
}
//Constructor with one argument 
template <typename T>
MyVector<T>::MyVector(int cap) : v_size(0), v_capacity(cap) {
	//Create a vector that holds the capacity of v_capacity
	data = new T[v_capacity]; 
}
//Copy construtor to copy all the information including size, capacity, and elements from the other object into newly made vector
template <typename T>
MyVector<T>::MyVector(const MyVector &other) : v_size (other.v_size), v_capacity(other.v_capacity) {
	//Create an array of type T with the capacity of v_capacity 
	data = new T[v_capacity]; 
	
	//Iterate through each element of the other data to store them into the new array 
	for (int i = 0; i < v_size; i++){
		data[i] = other.data[i]; 
	}
}
//Deconstructor to delete all
template <typename T>
MyVector<T>::~MyVector() {
	//Dynamically deallocate the memory space of data 
	delete[] data; 
}
//Function to return the value of the size of vector 
template <typename T>
int MyVector<T>::size() const
{
	//Return the size of vector 
	return v_size; 
}
//Function to return the value of the capacity 
template <typename T>
int MyVector<T>::capacity() const
{
	//Return the capacity of vector 
	return v_capacity; 
}
//Function to check if vector is empty or not 
template <typename T>
bool MyVector<T>::empty() const
{
	//Return true if the size is 0
	return v_size == 0; 
}
//Function to add a new element at the end of vector 
template <typename T>
void MyVector<T>::push_back(T element) {
	//If the size of the vector is larger or equal to the capacity of vector
	if (v_size >= v_capacity){
		resize(); 
	}
	//Append a new element at the end of vector 
	data[v_size++] = element; 
}
//Function to insert an element at a specified index in the vector 
template <typename T>	
void MyVector<T>::insert(int index, T element)
{
	//If index is out of range, then display an error message 
	if (index < 0 || index >= v_size){
		throw std::out_of_range("Index is out of range"); 
	}

	//If the size of vector is larger than its capacity, then resize its capapacity by calling resize function 
	if (v_size >= v_capacity){
		resize(); 
	}
	//Shift all the elements to the right for the sake of the space for new element 
	for (int i = v_size; i > index; i--){
		data[i] = data[i - 1]; 
	}
	//Insert a new element at the specified index
	data[index] = element; 
	//Increment size by one
	v_size++; 
}
//Function to remove the element at the specified index in the vector 
template <typename T>	
void MyVector<T>::erase(int index) {
	//If the index is out of range then display an error message 
	if (index < 0 || index >= v_size){
		throw std::out_of_range("Index is out of range"); 
	}

	//Shift all the elements to the left to make a space for newly added element 
	for (int i = index; i < v_size - 1; i++){
		data[i] = data[i + 1]; 
	}
	//Decrease the size of vector by 1 
	v_size--; 
}
//Overload function to access elements at the specific index using [] operator 
template <typename T>
T& MyVector<T>::operator[](int index) {
	//Return the element at the given index 
	return data[index]; 
}
//Function to access the element at the given index with the boundary check as well
template <typename T>
T& MyVector<T>::at(int index) {
	//If the index is out of range, then display an error message
	if (index < 0 || index >= v_size){
		throw std::out_of_range("Index is out of range"); 
	}
	//Return the element at the given index 
	return data[index]; 
}
//Function to return the first element of the vector 
template <typename T>
const T& MyVector<T>::front()
{
	//If vector is empty, then display an error message indicating that the vector is empty
	if (empty()){
		throw std::out_of_range("Vector is empty!"); 
	}
    //Return the first element of the vector 
	return data[0]; 
}
//Function to return the element at the last index 
template <typename T>
const T& MyVector<T>::back()
{
	//If vector is empty, then display an error message indicating that the vector is empty
	if (empty()){
		throw std::out_of_range("Vector is empty!"); 
	}
	//Return the last element 
	return data[v_size - 1]; 
}
//Function to reduce the capacity of the vector to the size of it 
template <typename T>
void MyVector<T>::shrink_to_fit()
{
	//If size of the vector is smaller than its capacity
	if (v_size < v_capacity){
		//Create vector called newData with the capacity of v_size 
		T* newData = new T[v_size]; 
		//Iterate through each element in the data to copy them into newData
		for (int i = 0; i < v_size; i++){
			newData[i] = data[i]; 
		}
		//Dynamically deallocate the memory space of data 
		delete[] data; 
		//Assign newData to data
		data = newData; 
		//Assign size to capacity 
		v_capacity = v_size; 
	}
}

#endif
# 📚 Library Catalog Management System (LCMS)

A fully functional **C++ console application** to manage a digital library catalog.  
Users can import/export book data, manage book categories, track borrowers, and interactively manage a tree-based hierarchy of books.

Created by **Shota Matsumoto**.

---

## 🌟 Key Features

- 📁 Organize books in a category/sub-category tree structure
- 📦 Import/export book data from/to CSV
- 📚 Add, edit, delete books and categories
- 👥 Track current and past borrowers
- 🔍 Find books or categories interactively
- 🧾 Custom implementation of `vector`, `tree`, and hash-based components

---

## 🧠 Technologies & Concepts

- 🧱 Custom `MyVector<T>` class (dynamic array)
- 🌲 Tree-based category organization (`Tree` and `Node`)
- 👤 Borrower tracking system with history
- 📘 Book objects with metadata
- 🛠️ Manual memory management (no STL containers)
- 📄 CSV file parsing and exporting

---

## 📂 File Structure

📁 Project/ ├── main.cpp # Command-line UI for LCMS ├── lcms.h/.cpp # Core LCMS logic (books, categories, borrowers) ├── tree.h/.cpp # Tree structure for category management ├── book.h/.cpp # Book class definition ├── borrower.h/.cpp # Borrower class with book history ├── myvector.h # Custom vector implementation ├── makefile # Build file


---

## 🚀 How to Build and Run

### 🔨 Build with Makefile

```bash
make

./main

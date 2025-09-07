# 🌳 Trie Data Structure Implementation

![C++](https://img.shields.io/badge/Language-C++-blue.svg)  
![Status](https://img.shields.io/badge/Status-In_Progress-yellow.svg)

A **C++ implementation of the Trie (Prefix Tree)** designed to efficiently manage word storage, retrieval, and prefix-based operations.  
This project was developed as part of the **FuzeTek Summer Internship 2025 (Project 4)** to practice Object-Oriented Programming (OOP) principles, algorithm design, and real-world applications of data structures.

---

## 🚀 Features

- **Word Insertion** → Add new words to the Trie.
- **Word Search** → Check if a complete word exists.
- **Prefix Checking** → Verify if words start with a given prefix.
- **Autocomplete** → Suggest words that match a prefix.
- **Edge Case Handling** → Handles empty strings, non-existent prefixes, and case sensitivity.
- **Test Suite** → Includes multiple scenarios to validate functionalities.

---

## 📂 System Architecture

The project is designed with an **OOP approach**:

| Class      | Responsibility                                                           |
| ---------- | ------------------------------------------------------------------------ |
| `TrieNode` | Represents a node with up to 26 children and an end-of-word flag.        |
| `Trie`     | Provides core operations: insert, search, prefix checking, autocomplete. |
| `Main`     | Runs test cases and demonstrates Trie functionalities interactively.     |

---

## 🖼️ Visual Representation

Here’s a high-level diagram of the basic Trie operations:

```mermaid
flowchart TD
    A[Insert Word] --> B[Traverse Characters]
    B -->|Node Exists| C[Move to Next Character]
    B -->|Node Missing| D[Create New Node]
    C --> E[Mark End of Word]
    D --> E[Mark End of Word]

    F[Search Word] --> G[Traverse Characters]
    G -->|Path Exists| H[Check End of Word]
    G -->|Path Missing| I[Word Not Found]

    J[Autocomplete] --> K[Find Prefix Node]
    K -->|Prefix Found| L[DFS to Collect All Words]
    K -->|Prefix Missing| M[No Suggestions]
```

---

## 📖 Example Usage

```cpp
#include "trie.h"

Trie trie;

// Insert words
trie.insert("apple");
trie.insert("application");
trie.insert("banana");

// Search
trie.search("apple");      // true
trie.search("app");        // false

// Prefix checking
trie.startsWith("app");    // true

// Autocomplete
vector<string> results = trie.autocomplete("app");
// results → ["apple", "application"]
```

---

## 📑 Applications

Tries are widely used in real-world systems such as:

- 🔍 **Search Engines** → Autocomplete and query suggestions
- 📖 **Spell Checkers** → Word lookup and corrections
- 📱 **Contact Lists** → Quick name suggestions
- 📚 **Dictionary Implementations** → Efficient storage and retrieval
- 🌐 **IP Routing** → Fast lookup of routing tables using prefix matching
- 🎮 **Games** → Efficient word storage and validation in word-based games

---

## ⚙️ Installation & Usage

1. Clone the repository:
   ```bash
   git clone https://github.com/Sondos100/Trie-DS-Implementation.git
   ```
2. Compile the code
3. Run the program

---

## 🎯 Learning Outcomes

Through this project, we gained experience in:

- Implementing efficient **tree-based data structures**.
- Applying **OOP design principles** in C++.
- Writing **modular and testable code**.
- Understanding **real-world applications** of Tries in software systems.

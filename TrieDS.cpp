#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Each node in the Trie
class TrieNode {
public:
    // Each node has up to 26 children (for each letter)
    TrieNode *children[26];

    // Marks if this node completes a word
    bool isEndOfWord;
    int frequency;
    int unifreq;

    // Constructor
    TrieNode() {
        isEndOfWord = false;
        frequency = 0;
        unifreq = 0;
        for (int i = 0; i < 26; i++) {
            children[i] = nullptr;
        }
    }

    bool hasChildren() {
        for (int i = 0; i < 26; i++) {
            if (children[i] != nullptr) {
                return true;
            }
        }
        return false;
    }
};

// The Trie class
class Trie {
private:
    TrieNode *root;

    // Helper function to find all words from a node
    // Input: current node, current word formed so far, results vector to store words
    // Output: none (modifies results vector by reference)
    // Purpose: Recursively find all complete words starting from the given node

    void findAllWords(TrieNode *node, string currentWord, vector<pair<string, int> > &results) {
        if (node->isEndOfWord) {
            results.push_back({currentWord, node->frequency});
        }

        for (int i = 0; i < 26; i++) {
            if (node->children[i] != nullptr) {
                char nxt = 'a' + i;
                findAllWords(node->children[i], currentWord + nxt, results);
            }
        }
    }

    // Helper function to delete a word from the Trie
    // Input: current node, word to delete, current depth in the word
    // Output: boolean indicating whether the current node can be safely deleted
    // Purpose: Recursively remove the word from the Trie and clean up unused nodes
    bool deleteHelper(TrieNode *node, const string &word, int depth, bool &wordExisted) {
        // if the node is a nullptr
        if (!node) {
            return false;
        }

        // reached the end of the word
        if (depth == (int) word.size()) {
            if (!node->isEndOfWord) {
                return false; // Word not found
            }

            // unmark this node as the end of a word
            node->isEndOfWord = false;
            node->unifreq--;
            wordExisted = true;

            // the node can be deleted if it has no children
            return !node->hasChildren();
        }

        // move to the child node for the next character
        int index = tolower(word[depth]) - 'a';
        TrieNode *childNode = node->children[index];

        // If the child node doesn't exist
        if (!childNode) {
            return false;
        }

        // call the function for the child node
        // true if the child node can be deleted
        bool shouldDeleteChild = deleteHelper(childNode, word, depth + 1, wordExisted);
        if (wordExisted) {
            node->unifreq--;
        }

        // If node should be deleted handle the deletion here
        if (shouldDeleteChild) {
            delete childNode;
            node->children[index] = nullptr;
        }

        // The current node can be deleted only if it's not a word itself AND has no other children after
        return !node->isEndOfWord && !node->hasChildren();
    }

    // 1. If node is null, return 0.
    // 2. Add 1 if this node marks the end of a word.
    // 3. Recursively check all 26 children.
    // 4. Return the total count of words.
    int countWordsHelper(TrieNode *node) {
        if (!node) return 0;

        int count = node->isEndOfWord; // bool -> 0 or 1

        for (int i = 0; i < 26; i++) {
            if (node->children[i] != nullptr) {
                count += countWordsHelper(node->children[i]);
            }
        }
        return count;
    }

public:
    // Constructor
    // Input: none
    // Output: none
    // Purpose: Initialize the Trie with a root node
    Trie() {
        root = new TrieNode();
    }

    // Insert a word into the Trie
    // Input: word to insert (string)
    // Output: none
    // Purpose: Add a word to the Trie by creating nodes for each character
    bool insert(string word) {
        // Validate that the word contains only alphabetic characters
        for (char c: word) {
            if (!isalpha(c)) {
                return false;
            }
        }
        TrieNode *curr = root;

        for (char c: word) {
            c = tolower(c);

            if (curr->children[c - 'a'] == nullptr) {
                curr->children[c - 'a'] = new TrieNode();
            }

            curr = curr->children[c - 'a'];
        }

        curr->unifreq++;
        if (curr->isEndOfWord) {
            // Word already existed
            return false; // duplicate
        }
        curr->isEndOfWord = true;
        return true; // unique
    }


    int getFrequency(string word) {
        TrieNode *node = root;
        for (char c: word) {
            int idx = c - 'a';
            if (!node->children[idx]) return 0;
            node = node->children[idx];
        }
        if (node->isEndOfWord)
            return node->unifreq;
        return 0;
    }

    // 1. Start counting words from the root node.
    // 2. Pass the root to the recursive helper.
    int countWords() {
        return countWordsHelper(root);
    }

    int prefixCount(const string &prefix) {
        TrieNode *node = root;
        vector<pair<string, int> > result;

        for (int i = 0; i < prefix.size(); i++) {
            int charIndex = tolower(prefix[i]) - 'a';

            // To easily modify according to the supported chars. // To handle uninserted characters.
            if (charIndex < 0 || charIndex >= 26 || !node->children[charIndex]) {
                return {}; // Invalid character
            }
            node = node->children[charIndex];
        }

        findAllWords(node, prefix, result);

        return result.size();
    }

    vector<pair<string, int> > Lexicographical() {
        vector<pair<string, int> > result;
        TrieNode *node = root;
        string curr = "";

        findAllWords(node, curr, result);

        return result;
    }

    // Search for a word in the Trie
    // Input: word to search for (string)
    // Output: boolean indicating if the word exists
    // Purpose: Check if the complete word exists in the Trie
    bool search(string word) {
        TrieNode *node = root;

        for (char c: word) {
            c = tolower(c);
            int index = c - 'a';

            if (index < 0 || index >= 26)
                return false;

            if (!node->children[index])
                return false;

            node = node->children[index];
        }

        if (node->isEndOfWord) {
            node->frequency++;
            return true;
        }
        return false;
    }

    // Check if any word starts with the given prefix
    // Input: prefix to check (string)
    // Output: boolean indicating if any word has this prefix
    // Purpose: Verify if the prefix exists in the Trie (doesn't need to be a complete word)
    bool startsWith(string prefix) {
        TrieNode *current = root;

        for (char c: prefix) {
            if (!isalpha(c))
                return false;

            c = tolower(c);
            int index = c - 'a';

            if (current->children[index] == nullptr) {
                return false;
            }

            current = current->children[index];
        }

        return true;
    }


    static bool cmp(const pair<string, int> &a, const pair<string, int> &b) {
        return a.second > b.second;
    }

    // Get all words that start with the given prefix
    // Input: prefix to complete (string)
    // Output: vector of strings that start with the prefix
    // Purpose: Find all complete words that begin with the given prefix
    vector<pair<string, int> > autocomplete(string prefix) {
        // 1. Create a ptr of type TrieNode and set it to root.
        // 2. Traverse to get the start node to collect words from.
        // 3. Check validity of prefix characters and existence in Trie.
        // 4. Use helper to collect all words from that node & return.


        vector<pair<string, int> > suggestions;

        TrieNode *node = root;

        for (int i = 0; i < prefix.size(); i++) {
            int charIndex = tolower(prefix[i]) - 'a';

            // To easily modify according to the supported chars.
            if (charIndex < 0 || charIndex >= 26) {
                return {}; // Invalid character
            }

            // To handle uninserted characters.
            if (!node->children[charIndex]) {
                return {}; // No words with this prefix
            }

            node = node->children[charIndex];
        }
        //Collect all words starting from this node.
        findAllWords(node, prefix, suggestions);

        //Sort suggestions by frequency (highest first).
        sort(suggestions.begin(), suggestions.end(), cmp);

        return suggestions;
    }

    // Delete a word from the Trie
    // Input: word to delete (string)
    // Output: boolean indicating if deletion was successful
    // Purpose: Removes the word from the Trie and cleans up unused nodes
    bool deleteWord(string word) {
        // Handle empty strings
        if (word.empty()) {
            return false;
        }

        // Validate that the word contains only alphabetic characters
        for (char c: word) {
            if (!isalpha(c)) {
                return false;
            }
        }

        // Convert word to lowercase for consistent deletion
        string lowerWord = word;
        transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
        if (!search(word)) {
            return false; // no need to call helper
        }
        // Call helper function to handle recursive deletion
        bool wordExisted = false;
        deleteHelper(root, lowerWord, 0, wordExisted);

        return wordExisted;
    }
};

// Main function
// Input: none
// Output: integer return code
// Purpose: Program entry point, run tests and interactive demo
int main() {
    cout << "=== TRIE DATA STRUCTURE IMPLEMENTATION ===" << endl;
    cout << "Testing all Trie functionalities..." << endl;

    Trie trie;

    // Test 1: Basic insertion and search
    cout << "\n1. Testing basic insertion and search:" << endl;
    cout << "======================================" << endl;


    vector<string> words = {"apple", "banana", "orange", "grape", "kiwi", "apple"};

    for (const string &word: words) {
        if (trie.insert(word)) {
            cout << "Inserted: " << word << " (unique)" << endl;
        } else {
            cout << "Inserted: " << word << " (duplicate)" << endl;
        }
    }


    bool found = trie.search("apple");
    cout << "Search 'apple': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;

    found = trie.search("kiwi");
    cout << "Search 'kiwi':  " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;

    found = trie.search("apple");
    cout << "Search 'apple': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 2 " << endl;

    found = trie.search("grape");
    cout << "Search 'grape': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;

    found = trie.search("apple");
    cout << "Search 'apple': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 3 " << endl;

    found = trie.search("kiwi");
    cout << "Search 'kiwi':  " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 2 " << endl;

    cout << "Unique Word Count :   " << trie.countWords() << endl;

    // Test search for existing words
    for (const string &word: words) {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    // Test search for non-existing words
    vector<string> nonExisting = {"app", "ban", "ora", "graph", "kiwis"};
    for (const string &word: nonExisting) {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;
    }

    // Test 2: Prefix checking
    cout << "\n2. Testing prefix checking:" << endl;
    cout << "==========================" << endl;

    vector<string> prefixes = {"app", "ban", "ora", "grap", "k"};
    for (const string &prefix: prefixes) {
        bool hasPrefix = trie.startsWith(prefix);
        cout << "Prefix '" << prefix << "': " << (hasPrefix ? "EXISTS" : "DOESN'T EXIST") << endl;
    }

    vector<string> nonPrefixes = {"x", "yield", "zed", "micro", "nano"};
    for (const string &prefix: nonPrefixes) {
        bool hasPrefix = trie.startsWith(prefix);
        cout << "Prefix '" << prefix << "': " << (hasPrefix ? "EXISTS" : "DOESN'T EXIST") <<
                " (expected: DOESN'T EXIST)" << endl;
    }

    // Test 3: Autocomplete functionality
    cout << "\n3. Testing autocomplete functionality:" << endl;
    cout << "======================================" << endl;

    vector<string> testPrefixes = {"a", "b", "o", "g", "k", "ap", "ban", "ora", "gr", "ki"};
    for (const string &prefix: testPrefixes) {
        vector<pair<string, int> > suggestions = trie.autocomplete(prefix);
        cout << "Autocomplete for '" << prefix << "': ";
        if (suggestions.empty()) {
            cout << "No suggestions found";
        } else {
            for (size_t i = 0; i < suggestions.size(); i++) {
                if (i > 0)
                    cout << ", ";
                cout << suggestions[i].first << "--->" << suggestions[i].second;
            }
        }
        cout << endl;
    }

    // Test 4: Edge cases
    cout << "\n4. Testing edge cases:" << endl;
    cout << "======================" << endl;

    // Empty string tests
    bool emptySearch = trie.search("");
    cout << "Search empty string: " << (emptySearch ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;

    bool emptyPrefix = trie.startsWith("");
    cout << "Starts with empty prefix: " << (emptyPrefix ? "EXISTS" : "DOESN'T EXIST") << " (expected: EXISTS)" << endl;

    vector<pair<string, int> > emptySuggestions = trie.autocomplete("");
    cout << "Autocomplete for empty string: ";
    if (emptySuggestions.empty()) {
        cout << "No suggestions found";
    } else {
        for (size_t i = 0; i < emptySuggestions.size(); i++) {
            if (i > 0)
                cout << ", ";
            cout << emptySuggestions[i].first << "--->" << emptySuggestions[i].second;
        }
    }
    cout << " (expected: all words)" << endl;

    // Test 5: Adding more words and retesting
    cout << "\n5. Testing with additional words:" << endl;
    cout << "================================" << endl;


    vector<string> additionalWords = {
        "application", "appetizer", "application", "banister", "oracle", "bandana", "oracle", "grapefruit"
    };

    for (const string &word: additionalWords) {
        if (trie.insert(word)) {
            cout << "Inserted: " << word << " (unique)" << endl;
        } else {
            cout << "Inserted: " << word << " (duplicate)" << endl;
        }
    }


    found = trie.search("bandana");
    cout << "Search 'bandana': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;

    found = trie.search("application");
    cout << "Search 'application': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;


    cout << "Unique Word Count :   " << trie.countWords() << endl;

    // Test search for new words
    for (const string &word: additionalWords) {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    // Test autocomplete with new words
    vector<string> newPrefixes = {"app", "ban", "ora", "gra"};
    for (const string &prefix: newPrefixes) {
        vector<pair<string, int> > suggestions = trie.autocomplete(prefix);
        cout << "Autocomplete for '" << prefix << "': ";
        if (suggestions.empty()) {
            cout << "No suggestions found";
        } else {
            for (size_t i = 0; i < suggestions.size(); i++) {
                if (i > 0)
                    cout << ", ";
                cout << suggestions[i].first << "--->" << suggestions[i].second;
            }
        }
        cout << endl;
    }

    // Test 6: Case sensitivity (all lowercase expected)
    cout << "\n6. Testing case sensitivity:" << endl;
    cout << "============================" << endl;

    trie.insert("Hello");
    trie.insert("WORLD");
    cout << "Inserted: Hello" << endl;
    cout << "Inserted: WORLD" << endl;
    cout << "Unique Word Count :   " << trie.countWords() << endl;

    vector<string> caseWords = {"hello", "Hello", "WORLD", "world"};
    for (const string &word: caseWords) {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    // Test 7: Lexicographical Word Listing
    cout << "\n7. Lexicographical Word Listing:" << endl;
    cout << "============================" << endl;

    vector<pair<string, int> > result = trie.Lexicographical();
    cout << "[ " << result[0].first;
    for (int i = 1; i < result.size(); i++) {
        cout << " , " << result[i].first;
    }
    cout << " ]" << endl;

    cout << "\n8. Prefix Word Count:" << endl;
    cout << "============================" << endl;

    cout << "Prefix 'app' ->  Count: " << trie.prefixCount("app") << "  // Expected: 3 -> appetizer, apple, application"
            << endl;
    cout << "Prefix 'ban' ->  Count: " << trie.prefixCount("ban") << "  // Expected: 3 -> banana, bandana, banister" <<
            endl;
    cout << "Prefix 'gra' ->  Count: " << trie.prefixCount("gra") << "  // Expected: 2 -> grape, grapefruit" << endl;
    cout << "Prefix 'or' ->   Count: " << trie.prefixCount("or") << "  // Expected: 2 -> oracle, orange" << endl;
    cout << "Prefix 'he' ->   Count: " << trie.prefixCount("he") << "  // Expected: 1 -> hello" << endl;
    cout << "Prefix 'ki' ->   Count: " << trie.prefixCount("ki") << "  // Expected: 1 -> kiwi" << endl;
    cout << "Prefix 'w' ->    Count: " << trie.prefixCount("w") << "  // Expected: 1 -> world" << endl;
    cout << "Prefix 'z' ->    Count: " << trie.prefixCount("z") << "  // Expected: 0 -> no word" << endl;
    cout << "Prefix 'appl' -> Count: " << trie.prefixCount("appl") << "  // Expected: 2 -> apple, application" << endl;
    cout << "Prefix '' ->     Count: " << trie.prefixCount("") << "  // Expected: 12 -> all words in Trie" << endl;


    cout << "\n9. Testing deleteWord functionality:" << endl;
    cout << "======================================" << endl;

    // Case 1: Delete a word that is a prefix of another word (e.g., "app" if "apple" and "application" exist)
    trie.insert("application");
    trie.insert("appetizer");
    cout << "Inserted: application, appetizer" << endl;
    cout << "Unique Word Count before deletion: " << trie.countWords() << endl;

    cout << "Deleting 'apple'..." << endl;
    bool deleted = trie.deleteWord("apple");
    cout << "Deletion status: " << (deleted ? "SUCCESS" : "FAILED") << endl;
    cout << "Search for 'apple': " << (trie.search("apple") ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" <<
            endl;
    cout << "Search for 'application': " << (trie.search("application") ? "FOUND" : "NOT FOUND") << " (expected: FOUND)"
            << endl;
    cout << "Search for 'appetizer': " << (trie.search("appetizer") ? "FOUND" : "NOT FOUND") << " (expected: FOUND)" <<
            endl;
    cout << "Prefix 'app' count after deleting 'apple': " << trie.prefixCount("app") << " (expected: 2)" << endl;

    // Case 2: Delete a word that is an isolated branch (e.g., "banana")
    cout << "\nDeleting 'banana'..." << endl;
    deleted = trie.deleteWord("banana");
    cout << "Deletion status: " << (deleted ? "SUCCESS" : "FAILED") << endl;
    cout << "Search for 'banana': " << (trie.search("banana") ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" <<
            endl;

    // Case 3: Delete a word that has a prefix also as a word (e.g., "application")
    cout << "\nDeleting 'application'..." << endl;
    deleted = trie.deleteWord("application");
    cout << "Deletion status: " << (deleted ? "SUCCESS" : "FAILED") << endl;
    cout << "Search for 'application': " << (trie.search("application") ? "FOUND" : "NOT FOUND") <<
            " (expected: NOT FOUND)" << endl;
    cout << "Search for 'appetizer': " << (trie.search("appetizer") ? "FOUND" : "NOT FOUND") << " (expected: FOUND)" <<
            endl;
    cout << "Prefix 'app' count after deleting 'application': " << trie.prefixCount("app") << " (expected: 1)" << endl;

    // Case 4: Delete a word that does not exist
    cout << "\nDeleting 'nonexistent'..." << endl;
    deleted = trie.deleteWord("nonexistent");
    cout << "Deletion status: " << (deleted ? "SUCCESS" : "FAILED") << " (expected: FAILED)" << endl;

    // Case 5: Deleting an already deleted word
    cout << "\nDeleting 'banana' again..." << endl;
    deleted = trie.deleteWord("banana");
    cout << "Deletion status: " << (deleted ? "SUCCESS" : "FAILED") << " (expected: FAILED)" << endl;

    // Case 6: Deleting with uppercase input
    trie.insert("HELLO");
    cout << "\nInserted: HELLO" << endl;
    cout << "Deleting 'HELLO'..." << endl;
    deleted = trie.deleteWord("HELLO");
    cout << "Deletion status: " << (deleted ? "SUCCESS" : "FAILED") << endl;
    cout << "Search for 'hello': " << (trie.search("hello") ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" <<
            endl;

    cout << "Unique Word Count after all deletions: " << trie.countWords() << endl;

    cout << "\n=== ALL TESTS COMPLETED ===" << endl;

    return 0;
}

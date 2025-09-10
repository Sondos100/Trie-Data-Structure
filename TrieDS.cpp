#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Each node in the Trie
class TrieNode
{
public:
    // Each node has up to 26 children (for each letter)
    TrieNode* children[26];

    // Marks if this node completes a word
    bool isEndOfWord;
    int frequency;
    int unifreq;

    // Constructor
    TrieNode()
    {
        isEndOfWord = false;
        frequency = 0;
        unifreq = 0;
        for (int i = 0; i < 26; i++)
        {
            children[i] = nullptr;
        }
    }
};

// The Trie class
class Trie
{
private:
    TrieNode* root;

    // Helper function to find all words from a node
    // Input: current node, current word formed so far, results vector to store words
    // Output: none (modifies results vector by reference)
    // Purpose: Recursively find all complete words starting from the given node
    void findAllWords(TrieNode* node, string currentWord, vector<pair<string, int>>& results)
    {
        if (node->isEndOfWord)
        {
            results.push_back({ currentWord , node->frequency });
        }

        for (int i = 0; i < 26; i++)
        {
            if (node->children[i] != nullptr)
            {
                char nxt = 'a' + i;
                findAllWords(node->children[i], currentWord + nxt, results);
            }
        }
    }

public:
    // Constructor
    // Input: none
    // Output: none
    // Purpose: Initialize the Trie with a root node
    Trie()
    {
        root = new TrieNode();
    }

    // Insert a word into the Trie
    // Input: word to insert (string)
    // Output: none
    // Purpose: Add a word to the Trie by creating nodes for each character
    bool insert(string word)
    {
        TrieNode* curr = root;

        for (char c : word)
        {
            c = tolower(c);

            if (curr->children[c - 'a'] == nullptr)
            {
                curr->children[c - 'a'] = new TrieNode();
            }

            curr = curr->children[c - 'a'];
        }

        curr->unifreq++;
        if (curr->isEndOfWord) {
            // Word already existed
            return false;  // duplicate
        }
        else {
            curr->isEndOfWord = true;
            return true;   // unique
        }
    }

    int getFrequency(string word) {
        TrieNode* node = root;
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) return 0;
            node = node->children[idx];
        }
        if (node->isEndOfWord)
            return node->unifreq;
        return 0;

    }
    // Search for a word in the Trie
    // Input: word to search for (string)
    // Output: boolean indicating if the word exists
    // Purpose: Check if the complete word exists in the Trie
    bool search(string word)
    {
        TrieNode* node = root;

        for (char c : word)
        {
            c = tolower(c);
            int index = c - 'a';

            if (index < 0 || index >= 26)
                return false;

            if (!node->children[index])
                return false;

            node = node->children[index];
        }

        if (node->isEndOfWord)
        {
            node->frequency++;
            return true;
        }
        return false;
    }

    // Check if any word starts with the given prefix
    // Input: prefix to check (string)
    // Output: boolean indicating if any word has this prefix
    // Purpose: Verify if the prefix exists in the Trie (doesn't need to be a complete word)
    bool startsWith(string prefix)
    {
        TrieNode* current = root;

        for (char c : prefix)
        {
            if (!isalpha(c))
                return false;

            c = tolower(c);
            int index = c - 'a';

            if (current->children[index] == nullptr)
            {
                return false;
            }

            current = current->children[index];
        }

        return true;
    }


    static bool cmp(const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    }
    // Get all words that start with the given prefix
    // Input: prefix to complete (string)
    // Output: vector of strings that start with the prefix
    // Purpose: Find all complete words that begin with the given prefix
    vector<pair<string, int>> autocomplete(string prefix)
    {
        // 1. Create a ptr of type TrieNode and set it to root.
        // 2. Traverse to get the start node to collect words from.
        // 3. Check validity of prefix characters and existence in Trie.
        // 4. Use helper to collect all words from that node & return.

        vector<pair<string, int>> suggestions;
        TrieNode* node = root;

        for (int i = 0; i < prefix.size(); i++)
        {
            int charIndex = tolower(prefix[i]) - 'a';

            // To easily modify according to the supported chars.
            if (charIndex < 0 || charIndex >= 26)
            {
                return {}; // Invalid character
            }

            // To handle uninserted characters.
            if (!node->children[charIndex])
            {
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
};

// Main function
// Input: none
// Output: integer return code
// Purpose: Program entry point, run tests and interactive demo
int main()
{
    cout << "=== TRIE DATA STRUCTURE IMPLEMENTATION ===" << endl;
    cout << "Testing all Trie functionalities..." << endl;

    Trie trie;

    // Test 1: Basic insertion and search
    cout << "\n1. Testing basic insertion and search:" << endl;
    cout << "======================================" << endl;

    vector<string> words = { "apple", "banana", "orange", "grape", "kiwi" ,"apple" };
    for (const string& word : words)
    {
        if (trie.insert(word)) {
            cout << "Inserted: " << word << " (unique)" << endl;
        }
        else {
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
    // Test search for existing words
    for (const string& word : words)
    {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    // Test search for non-existing words
    vector<string> nonExisting = { "app", "ban", "ora", "graph", "kiwis" };
    for (const string& word : nonExisting)
    {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;
    }

    // Test 2: Prefix checking
    cout << "\n2. Testing prefix checking:" << endl;
    cout << "==========================" << endl;

    vector<string> prefixes = { "app", "ban", "ora", "grap", "k" };
    for (const string& prefix : prefixes)
    {
        bool hasPrefix = trie.startsWith(prefix);
        cout << "Prefix '" << prefix << "': " << (hasPrefix ? "EXISTS" : "DOESN'T EXIST") << endl;
    }

    vector<string> nonPrefixes = { "x", "yield", "zed", "micro", "nano" };
    for (const string& prefix : nonPrefixes)
    {
        bool hasPrefix = trie.startsWith(prefix);
        cout << "Prefix '" << prefix << "': " << (hasPrefix ? "EXISTS" : "DOESN'T EXIST") << " (expected: DOESN'T EXIST)" << endl;
    }

    // Test 3: Autocomplete functionality
    cout << "\n3. Testing autocomplete functionality:" << endl;
    cout << "======================================" << endl;

    vector<string> testPrefixes = { "a", "b", "o", "g", "k", "ap", "ban", "ora", "gr", "ki" };
    for (const string& prefix : testPrefixes)
    {
        vector<pair<string, int>> suggestions = trie.autocomplete(prefix);
        cout << "Autocomplete for '" << prefix << "': ";
        if (suggestions.empty())
        {
            cout << "No suggestions found";
        }
        else
        {
            for (size_t i = 0; i < suggestions.size(); i++)
            {
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

    vector<pair<string, int>> emptySuggestions = trie.autocomplete("");
    cout << "Autocomplete for empty string: ";
    if (emptySuggestions.empty())
    {
        cout << "No suggestions found";
    }
    else
    {
        for (size_t i = 0; i < emptySuggestions.size(); i++)
        {
            if (i > 0)
                cout << ", ";
            cout << emptySuggestions[i].first << "--->" << emptySuggestions[i].second;
        }
    }
    cout << " (expected: all words)" << endl;

    // Test 5: Adding more words and retesting
    cout << "\n5. Testing with additional words:" << endl;
    cout << "================================" << endl;

    vector<string> additionalWords = { "application", "appetizer","application", "banister","oracle", "bandana", "oracle", "grapefruit" };
    for (const string& word : additionalWords)
    {
        if (trie.insert(word)) {
            cout << "Inserted: " << word << " (unique)" << endl;
        }
        else {
            cout << "Inserted: " << word << " (duplicate)" << endl;
        }
    }

    found = trie.search("bandana");
    cout << "Search 'bandana': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;

    found = trie.search("application");
    cout << "Search 'application': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;

    // Test search for new words
    for (const string& word : additionalWords)
    {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    // Test autocomplete with new words
    vector<string> newPrefixes = { "app", "ban", "ora", "gra" };
    for (const string& prefix : newPrefixes)
    {
        vector<pair<string, int>> suggestions = trie.autocomplete(prefix);
        cout << "Autocomplete for '" << prefix << "': ";
        if (suggestions.empty())
        {
            cout << "No suggestions found";
        }
        else
        {
            for (size_t i = 0; i < suggestions.size(); i++)
            {
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

    vector<string> caseWords = { "hello", "Hello", "WORLD", "world" };
    for (const string& word : caseWords)
    {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    cout << "\n=== ALL TESTS COMPLETED ===" << endl;

    return 0;
}

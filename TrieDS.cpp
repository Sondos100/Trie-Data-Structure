#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <stack>

using namespace std;

// Each node in the Trie
class TrieNode
{
public:
    // Each node has up to 26 children (for each letter)
    TrieNode *children[26];
    // Each node has a map to store children for extended characters
    unordered_map<char, TrieNode *> mapchildren;

    // Marks if this node completes a word
    bool isEndOfWord;
    int searchFreq;
    int insertFreq;

    // Constructor
    TrieNode()
    {
        isEndOfWord = false;
        searchFreq = 0;
        insertFreq = 0;
        for (int i = 0; i < 26; i++)
        {
            children[i] = nullptr;
        }
    }

    bool hasChildren()
    {
        for (int i = 0; i < 26; i++)
        {
            if (children[i] != nullptr)
            {
                return true;
            }
        }
        return !mapchildren.empty();
    }
};

// The Trie class
class Trie
{
private:
    TrieNode *root;

    // Helper function to find all words from a node
    // Input: current node, current word formed so far, results vector to store words
    // Output: none (modifies results vector by reference)
    // Purpose: Recursively find all complete words starting from the given node
    void findAllWords(TrieNode *node, string currentWord, vector<pair<string, int>> &results)
    {
        if (!node)
            return;

        if (node->isEndOfWord)
        {
            results.push_back({currentWord, node->searchFreq});
        }

        for (int i = 0; i < 26; i++)
        {
            if (node->children[i] != nullptr)
            {
                char nxt = 'a' + i;
                findAllWords(node->children[i], currentWord + nxt, results);
            }
        }

        for (auto &entry : node->mapchildren)
        {
            findAllWords(entry.second, (currentWord + entry.first), results);
        }
    }

    // Helper function to delete a word from the Trie
    // Input: current node, word to delete, current depth in the word
    // Output: boolean indicating whether the current node can be safely deleted
    // Purpose: Recursively remove the word from the Trie and clean up unused nodes
    bool deleteHelper(TrieNode *node, const string &word, int depth)
    {
        // If the node is a nullptr
        if (!node)
        {
            return false;
        }

        // Reached the end of the word
        if (depth == word.size())
        {
            if (!node->isEndOfWord)
                return false; // Word not found

            node->isEndOfWord = false;
            node->insertFreq--;

            // Node can be deleted if it has no children
            return !node->hasChildren();
        }
        //------------------------------------------------------------------

        char c = word[depth];
        TrieNode *childNode = nullptr;
        bool isMapped = false; // Track whether we are using mapchildren

        // Handle standard lowercase characters
        if (c >= 'a' && c <= 'z')
        {
            childNode = node->children[c - 'a'];
        }
        else
        {
            // Extended characters handled here
            if (node->mapchildren.count(c))
            {
                childNode = node->mapchildren[c];
                isMapped = true;
            }
        }

        // call the function for the child node
        // true if the child node can be deleted
        bool shouldDeleteChild = deleteHelper(childNode, word, depth + 1);

        /*
        if (wordExisted)
            node->unifreq--;
        */

        // If child should be deleted, handle the deletion here
        if (shouldDeleteChild)
        {
            delete childNode;

            // Choose the right place to remove from
            if (isMapped)
            {
                node->mapchildren.erase(c);
            }
            else
            {
                node->children[c - 'a'] = nullptr;
            }
        }

        // Parent node can be deleted only if it's not a word AND has no children.
        return !node->isEndOfWord && !node->hasChildren();
    }

    // Helper function to count words in the Trie
    // Input: current node
    // Output: integer count of words in the subtrie rooted at this node
    // Purpose: Recursively count all unique words stored in the Trie
    int countWordsHelper(TrieNode *node)
    {
        // 1. If node is null, return 0.
        // 2. Add 1 if this node marks the end of a word.
        // 3. Recursively check all 26 children.
        // 4. Return the total count of words.

        if (!node)
            return 0;

        int count = node->isEndOfWord; // bool -> 0 or 1

        for (int i = 0; i < 26; i++)
        {
            if (node->children[i] != nullptr)
            {
                count += countWordsHelper(node->children[i]);
            }
        }

        for (auto &entry : node->mapchildren)
        {
            count += countWordsHelper(entry.second);
        }

        return count;
    }

    // Helper function to find the longest word from a list of words
    // Input: vector of word-frequency pairs
    // Output: the longest word (string)
    // Purpose: Identify the longest word from the provided list, breaking ties lexicographically
    string LongestPrefixHelper(vector<pair<string, int>> &words)
    {
        string best = "";

        for (auto &entry : words)
        {
            const string &word = entry.first;
            if (word.length() > best.length() || (word.length() == best.length() && word < best))
                best = word;
        }

        return best;
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
        TrieNode *curr = root;

        for (char c : word)
        {
            if (c >= 'a' && c <= 'z')
            {
                if (curr->children[c - 'a'] == nullptr)
                {
                    curr->children[c - 'a'] = new TrieNode();
                }
                curr = curr->children[c - 'a'];
            }
            else
            {
                if (!curr->mapchildren[c])
                {
                    curr->mapchildren[c] = new TrieNode();
                }
                curr = curr->mapchildren[c];
            }
        }

        curr->insertFreq++;
        if (curr->isEndOfWord)
        {
            // Word already existed
            return false; // duplicate
        }
        curr->isEndOfWord = true;
        return true; // unique
    }

    // Get the frequency of a word in the Trie
    // Input: word to check (string)
    // Output: frequency count (integer)
    // Purpose: Return how many times the word has been inserted into the Trie
    int getFrequency(string word)
    {
        TrieNode *node = root;
        for (char c : word)
        {
            if (c >= 'a' && c <= 'z')
            {
                if (!node->children[c - 'a'])
                    return 0;

                node = node->children[c - 'a'];
            }
            else
            {
                if (!node->mapchildren.count(c))
                    return 0;

                node = node->mapchildren[c];
            }
        }

        if (node->isEndOfWord)
            return node->insertFreq;

        return 0;
    }

    // Count the total number of unique words in the Trie
    // Input: none
    // Output: integer count of unique words
    // Purpose: Traverse the Trie and count how many unique words are stored
    int countWords()
    {
        // 1. Start counting words from the root node.
        // 2. Pass the root to the recursive helper.

        return countWordsHelper(root);
    }

    // Count how many words start with the given prefix
    // Input: prefix to check (string)
    // Output: integer count of words with the prefix
    // Purpose: Find how many complete words begin with the given prefix
    int prefixCount(const string &prefix)
    {
        TrieNode *node = root;
        vector<pair<string, int>> result;

        for (int i = 0; i < prefix.size(); i++)
        {
            int charIndex = prefix[i];

            // To easily modify according to the supported chars.
            if (charIndex >= 'a' && charIndex <= 'z')
            {
                if (!node->children[charIndex - 'a'])
                    return 0;

                node = node->children[charIndex - 'a'];
            }
            else
            {
                // Case Sensitive Search
                if (!node->mapchildren.count(charIndex))
                    return 0;

                node = node->mapchildren[charIndex];
            }
        }

        findAllWords(node, prefix, result);

        return result.size();
    }

    // Return all words in lexicographical order
    // Input: none
    // Output: vector of words in lexicographical order
    // Purpose: List all words stored in the Trie in sorted order
    vector<pair<string, int>> Lexicographical()
    {
        vector<pair<string, int>> result;
        TrieNode *node = root;
        string curr = "";

        findAllWords(node, curr, result);

        return result;
    }

    // Search for a word in the Trie
    // Input: word to search for (string)
    // Output: boolean indicating if the word exists
    // Purpose: Check if the complete word exists in the Trie
    bool search(string word)
    {
        TrieNode *node = root;

        for (char c : word)
        {
            // c = tolower(c);
            // Case Sensitive Search
            if (c >= 'a' && c <= 'z')
            {
                int index = c - 'a';
                if (!node->children[index])
                    return false;
                node = node->children[index];
            }
            else
            {
                if (!node->mapchildren.count(c))
                    return false;
                node = node->mapchildren[c];
            }
        }

        if (node->isEndOfWord)
        {
            node->searchFreq++;
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
        TrieNode *current = root;

        for (char c : prefix)
        {
            // c = tolower(c);
            //  Case Sensitive Search
            if (c >= 'a' && c <= 'z')
            {
                int index = c - 'a';

                if (!current->children[index])
                    return false;

                current = current->children[index];
            }
            else
            {
                if (!current->mapchildren.count(c))
                    return false;

                current = current->mapchildren[c];
            }
        }

        return true;
    }

    static bool cmp(const pair<string, int> &a, const pair<string, int> &b)
    {
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
        // 5. Sort suggestions by frequency (highest first).

        vector<pair<string, int>> suggestions;

        TrieNode *node = root;

        for (int i = 0; i < prefix.size(); i++)
        {
            int charIndex = prefix[i];

            // To easily modify according to the supported chars.
            if ((charIndex >= 'a') && (charIndex <= 'z'))
            {
                charIndex = prefix[i] - 'a';

                if (!node->children[charIndex])
                    return {};

                node = node->children[charIndex];
            }

            // To handle characters other than lowercase.
            // Case Insensitive
            else
            {
                // Try mapchildren first
                if (node->mapchildren.count(charIndex))
                {
                    node = node->mapchildren[charIndex];
                }
                else
                {
                    // Try lowercase children as fallback
                    int index = tolower(charIndex) - 'a';
                    if (index < 0 || index >= 26 || !node->children[index])
                        return {}; // prefix not found
                    node = node->children[index];
                }
            }
        }

        // Collect all words starting from this node.
        findAllWords(node, prefix, suggestions);

        // Sort suggestions by frequency (highest first).
        sort(suggestions.begin(), suggestions.end(), cmp);

        return suggestions;
    }

    // Find the longest word in the Trie
    // Input: none
    // Output: the longest word (string)
    // Purpose: Traverse the Trie to find the longest word stored
    string findLongestWord()
    {
        string best = "";
        stack<pair<TrieNode *, string>> st;
        st.push({root, ""});

        while (!st.empty())
        {
            pair<TrieNode *, string> topPair = st.top();
            TrieNode *node = st.top().first;
            string curr = st.top().second;
            st.pop();

            if (node->isEndOfWord)
            {
                if (curr.size() > best.size() || (curr.size() == best.size() && curr < best))
                    best = curr;
            }

            for (int i = 25; i >= 0; i--)
            {
                if (node->children[i])
                {
                    char c = 'a' + i;
                    st.push({node->children[i], curr + c});
                }
            }

            // Doesn't guarantee lexicographical order for mapchildren
            for (auto &entry : node->mapchildren)
            {
                st.push({entry.second, curr + entry.first});
            }
        }

        return best;
    }

    // Get the longest word that starts with the given prefix
    // Input: prefix to match (string)
    // Output: the longest matching word (string)
    // Purpose: Find the longest complete word that begins with the given prefix
    string longestWithPrefix(const string &prefix)
    {
        vector<pair<string, int>> words = autocomplete(prefix);
        string match = LongestPrefixHelper(words);

        return match;
    }

    // Find the longest prefix of the target that exists in the Trie
    // Input: target string to match
    // Output: the longest matching prefix (string)
    // Purpose: Identify the longest prefix of the target string that is a complete word in the Trie
    // N.B. This is different from longestWithPrefix which finds words starting with a given prefix.
    // N.B. This function is used in applications like IP routing.
    string LongestPrefixMatch(const string &target)
    {
        TrieNode *node = root;
        string currentPrefix = "";
        string longestPrefix = "";

        for (char c : target)
        {
            // Case Sensitive Search
            if (c >= 'a' && c <= 'z')
            {
                if (!node->children[c - 'a'])
                    break;

                node = node->children[c - 'a'];
            }
            else
            {
                if (!node->mapchildren.count(c))
                    break;

                node = node->mapchildren[c];
            }

            currentPrefix += c;

            if (node->isEndOfWord)
            {
                longestPrefix = currentPrefix;
            }
        }

        return longestPrefix;
    }

    // Delete a word from the Trie
    // Input: word to delete (string)
    // Output: boolean indicating if deletion was successful
    // Purpose: Removes the word from the Trie and cleans up unused nodes
    bool deleteWord(string word)
    {
        // Handle empty strings
        if (word.empty())
            return false;

        if (!search(word))
            return false; // no need to call helper

        // Call helper function to handle recursive deletion
        bool deleteMe = deleteHelper(root, word, 0);

        return deleteMe;
    }

    // Clear the Trie to free memory
    // Input: node to clear
    // Output: void (none)
    // Purpose: Recursively delete all nodes in the Trie
    void clear(TrieNode *node)
    {
        if (!node)
            return;

        // Delete all 26 fixed children
        for (int i = 0; i < 26; i++)
        {
            if (node->children[i])
            {
                clear(node->children[i]);
            }
        }

        // Delete all map-based children
        for (auto &pair : node->mapchildren)
        {
            clear(pair.second);
        }

        // Delete current node
        delete node;
    }

    // Destructor
    // Input: none
    // Output: none
    // Purpose: Free all allocated memory when Trie is destroyed
    ~Trie()
    {
        clear(root);
        root = new TrieNode();
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

    vector<string> words = {"apple", "banana", "orange", "grape", "kiwi", "apple"};

    for (const string &word : words)
    {
        if (trie.insert(word))
        {
            cout << "Inserted: " << word << " (unique)" << endl;
        }
        else
        {
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
    for (const string &word : words)
    {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    // Test search for non-existing words
    vector<string> nonExisting = {"app", "ban", "ora", "graph", "kiwis"};
    for (const string &word : nonExisting)
    {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;
    }

    // Test 2: Prefix checking
    cout << "\n2. Testing prefix checking:" << endl;
    cout << "==========================" << endl;

    vector<string> prefixes = {"app", "ban", "ora", "grap", "k"};
    for (const string &prefix : prefixes)
    {
        bool hasPrefix = trie.startsWith(prefix);
        cout << "Prefix '" << prefix << "': " << (hasPrefix ? "EXISTS" : "DOESN'T EXIST") << endl;
    }

    vector<string> nonPrefixes = {"x", "yield", "zed", "micro", "nano"};
    for (const string &prefix : nonPrefixes)
    {
        bool hasPrefix = trie.startsWith(prefix);
        cout << "Prefix '" << prefix << "': " << (hasPrefix ? "EXISTS" : "DOESN'T EXIST") << " (expected: DOESN'T EXIST)" << endl;
    }

    // Test 3: Autocomplete functionality
    cout << "\n3. Testing autocomplete functionality:" << endl;
    cout << "======================================" << endl;

    vector<string> testPrefixes = {"a", "b", "o", "g", "k", "ap", "ban", "ora", "gr", "ki"};
    for (const string &prefix : testPrefixes)
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

    vector<string> additionalWords = {
        "application", "appetizer", "application", "banister", "oracle", "bandana", "oracle", "grapefruit"};

    for (const string &word : additionalWords)
    {
        if (trie.insert(word))
        {
            cout << "Inserted: " << word << " (unique)" << endl;
        }
        else
        {
            cout << "Inserted: " << word << " (duplicate)" << endl;
        }
    }

    found = trie.search("bandana");
    cout << "Search 'bandana': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;

    found = trie.search("application");
    cout << "Search 'application': " << (found ? "FOUND" : "NOT FOUND") << "  frequency  = 1 " << endl;

    cout << "Unique Word Count :   " << trie.countWords() << endl;

    // Test search for new words
    for (const string &word : additionalWords)
    {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    // Test autocomplete with new words
    vector<string> newPrefixes = {"app", "ban", "ora", "gra"};
    for (const string &prefix : newPrefixes)
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
    cout << "Inserted: Hello" << endl;
    cout << "Inserted: WORLD" << endl;
    cout << "Unique Word Count :   " << trie.countWords() << endl;

    vector<string> caseWords = {"hello", "Hello", "WORLD", "world"};
    for (const string &word : caseWords)
    {
        bool found = trie.search(word);
        cout << "Search '" << word << "': " << (found ? "FOUND" : "NOT FOUND") << endl;
    }

    // Test 7: Lexicographical Word Listing
    cout << "\n7. Lexicographical Word Listing:" << endl;
    cout << "============================" << endl;

    vector<pair<string, int>> result = trie.Lexicographical();
    cout << "[ " << result[0].first;
    for (int i = 1; i < result.size(); i++)
    {
        cout << " , " << result[i].first;
    }
    cout << " ]" << endl;

    cout << "\n8. Prefix Word Count:" << endl;
    cout << "============================" << endl;

    cout << "Prefix 'app' ->  Count: " << trie.prefixCount("app") << "  // Expected: 3 -> appetizer, apple, application"
         << endl;
    cout << "Prefix 'ban' ->  Count: " << trie.prefixCount("ban") << "  // Expected: 3 -> banana, bandana, banister" << endl;
    cout << "Prefix 'gra' ->  Count: " << trie.prefixCount("gra") << "  // Expected: 2 -> grape, grapefruit" << endl;
    cout << "Prefix 'or' ->   Count: " << trie.prefixCount("or") << "  // Expected: 2 -> oracle, orange" << endl;
    cout << "Prefix 'he' ->   Count: " << trie.prefixCount("he") << "  // Expected: 0 -> no word" << endl;
    cout << "Prefix 'ki' ->   Count: " << trie.prefixCount("ki") << "  // Expected: 1 -> kiwi" << endl;
    cout << "Prefix 'w' ->    Count: " << trie.prefixCount("w") << "  // Expected: 0 -> no word" << endl;
    cout << "Prefix 'z' ->    Count: " << trie.prefixCount("z") << "  // Expected: 0 -> no word" << endl;
    cout << "Prefix 'appl' -> Count: " << trie.prefixCount("appl") << "  // Expected: 2 -> apple, application" << endl;
    cout << "Prefix '' ->     Count: " << trie.prefixCount("") << "  // Expected: 13 -> all words in Trie" << endl;

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
    cout << "Search for 'apple': " << (trie.search("apple") ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;
    cout << "Search for 'application': " << (trie.search("application") ? "FOUND" : "NOT FOUND") << " (expected: FOUND)"
         << endl;
    cout << "Search for 'appetizer': " << (trie.search("appetizer") ? "FOUND" : "NOT FOUND") << " (expected: FOUND)" << endl;
    cout << "Prefix 'app' count after deleting 'apple': " << trie.prefixCount("app") << " (expected: 2)" << endl;

    // Case 2: Delete a word that is an isolated branch (e.g., "banana")
    cout << "\nDeleting 'banana'..." << endl;
    deleted = trie.deleteWord("banana");
    cout << "Deletion status: " << (deleted ? "SUCCESS" : "FAILED") << endl;
    cout << "Search for 'banana': " << (trie.search("banana") ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;

    // Case 3: Delete a word that has a prefix also as a word (e.g., "application")
    cout << "\nDeleting 'application'..." << endl;
    deleted = trie.deleteWord("application");
    cout << "Deletion status: " << (deleted ? "SUCCESS" : "FAILED") << endl;
    cout << "Search for 'application': " << (trie.search("application") ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;
    cout << "Search for 'appetizer': " << (trie.search("appetizer") ? "FOUND" : "NOT FOUND") << " (expected: FOUND)" << endl;
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
    cout << "Search for 'hello': " << (trie.search("hello") ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;

    cout << "Unique Word Count after all deletions: " << trie.countWords() << endl;

    cout << "\n10. Testing longest word:" << endl;
    cout << "============================" << endl;

    string longest = trie.findLongestWord();
    if (longest.empty())
        cout << "Trie is empty, no longest word.\n";
    else
        cout << "Longest word in Trie: " << longest
             << " (length = " << longest.size() << ")" << endl;

    cout << "\n11. Testing longest word with prefix:" << endl;
    cout << "========================================" << endl;
    vector<string> testLPM = {"app", "ban", "Or", "gra", "unknown", "banana", "appl", ""};
    for (const string &testStr : testLPM)
    {
        string lpm = trie.longestWithPrefix(testStr);
        cout << "Longest prefix match for '" << testStr << "': '" << lpm << "'" << endl;
    }

    // Test 8: Longest Prefix Match (used in IP routing and predictive systems)
    cout << "\n12. Testing Longest Prefix Match:" << endl;
    cout << "=================================" << endl;

    Trie lpmTrie;

    // Insert words that could form prefixes
    vector<string> prefixWords = {
        "a", "app", "apple", "application", "apply",
        "ban", "banana", "band", "bandana", "b",
        "cater", "cat", "catalog", "dog", "do"};

    for (const string &word : prefixWords)
    {
        lpmTrie.insert(word);
        cout << "Inserted: " << word << endl;
    }

    // Words to test longest prefix matching
    vector<string> targets = {
        "applicationform", // Should match "application"
        "applynow",        // Should match "apply"
        "applepie",        // Should match "apple"
        "bananafish",      // Should match "banana"
        "bandwidth",       // Should match "band"
        "bandanastyle",    // Should match "bandana"
        "caterpillar",     // Should match "cater"
        "catalogue",       // Should match "catalog"
        "doormat",         // Should match "do"
        "doghouse",        // Should match "dog"
        "zebra",           // Should match ""
        "",                // Should match ""
        "a",               // Should match "a"
        "b",               // Should match "b"
        "c",               // Should match "" (no exact "c")
    };

    for (const string &target : targets)
    {
        string match = lpmTrie.LongestPrefixMatch(target);
        cout << "Longest prefix match for '" << target << "': '" << match << "'" << endl;
    }

    cout << "\n13. Testing memory cleanup (destructor):" << endl;
    cout << "========================================" << endl;
    {
        Trie testTrie;
        testTrie.insert("clear");
        testTrie.insert("clean");
        testTrie.insert("clap");
        testTrie.insert("clash");

        cout << "Inserted 4 words: clear, clean, clap, clash" << endl;

        cout << "Word count before clearing: " << testTrie.countWords() << " (expected: 4)" << endl;

        // Perform clearing
        testTrie.~Trie();

        cout << "Cleared the Trie using destructor." << endl;

        // After clearing, Trie should be empty
        cout << "Word count after clearing: " << testTrie.countWords() << " (expected: 0)" << endl;

        bool found = testTrie.search("clear");
        cout << "Search 'clear' after clearing: " << (found ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;

        found = testTrie.search("clean");
        cout << "Search 'clean' after clearing: " << (found ? "FOUND" : "NOT FOUND") << " (expected: NOT FOUND)" << endl;

        // Ensure insert still works after clear
        testTrie.insert("clean");
        found = testTrie.search("clean");
        cout << "Insert and Search 'clean' again: " << (found ? "FOUND" : "NOT FOUND") << " (expected: FOUND)" << endl;

        cout << "Word count after reinserting 'clean': " << testTrie.countWords() << " (expected: 1)" << endl;
    }

    cout << "\n=== ALL TESTS COMPLETED ===" << endl;

    return 0;
}

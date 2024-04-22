#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <stdexcept>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

// I am creating a templated hash table class using linear probing for collision resolution.
template<typename K, typename V>
class HashTableLinearProbing {
private:
    struct Entry {
        K key;                // The key of the entry
        V value;              // The value associated with the key
        bool occupied = false;// Flag to indicate if the slot has been occupied
        bool active = true;   // Flag to check if the slot is actively used or marked as deleted

        // Default constructor for an empty entry
        Entry() : occupied(false), active(false) {}

        // Constructor to initialize an entry with a key and a value
        Entry(K k, V v) : key(k), value(v), occupied(true), active(true) {}
    };

    vector<Entry> table;      // The table is a vector of entries
    int capacity;             // Maximum number of entries in the hash table
    int size;                 // Current number of active entries

    // This function calculates the index for a key using the standard hash function and modulo operation.
    int hashFunction(K key) const {
        hash<K> hashObj;
        return hashObj(key) % capacity;
    }

public:
    // Constructor to initialize the hash table with a specified capacity.
    HashTableLinearProbing(int capacity = 15000) : capacity(capacity), size(0), table(capacity) {}

    // Method to insert a key-value pair into the hash table.
    void insert(K key, V value) {
        int index = hashFunction(key); // Calculate the index using the hash function.
        int start_index = index;       // Remember the start index to detect when we've looped through the entire table.

        // Keep probing linearly until an empty or deletable spot is found.
        while (table[index].occupied && table[index].key != key) {
            index = (index + 1) % capacity; // Move to the next index.
            if (index == start_index) {     // If we return to the start, the table is full.
                throw overflow_error("Hash table is full");
            }
        }

        table[index] = Entry(key, value);  // Place the entry in the found spot.
        if (!table[index].active) {        // If the spot was previously deactivated, reactivate it.
            table[index].active = true;
            size++;
        }
    }

    // Method to retrieve the value associated with a key.
    V retrieve(K key) {
        int index = hashFunction(key);   // Calculate the index.
        int start_index = index;         // Store the start index to avoid infinite loops.

        // Probe until an unoccupied slot or the full loop around the table is detected.
        while (table[index].occupied) {
            if (table[index].key == key && table[index].active) { // Check if the active key matches.
                return table[index].value;
            }
            index = (index + 1) % capacity;
            if (index == start_index) { // If we've checked the whole table, the key isn't here.
                break;
            }
        }

        throw runtime_error("Key not found");  // Key was not found.
    }

    // Method to remove an entry by key.
    bool remove(K key) {
        int index = hashFunction(key);   // Find the index for the key.
        int start_index = index;         // Remember the start index.

        // Probe until we find an unoccupied slot or circle back to the start.
        while (table[index].occupied) {
            if (table[index].key == key && table[index].active) {
                table[index].active = false; // Deactivate the entry.
                size--;
                return true;  // Successfully removed.
            }
            index = (index + 1) % capacity;
            if (index == start_index) {
                break;
            }
        }

        return false;  // The key was not found for removal.
    }

    // Method to perform performance tests on the hash table operations.
    void performTest(int numOperations) {
        vector<string> keys(numOperations);
        vector<int> values(numOperations);
        random_device rd;  // Random device to seed the generator for random keys and values.
        mt19937 eng(rd());
        uniform_int_distribution<> distr(100000, 999999);

        // Record the start time of the insert operations.
        auto insert_start = high_resolution_clock::now();
        for (int i = 0; i < numOperations; ++i) {
            keys[i] = "key" + to_string(distr(eng));  // Generate random keys.
            values[i] = distr(eng);                  // Generate random values.
            insert(keys[i], values[i]);              // Insert them into the hash table.
        }
        auto insert_end = high_resolution_clock::now();  // Record the end time.

        // Record the start time of the retrieve operations.
        auto retrieve_start = high_resolution_clock::now();
        for (int i = 0; i < numOperations; ++i) {
            retrieve(keys[i]);  // Retrieve each key to measure performance.
        }
        auto retrieve_end = high_resolution_clock::now();  // Record the end time.

        // Start timing the remove operations.
        auto remove_start = high_resolution_clock::now();
        for (int i = 0; i < numOperations; ++i) {
            remove(keys[i]);  // Remove each key to measure performance.
        }
        auto remove_end = high_resolution_clock::now();  // Record the end time.

        // Calculate and print the durations of each operation.
        auto insert_duration = duration_cast<milliseconds>(insert_end - insert_start);
        auto retrieve_duration = duration_cast<milliseconds>(retrieve_end - retrieve_start);
        auto remove_duration = duration_cast<milliseconds>(remove_end - remove_start);

        cout << "Performance for " << numOperations << " operations:" << endl;
        cout << "Insert Duration: " << insert_duration.count() << " ms" << endl;
        cout << "Retrieve Duration: " << retrieve_duration.count() << " ms" << endl;
        cout << "Remove Duration: " << remove_duration.count() << " ms" << endl;
    }

    // Displays the menu for interacting with the hash table from the command line.
    void displayMenu() {
        cout << "HASH TABLE OPERATIONS\n";
        cout << "1. Insert Key/Value\n";
        cout << "2. Retrieve Value by Key\n";
        cout << "3. Remove Key\n";
        cout << "4. Performance Test\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
    }
};

int main() {
    HashTableLinearProbing<string, int> hashTable(15000);  // capacity

    int choice;

    while (true) {
        hashTable.displayMenu();
        cin >> choice;

        switch (choice) {
        case 1: {
            string key;
            int value;
            cout << "Enter key: ";
            cin >> key;
            cout << "Enter value: ";
            cin >> value;
            try {
                hashTable.insert(key, value);
                cout << "Inserted (" << key << ", " << value << ") into the Hash Table.\n";
            }
            catch (const overflow_error& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        }
        case 2: {
            string key;
            cout << "Enter key: ";
            cin >> key;
            try {
                int value = hashTable.retrieve(key);
                cout << "Value at key '" << key << "' is " << value << ".\n";
            }
            catch (const runtime_error& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        }
        case 3: {
            string key;
            cout << "Enter key: ";
            cin >> key;
            if (hashTable.remove(key)) {
                cout << "Key '" << key << "' has been removed.\n";
            }
            else {
                cout << "Key '" << key << "' not found.\n";
            }
            break;
        }
        case 4: {
            int numTests;
            cout << "Enter number of operations for performance testing (e.g., 100, 1000, 10000): ";
            cin >> numTests;
            hashTable.performTest(numTests);
            break;
        }
        case 5:
            cout << "Exiting program.\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    }

    return 0;
}

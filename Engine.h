#ifndef ENGINE_H
#define ENGINE_H

#include <algorithm>
#include <iostream>
#include <vector>

#include "BST.h"
#include "Record.h"
// add header files as needed

using namespace std;

// Converts a string to lowercase (used for case-insensitive searches)
static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ================== Index Engine ==================
// Acts like a small "database engine" that manages records and two BST indexes:
// 1) idIndex: maps student_id → record index (unique key)
// 2) lastIndex: maps lowercase(last_name) → list of record indices (non-unique key)
struct Engine {
        vector<Record> heap;    // the main data store (simulates a heap file)
        BST<int, int> idIndex;  // index by student ID
        BST<string, vector<int>> lastIndex;  // index by last name (can have duplicates)

        // Inserts a new record and updates both indexes.
        // Returns the record ID (RID) in the heap.
        int insertRecord(const Record &recIn) {
            const int id = recIn.id;
            // add to heap
            int rid = (int)heap.size();
            heap.push_back(recIn);

            // append to id index
            idIndex.insert(id, rid);

            // append to last name index
            string lastNameAsKey = toLower(recIn.last);
            vector<int> *rids = lastIndex.find(lastNameAsKey);
            if (rids) {
                rids->push_back(id);
            } else {
                // create new list if there is no such key with last name
                lastIndex.insert(lastNameAsKey, vector<int>{id});
            }

            return rid;
        }

        // Deletes a record logically (marks as deleted and updates indexes)
        // Returns true if deletion succeeded.
        bool deleteById(int id) {
            const int *heapIndexPtr = idIndex.find(id);
            if (!heapIndexPtr) return false;
            const int heapIndex = *heapIndexPtr;
            if (heapIndex < 0 || heapIndex >= (int)heap.size()) return false;

            // soft delete
            if (heap[heapIndex].deleted) return false;
            Record &rec = heap[heapIndex];
            rec.deleted = true;

            // remove from id index
            if (!idIndex.erase(id)) return false;
            // remove from last name index
            string lastNameAsKey = toLower(rec.last);
            // list of heap index of all records that has same last name
            vector<int> *rids = lastIndex.find(lastNameAsKey);
            if (!rids) return false;
            rids->erase(std::remove(rids->begin(), rids->end(), heapIndex), rids->end());
            if (rids->empty()) lastIndex.erase(lastNameAsKey);

            return true;
        }

        // Finds a record by student ID.
        // Returns a pointer to the record, or nullptr if not found.
        // Outputs the number of comparisons made in the search.
        const Record *findById(int id, int &cmpOut) {
            idIndex.resetMetrics();
            const int *heapIndexPtr = idIndex.find(id);
            cmpOut = idIndex.comparisons;
            if (!heapIndexPtr) return nullptr;
            const int heapIndex = *heapIndexPtr;
            if (heapIndex < 0 || heapIndex >= (int)heap.size()) return nullptr;
            const Record &rec = heap[heapIndex];
            return rec.deleted ? nullptr : &rec;
        }

        // Returns all records with ID in the range [lo, hi].
        // Also reports the number of key comparisons performed.
        vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {
            return vector<const Record *>();
        }

        // Returns all records whose last name begins with a given prefix.
        // Case-insensitive using lowercase comparison.
        vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
            return vector<const Record *>();
        }
};

#endif

//
// Created by alex on 22.06.20.
//

#ifndef BCG_HEAP_H
#define BCG_HEAP_H

#include <cassert>
#include <vector>
#include <iostream>

namespace bcg {

template<class heap_entry, class heap_interface>
class heap : private std::vector<heap_entry> {
public:
    typedef heap<heap_entry, heap_interface> This;

    //! Constructor
    heap() : heapVector() {}

    //! Construct with a given \c heapInterface.
    heap(const heap_interface &i) : heapVector(), interface(i) {}

    //! Destructor.
    ~heap() {};

    //! clear the heap
    void clear() { heapVector::clear(); }

    //! is heap empty?
    bool empty() { return heapVector::empty(); }

    //! returns the size of heap
    unsigned int size() { return (unsigned int) heapVector::size(); }

    //! reserve space for N entries
    void reserve(unsigned int n) { heapVector::reserve(n); }

    //! reset heap position to -1 (not in heap)
    void reset_heap_position(heap_entry h) {
        interface.set_heap_position(h, -1);
    }

    //! is an entry in the heap?
    bool is_stored(heap_entry h) {
        return interface.get_heap_position(h) != -1;
    }

    //! insert the entry h
    void insert(heap_entry h) {
        This::push_back(h);
        upheap(size() - 1);
    }

    //! get the first entry
    heap_entry front() {
        assert(!empty());
        return entry(0);
    }

    //! delete the first entry
    void pop_front() {
        assert(!empty());
        interface.set_heap_position(entry(0), -1);
        if (size() > 1) {
            entry(0, entry(size() - 1));
            heapVector::resize(size() - 1);
            downheap(0);
        } else
            heapVector::resize(size() - 1);
    }

    //! remove an entry
    void remove(heap_entry h) {
        int pos = interface.get_heap_position(h);
        interface.set_heap_position(h, -1);

        assert(pos != -1);
        assert((unsigned int) pos < size());

        // last item ?
        if ((unsigned int) pos == size() - 1)
            heapVector::resize(size() - 1);

        else {
            entry(pos, entry(size() - 1)); // move last elem to pos
            heapVector::resize(size() - 1);
            downheap(pos);
            upheap(pos);
        }
    }

    //! update an entry: change the key and update the position to
    //! reestablish the heap property.
    void update(heap_entry h) {
        int pos = interface.get_heap_position(h);
        assert(pos != -1);
        assert((unsigned int) pos < size());
        downheap(pos);
        upheap(pos);
    }

    //! check heap condition
    bool check() {
        bool ok(true);
        unsigned int i, j;
        for (i = 0; i < size(); ++i) {
            if (((j = left(i)) < size()) &&
                interface.greater(entry(i), entry(j))) {
                std::cerr << "heap condition violated\n";
                ok = false;
            }
            if (((j = right(i)) < size()) &&
                interface.greater(entry(i), entry(j))) {
                std::cerr << "heap condition violated\n";
                ok = false;
            }
        }
        return ok;
    }

private:
    // typedef
    typedef std::vector<heap_entry> heapVector;

    //! Upheap. Establish heap property.
    void upheap(unsigned int idx) {
        heap_entry h = entry(idx);
        unsigned int parentIdx;

        while ((idx > 0) && interface.less(h, entry(parentIdx = parent(idx)))) {
            entry(idx, entry(parentIdx));
            idx = parentIdx;
        }

        entry(idx, h);
    }

    //! Downheap. Establish heap property.
    void downheap(unsigned int idx) {
        heap_entry h = entry(idx);
        unsigned int childIdx;
        unsigned int s = size();

        while (idx < s) {
            childIdx = left(idx);
            if (childIdx >= s)
                break;

            if ((childIdx + 1 < s) &&
                (interface.less(entry(childIdx + 1), entry(childIdx))))
                ++childIdx;

            if (interface.less(h, entry(childIdx)))
                break;

            entry(idx, entry(childIdx));
            idx = childIdx;
        }

        entry(idx, h);
    }

    //! Get the entry at index idx
    inline heap_entry entry(unsigned int idx) {
        assert(idx < size());
        return (This::operator[](idx));
    }

    //! Set entry H to index idx and update H's heap position.
    inline void entry(unsigned int idx, heap_entry h) {
        assert(idx < size());
        This::operator[](idx) = h;
        interface.set_heap_position(h, idx);
    }

    //! Get parent's index
    inline unsigned int parent(unsigned int i) { return (i - 1) >> 1; }

    //! Get left child's index
    inline unsigned int left(unsigned int i) { return (i << 1) + 1; }

    //! Get right child's index
    inline unsigned int right(unsigned int i) { return (i << 1) + 2; }

    //! Instance of heapInterface
    heap_interface interface;
};

}

#endif //BCG_HEAP_H

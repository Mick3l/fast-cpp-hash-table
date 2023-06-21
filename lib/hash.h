#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <memory>

namespace mtl {

    template<typename T, typename U, typename Compare, typename Alloc = std::allocator<U>, typename Hash = std::hash<T>>
    class HashTable {
    public:

        struct HashTable_impl : public Alloc, public Hash {
            U* array;
            size_t size;
            size_t capacity;
            int8_t* deleted;// -1 <=> not ever used, 0 <=> currently in use, 1 <=> was used and now is free
            Compare compare;
        };

        class iterator {
        public:
            explicit iterator(size_t idx, HashTable_impl& hashTable);

            ~iterator() = default;

            iterator& operator=(const iterator& other) = default;

            iterator(const iterator& other) = default;

            inline iterator& operator++();

            inline iterator& operator--();

            inline iterator operator++(int);

            inline iterator operator--(int);

            inline bool operator==(const iterator& other) const { return idx == other.idx; }

            inline bool operator<(const iterator& other) const { return idx < other.idx; }

            inline bool operator>(const iterator& other) const { return idx > other.idx; }

            inline U& operator*() { return hashTable->array[idx]; };

            inline U* operator->() { return hashTable->array + idx; }

        private:
            size_t idx;

            HashTable_impl* hashTable;
        };


        explicit HashTable(size_t size, Alloc alloc = Alloc());

        HashTable(const HashTable& other) = delete;

        HashTable& operator=(const HashTable& other) = delete;

        ~HashTable();

        template<typename KeyByValue>
        void Insert(const T& key, const U& value);

        template<typename KeyByValue>
        void Insert(const T& key, U&& value);

        void Delete(const T& key);

        template<typename KeyByValue>
        inline void Rehash(size_t new_size, KeyByValue keyByValue);

        inline size_t Capacity() { return impl.capacity; };

        bool Contains(const T& key);

        U& Find(const T& key);

        inline iterator begin();

        inline iterator end();

        inline U& GetByIdx(size_t idx) { return impl.array[idx]; }

        inline size_t size() { return impl.size; }

        inline size_t capacity() { return impl.capacity; }

    private:
        HashTable_impl impl;
    };

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    HashTable<T, U, Compare, Alloc, Hash>::iterator::iterator(size_t idx, HashTable_impl& hashTable):idx(idx),
                                                                                                     hashTable(
                                                                                                             &hashTable) {

    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    typename HashTable<T, U, Compare, Alloc, Hash>::iterator&
    HashTable<T, U, Compare, Alloc, Hash>::iterator::operator++() {
        ++idx;
        while (idx < hashTable->capacity && hashTable->deleted[idx] != 0) {
            ++idx;
        }
        return *this;
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    typename HashTable<T, U, Compare, Alloc, Hash>::iterator&
    HashTable<T, U, Compare, Alloc, Hash>::iterator::operator--() {
        --idx;
        while (idx <= hashTable->capacity && hashTable->deleted[idx] != 0) {
            --idx;
        }
        return *this;
    }

#define _array impl.array
#define _size impl.size
#define _capacity impl.capacity
#define _deleted impl.deleted

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    HashTable<T, U, Compare, Alloc, Hash>::HashTable(size_t size, Alloc alloc):impl(alloc) {
        static_assert(std::is_same_v<typename Alloc::value_type, U>);
        _size = 0;
        _capacity = size * 2;
        _array = impl.allocate(_capacity);
        for (int i = 0; i < _capacity; ++i) {
            std::allocator_traits<Alloc>::construct(impl, _array + i);
        }
        typename std::allocator_traits<Alloc>::template rebind_alloc<int8_t> _rebind(impl);
        _deleted = _rebind.allocate(_capacity);
        std::fill(_deleted, _deleted + _capacity, -1);
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    HashTable<T, U, Compare, Alloc, Hash>::~HashTable() {
        for (int i = 0; i < _capacity; ++i) {
            std::allocator_traits<Alloc>::destroy(impl, _array + i);
        }
        impl.deallocate(_array, _capacity);
        typename std::allocator_traits<Alloc>::template rebind_alloc<int8_t> _rebind(impl);
        _rebind.deallocate(_deleted, _capacity);
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    template<typename KeyByValue>
    void HashTable<T, U, Compare, Alloc, Hash>::Insert(const T& key, const U& value) {
        auto hash = impl(key) % _capacity;
        int32_t idx = -1;
        while (_deleted[hash] != -1 && !impl.compare(_array[hash], key)) {
            ++hash;
            if (hash >= _capacity) {
                hash -= _capacity;
            }
            if (_deleted[hash] && idx == -1) {
                idx = hash;
            }
        }
        if (_deleted[hash] != -1) {
            _array[hash] = value;
            return;
        }
        if (idx < 0) {
            idx = hash;
        }
        _array[idx] = value;
        _deleted[idx] = 0;
        ++_size;
        if (_size >= _capacity >> 1u) {
            Rehash(_capacity << 1u, KeyByValue());
        }
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    void HashTable<T, U, Compare, Alloc, Hash>::Delete(const T& key) {
        auto hash = impl(key) % _capacity;
        while (_deleted[hash] != -1 && !impl.compare(_array[hash], key)) {
            ++hash;
            if (hash >= _capacity) {
                hash -= _capacity;
            }
        }
        if (!_deleted[hash]) {
            _deleted[hash] = 1;
            --_size;
        }
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    template<typename KeyByValue>
    void HashTable<T, U, Compare, Alloc, Hash>::Rehash(size_t new_size, KeyByValue keyByValue) {
        typename std::allocator_traits<Alloc>::template rebind_alloc<int8_t> _rebind;
        HashTable_impl new_impl(impl);
        new_impl.array = new_impl.allocate(new_size);
        new_impl.size = _size;
        new_impl.capacity = new_size;
        new_impl.deleted = _rebind.allocate(new_size);
        std::fill(new_impl.deleted, new_impl.deleted + new_size, -1);
        for (int i = 0; i < new_impl.capacity; ++i) {
            std::allocator_traits<Alloc>::construct(new_impl, new_impl.array + i);
        }

        for (auto i = begin(); i < end(); ++i) {
            auto& j = *i;
            //Insert
            auto hash = impl(keyByValue(j)) % new_impl.capacity;
            while (new_impl.deleted[hash] != -1) {
                ++hash;
                if (hash >= new_impl.capacity) {
                    hash -= new_impl.capacity;
                }
            }
            new_impl.array[hash] = std::move(j);
            new_impl.deleted[hash] = 0;
        }
        //destructor
        for (int i = 0; i < _capacity; ++i) {
            std::allocator_traits<Alloc>::destroy(impl, _array + i);
        }
        impl.deallocate(_array, _capacity);
        _rebind.deallocate(_deleted, _capacity);
        impl = new_impl;
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    bool HashTable<T, U, Compare, Alloc, Hash>::Contains(const T& key) {
        auto hash = impl(key) % _capacity;
        while (_deleted[hash] != -1 && !impl.compare(_array[hash], key)) {
            ++hash;
            if (hash >= _capacity) {
                hash -= _capacity;
            }
        }
        return _deleted[hash] == 0;
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    U& HashTable<T, U, Compare, Alloc, Hash>::Find(const T& key) {
        auto hash = impl(key) % _capacity;
        while (!_deleted[hash] && !impl.compare(_array[hash], key)) {
            ++hash;
            if (hash >= _capacity) {
                hash -= _capacity;
            }
        }

        return _array[hash];
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    typename HashTable<T, U, Compare, Alloc, Hash>::iterator HashTable<T, U, Compare, Alloc, Hash>::begin() {
        size_t index = 0;
        while(_deleted[index]){
            ++index;
        }
        return HashTable::iterator(index, impl);
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    typename HashTable<T, U, Compare, Alloc, Hash>::iterator HashTable<T, U, Compare, Alloc, Hash>::end() {
        return HashTable::iterator(impl.capacity, impl);
    }

    template<typename T, typename U, typename Compare, typename Alloc, typename Hash>
    template<typename KeyByValue>
    void HashTable<T, U, Compare, Alloc, Hash>::Insert(const T& key, U&& value) {
        auto hash = impl(key) % _capacity;
        int32_t idx = -1;
        while (_deleted[hash] != -1 && !impl.compare(_array[hash], key)) {
            ++hash;
            if (hash >= _capacity) {
                hash -= _capacity;
            }
            if (_deleted[hash] && idx == -1) {
                idx = hash;
            }
        }
        if (_deleted[hash] != -1) {
            _array[hash] = std::move(value);
            return;
        }
        if (idx < 0) {
            idx = hash;
        }
        _array[idx] = std::move(value);
        _deleted[idx] = 0;
        ++_size;
        if (_size >= _capacity >> 1u) {
            Rehash(_capacity << 1u, KeyByValue());
        }
    }

    template<typename T, typename U, typename Alloc = std::allocator<std::pair<T, U>>, typename Hash = std::hash<T>>
    class hash_map {
    private:
        struct Compare {
            inline bool operator()(const std::pair<T, U>& pair, T key) const {
                return pair.first == key;
            }
        };

        struct KeyByValue {
            inline const T& operator()(const std::pair<T, U>& pair) const {
                return pair.first;
            }
        };

        HashTable<T, std::pair<T, U>, Compare, Alloc, Hash> hashTable;
    public:
        hash_map();

        explicit hash_map(size_t size);

        ~hash_map() = default;

        inline U& operator[](const T& key) { return hashTable.Find(key).second; };

        inline bool Contains(const T& key) { return hashTable.Contains(key); };

        inline void Insert(const T& key, const U& value) {
            hashTable.template Insert<KeyByValue>(key, std::make_pair(key, value));
        };

        inline void Insert(const T& key, U&& value) {
            hashTable.template Insert<KeyByValue>(key, std::make_pair(key, std::move(value)));
        };

        inline void Delete(const T& key) { hashTable.Delete(key); };

        inline typename HashTable<T, std::pair<T, U>, Compare, Alloc, Hash>::iterator
        begin() { return hashTable.begin(); }

        inline typename HashTable<T, std::pair<T, U>, Compare, Alloc, Hash>::iterator
        end() { return hashTable.end(); }

        inline size_t size() { return hashTable.size(); }

        inline size_t capacity() { return hashTable.capacity(); }
    };

    template<typename T, typename U, typename Alloc, typename Hash>
    hash_map<T, U, Alloc, Hash>::hash_map():hashTable(1) {
    }

    template<typename T, typename U, typename Alloc, typename Hash>
    hash_map<T, U, Alloc, Hash>::hash_map(size_t size):hashTable(size) {
    }

    template<typename T, typename Alloc = std::allocator<T>, typename Hash = std::hash<T>>
    class hash_set {
    private:
        struct Compare {
            inline bool operator()(T element, T key) {
                return element == key;
            }
        };

        struct KeyByValue {
            inline T operator()(T element) {
                return element;
            }
        };

        HashTable<T, T, Compare, Alloc, Hash> hashTable;

    public:
        hash_set();

        explicit hash_set(size_t size);

        ~hash_set() = default;

        inline bool Contains(const T& value) { return hashTable.Contains(value); };

        inline void Insert(const T& value) { hashTable.template Insert<KeyByValue>(value, value); };

        inline void Delete(const T& value) { hashTable.Delete(value); };

        inline typename HashTable<T, T, Compare, Alloc, Hash>::iterator
        begin() { return hashTable.begin(); }

        inline typename HashTable<T, T, Compare, Alloc, Hash>::iterator
        end() { return hashTable.end(); }

        inline size_t size() { return hashTable.size(); }

        inline size_t capacity() { return hashTable.capacity(); }

    };

    template<typename T, typename Alloc, typename Hash>
    hash_set<T, Alloc, Hash>::hash_set():hashTable(1) {
    }

    template<typename T, typename Alloc, typename Hash>
    hash_set<T, Alloc, Hash>::hash_set(size_t size):hashTable(size) {
    }

    template<typename T, typename Alloc = std::allocator<std::pair<T, int32_t>>, typename Hash = std::hash<T>>
    class hash_multiset {
    private:
        struct Compare {
            inline bool operator()(std::pair<T, int32_t> pair, T key) {
                return pair.first == key;
            }
        };

        struct KeyByValue {
            inline T operator()(std::pair<T, int32_t> pair) {
                return pair.first;
            }
        };

        HashTable<T, std::pair<T, int32_t>, Compare, Alloc, Hash> hashTable;
    public:
        hash_multiset();

        explicit hash_multiset(size_t size);

        ~hash_multiset() = default;

        inline bool Contains(const T& value) { return hashTable.Contains(value); }

        inline void Insert(const T& value);

        inline void Delete(const T& value) { hashTable.Delete(value); }

        inline int32_t Count(const T& value);

        inline typename HashTable<T, std::pair<T, int32_t>, Compare, Alloc, Hash>::iterator
        begin() { return hashTable.begin(); }

        inline typename HashTable<T, std::pair<T, int32_t>, Compare, Alloc, Hash>::iterator
        end() { return hashTable.end(); }

        inline size_t size() { return hashTable.size(); }

        inline size_t capacity() { return hashTable.capacity(); }

    };

    template<typename T, typename Alloc, typename Hash>
    hash_multiset<T, Alloc, Hash>::hash_multiset():hashTable(1) {
    }

    template<typename T, typename Alloc, typename Hash>
    hash_multiset<T, Alloc, Hash>::hash_multiset(size_t size):hashTable(size) {
    }

    template<typename T, typename Alloc, typename Hash>
    void hash_multiset<T, Alloc, Hash>::Insert(const T& value) {
        if (hashTable.Contains(value)) {
            ++hashTable.Find(value).second;
            return;
        }
        hashTable.template Insert<KeyByValue>(value, std::make_pair(value, 1));
    }

    template<typename T, typename Alloc, typename Hash>
    int32_t hash_multiset<T, Alloc, Hash>::Count(const T& value) {
        if (hashTable.Contains(value)) {
            return hashTable.Find(value).second;
        }
        return 0;
    }

}

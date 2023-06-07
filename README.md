# Fast c++ Hash table implementation

# Cmake
If you already have a CMake project you can integrate Hash Table with `fetch_content`.
Add the following to your `CMakeLists.txt`.


```cmake
include(FetchContent)
FetchContent_Declare(hash GIT_REPOSITORY https://github.com/Mick3l/fast-cpp-hash-table.git
        GIT_TAG 332848acccd084e074fea278c25eb3f769d19581) #replace with the latest commit in main branch
FetchContent_MakeAvailable(hash)
```

This will produce variable `HASH` with `hash.h`'s file path. To make it includable in your target you should do:

```cmake
target_include_directories(your_target_name PRIVATE "${HASH}")
```

That should do it!

# Usage
This library implements three classes:
* mtl::hash_map
* mtl::hash_set
* mtl::hash_multiset

they take type(s), allocator and hash function as template arguments
# Performance
You can test the performance on your current platform using tests in `tests\tests_hash.cpp` file
In average you can get 2-3x time speedup in comparison with the standard `std::unordered_xxx`

P.S.Keep in mind the fact that hash tables use additional memory to reduce the number of collisions [simply if you write `mtl::hash_set<T> hash_set(n)` then `2 * n * (sizeof(T) + sizeof(size_t))` memory will be allocated]
# Examples
hash_map example
```cpp
#include <iostream>

#include "hash.h"

int main(){
    mtl::hash_map<std::string,int> map;
    //to insert value you should use Insert method, operator[] is only for getting element reference 
    map.Insert("hello", 2023);
    map.Insert("world", 0);
    std::cout << map["hello"] << std::endl;//2023
    std::cout << map.Contains("world") << std::endl;//true
    map.Delete("world");
    std::cout << map.Contains("world") << std::endl;//false
    map.Insert("cpp", 100);
    map["hello"] = 28;
    for(auto& i:map){
        std::cout << i.first << ' ' << i.second << std::endl;
        /* be aware that order of elements may differ from this*/
        //hello 28
        //cpp 100
    }
    return 0;
}
```
hash_set example
```cpp
#include <iostream>

#include "hash.h"

int main(){
    mtl::hash_set<std::string> set;
    set.Insert("hello");
    set.Insert("world");
    std::cout << set.Contains("hello") << std::endl;//true
    std::cout << set.Contains("world") << std::endl;//true
    set.Insert("world");
    set.Insert("world");
    set.Delete("world");
    std::cout << set.Contains("world") << std::endl;//false
    set.Insert("cpp");
    for(auto& i:set){
        std::cout << i << std::endl;
        /* be aware that order of elements may differ from this*/
        //hello
        //cpp
    }
    return 0;
}
```
hash_multiset example
```cpp
#include <iostream>

#include "hash.h"

int main(){
    mtl::hash_multiset<std::string> multiset;
    multiset.Insert("hello");
    multiset.Insert("world");
    multiset.Insert("world");
    std::cout << multiset.Contains("hello") << std::endl;//true
    std::cout << multiset.Count("world") << std::endl;//2
    multiset.Insert("world");
    multiset.Insert("world");
    multiset.Delete("world");
    std::cout << multiset.Count("world") << std::endl;//3
    multiset.Insert("cpp");
    for(auto& i:multiset){
        std::cout << i.first << ' ' << i.second << std::endl;
        /* be aware that order of elements may differ from this*/
        //hello
        //cpp
    }
    return 0;
}
```
# Warning
* all this containers have no exception warranty when allocate memory (todo)
* mtl::multiset has some bugs (todo)
* all containers have no copy constructor and operator= (todo)

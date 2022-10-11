#include <utility>
#include <algorithm> 
#include "StrVec.h"

allocator<string> StrVec::alloc;

StrVec::StrVec(initializer_list<string> il) : StrVec() 
{
    for (const auto &s : il)
        push_back(s);
}
pair<string *, string *> StrVec::alloc_n_copy(const string *b, const string *e) 
{
    auto b2 = alloc.allocate(e - b);
    return {b2, uninitialized_copy(b, e, b2)};
}

void StrVec::reallocate() {
    auto new_cap_sz = size() ? 2 * size() : 1;
    auto new_first_elem = alloc.allocate(new_cap_sz);
    auto new_first_free = new_first_elem;
    auto old_first_elem = first_elem;
    for (size_type i = 0; i != size(); ++i)
        alloc.construct(new_first_free++, std::move(*old_first_elem++));
    free();
    first_elem = new_first_elem;
    first_free = new_first_free;
    cap = first_elem + new_cap_sz;
}

void StrVec::free() 
{
    if (first_elem) 
    {
        for_each(first_elem, first_free,
            [](string &p) { alloc.destroy(&p); });
        alloc.deallocate(first_elem, cap - first_elem);
    }
}

StrVec::StrVec(const StrVec &rhs) 
{
    auto data = alloc_n_copy(rhs.begin(), rhs.end());
    first_elem = data.first;
    first_free = cap = data.second;
}

StrVec::~StrVec() 
{
    free();
}

StrVec &StrVec::operator=(const StrVec &rhs) 
{
    auto data = alloc_n_copy(rhs.begin(), rhs.end());
    free();
    first_elem = data.first;
    first_free = cap = data.second;
    return *this;
}


void StrVec::push_back(const string &s) 
{
    chk_n_alloc();
    alloc.construct(first_free++, s);
}

void StrVec::pop_back() 
{
    if (size() > 0)
        alloc.destroy(--first_free);
}

void StrVec::reserve(size_type n) 
{
    if (n > capacity()) 
    {
        auto new_first_elem = alloc.allocate(n);
        auto new_first_free = new_first_elem;
        auto old_first_elem = first_elem;
        for (size_type i = 0; i != size(); ++i)
        alloc.construct(new_first_free++, std::move(*old_first_elem++));
        free();
        first_elem = new_first_elem;
        first_free = new_first_free;
        cap = first_elem + n;
    }
}

void StrVec::resize(size_type n, const string &s) 
{
    if (n > size()) 
    {
        for (int i = n - size(); i != 0; --i)
        push_back(s);
    } 
    else if (n < size()) 
    {
        for (int i = size() - n; i != 0; --i)
        pop_back();
    }
}
#include <iostream>
#include <cassert>

template <typename Derived>
struct less_than_comparable 
{
protected:
    ~less_than_comparable() = default;

public:
    friend bool operator>(const Derived& lhs, const Derived& rhs) 
    {
        return rhs < lhs;
    }

    friend bool operator<=(const Derived& lhs, const Derived& rhs) 
    {
        return !(rhs < lhs);
    }

    friend bool operator>=(const Derived& lhs, const Derived& rhs) 
    {
        return !(lhs < rhs);
    }

    friend bool operator==(const Derived& lhs, const Derived& rhs) 
    {
        return !(lhs < rhs) && !(rhs < lhs);
    }

    friend bool operator!=(const Derived& lhs, const Derived& rhs) 
    {
        return !(lhs == rhs); 
    }
};


template <typename Derived>
struct counter 
{
protected:
    counter() { ++count_; }
    
    counter(const counter&) { ++count_; }
    
    counter(counter&&) noexcept { ++count_; }

    ~counter() { --count_; }

    counter& operator=(const counter&) = default;
    counter& operator=(counter&&) noexcept = default;

public:
    static size_t count() { return count_; }

private:
    static size_t count_;
};


template <typename Derived>
size_t counter<Derived>::count_ = 0;

class Number : public less_than_comparable<Number>, public counter<Number> 
{
public:
    Number(int value) : m_value{value} {}

    int value() const { return m_value; }

    bool operator<(const Number& other) const 
    {
        return m_value < other.m_value;
    }

private:
    int m_value;
};


int main() 
{
    Number one{1};
    Number two{2};
    Number three{3};
    Number four{4};

    assert(one >= one);
    assert(three <= four);
    assert(two == two);
    assert(three > two);
    assert(one < two);
    assert(one != two); 

    return 0;
}
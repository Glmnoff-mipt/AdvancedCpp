#include <iostream>
#include <vector>
#include <unordered_set>
#include <memory>
#include <algorithm>

constexpr size_t THRESHOLD = 5; 

template <typename T>
class ISetImpl 
{
public:
    virtual ~ISetImpl() = default;

    virtual void add(const T& value) = 0;
    virtual void remove(const T& value) = 0;
    virtual bool contains(const T& value) const = 0;
    virtual size_t size() const = 0;
    virtual std::vector<T> toVector() const = 0; 
};


template <typename T>
class VectorSetImpl : public ISetImpl<T> 
{
private:
    std::vector<T> data_;

public:
    void add(const T& value) override 
    {
        if (!contains(value)) { data_.push_back(value); }
    }

    void remove(const T& value) override 
    {
        auto it = std::find(data_.begin(), data_.end(), value);
        if (it != data_.end()) { data_.erase(it); }
    }

    bool contains(const T& value) const override 
    {
        return std::find(data_.begin(), data_.end(), value) != data_.end();
    }

    size_t size() const override { return data_.size(); }

    std::vector<T> toVector() const override { return data_; }
};


template <typename T>
class HashSetImpl : public ISetImpl<T> 
{
private:
    std::unordered_set<T> data_;

public:
    HashSetImpl(const std::vector<T>& initial_data) 
    {
        for (const auto& item : initial_data) { data_.insert(item); }
    }

    void add(const T& value) override { data_.insert(value); }

    void remove(const T& value) override { data_.erase(value); }

    bool contains(const T& value) const override { return data_.count(value) > 0; }

    size_t size() const override { return data_.size(); }

    std::vector<T> toVector() const override 
    {
        return std::vector<T>(data_.begin(), data_.end());
    }
};


template <typename T>
class MySet 
{
private:
    std::unique_ptr<ISetImpl<T>> impl_;

    void checkAndSwitchImpl() 
    {
        size_t current_size = impl_->size();

        if (current_size >= THRESHOLD && dynamic_cast<VectorSetImpl<T>*>(impl_.get())) 
        {
            std::cout << "Switched to HashSet\n";
            std::vector<T> old_data = impl_->toVector();
            impl_ = std::make_unique<HashSetImpl<T>>(old_data);
        }
    }

public:
    MySet() : impl_{std::make_unique<VectorSetImpl<T>>()} {}

    void add(const T& value) 
    {
        impl_->add(value);
        checkAndSwitchImpl(); 
    }

    void remove(const T& value) { impl_->remove(value); }

    bool contains(const T& value) const { return impl_->contains(value); }

    size_t size() const { return impl_->size(); }

    MySet<T> unionWith(const MySet<T>& other) const 
    {
        MySet<T> result;
        for (const auto& item : this->impl_->toVector()) { result.add(item); }
        for (const auto& item : other.impl_->toVector()) { result.add(item); }
        return result;
    }

    MySet<T> intersectWith(const MySet<T>& other) const 
    {
        MySet<T> result;
        for (const auto& item : this->impl_->toVector()) 
        {
            if (other.contains(item)) { result.add(item); }
        }
        return result;
    }
};

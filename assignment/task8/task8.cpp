#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

class Expression 
{
public:
    virtual ~Expression() = default;

    virtual void print() const = 0;
    virtual int calculate(const std::map<std::string, int>& context) const = 0;

    virtual bool isFlyweight() const { return false; }
};


class Constant : public Expression 
{
private:
    int value_;

public:
    explicit Constant(int val) : value_{val} {}

    void print() const override { std::cout << value_; }

    int calculate(const std::map<std::string, int>&) const override 
    { 
        return value_; 
    }

    bool isFlyweight() const override { return true; }
};


class Variable : public Expression 
{
private:
    std::string name_;

public:
    explicit Variable(std::string name) : name_{std::move(name)} {}

    void print() const override {  std::cout << name_; }

    int calculate(const std::map<std::string, int>& context) const override 
    {
        auto it = context.find(name_);

        if (it == context.end()) 
        { 
            throw std::runtime_error("Variable '" + name_ + "' is not defined");
        }
        return it->second;
    }

    bool isFlyweight() const override { return true; }
};


class Addition : public Expression 
{
private:
    Expression* left_;
    Expression* right_;

public:
    Addition(Expression* left, Expression* right) : left_{left}, right_{right} 
    {
        if (!left_ || !right_) throw std::invalid_argument("Expression branches cannot be null");
    }

    ~Addition() override 
    {
        if (!left_->isFlyweight())  { delete left_; }
        if (!right_->isFlyweight()) { delete right_; }
    }

    void print() const override 
    {
        std::cout << "(";
        left_->print();
        std::cout << " + ";
        right_->print();
        std::cout << ")";
    }

    int calculate(const std::map<std::string, int>& context) const override 
    {
        return left_->calculate(context) + right_->calculate(context);
    }
};


class Subtraction : public Expression 
{
private:
    Expression* left_;
    Expression* right_;

public:
    Subtraction(Expression* left, Expression* right) : left_{left}, right_{right} 
    {
        if (!left_ || !right_) throw std::invalid_argument("Expression branches cannot be null");
    }

    ~Subtraction() override 
    {
        if (!left_->isFlyweight())  { delete left_; }
        if (!right_->isFlyweight()) { delete right_; }
    }

    void print() const override 
    {
        std::cout << "(";
        left_->print();
        std::cout << " - ";
        right_->print();
        std::cout << ")";
    }

    int calculate(const std::map<std::string, int>& context) const override 
    {
        return left_->calculate(context) - right_->calculate(context);
    }
};


class ExpressionFactory 
{
private:
    std::map<int, Constant*> constants_;
    std::map<std::string, Variable*> variables_;

public:
    ExpressionFactory() 
    {
        for (int i = -5; i <= 256; ++i) { constants_[i] = new Constant(i); }
    }

    ~ExpressionFactory() 
    {
        for (auto& pair : constants_) { delete pair.second; }
        for (auto& pair : variables_) { delete pair.second; }
    }

    Constant* createConstant(int value) 
    {
        auto& const_ptr = constants_[value];
        if (!const_ptr) { const_ptr = new Constant(value); }
        return const_ptr;
    }

    Variable* createVariable(const std::string& name) 
    {
        auto& var_ptr = variables_[name];
        if (!var_ptr) { var_ptr = new Variable(name); }
        return var_ptr;
    }

    void deleteConstant(int value) 
    {
        if (value >= -5 && value <= 256) return;

        auto it = constants_.find(value);
        if (it != constants_.end()) 
        {
            delete it->second;
            constants_.erase(it);
        }
    }

    void deleteVariable(const std::string& name) 
    {
        auto it = variables_.find(name);
        if (it != variables_.end()) 
        {
            delete it->second;
            variables_.erase(it);
        }
    }
};


int main() 
{
    ExpressionFactory factory;

    Constant *c = factory.createConstant(2);
    Variable *v = factory.createVariable("x");
    Addition *expression = new Addition(c, v);

    std::map<std::string, int> context; 
    context["x"] = 3;

    std::cout << "Expression: ";
    expression->print();
    std::cout << "\nResult: ";
    std::cout << expression->calculate(context) << std::endl;

    delete expression; 

    return 0;
}
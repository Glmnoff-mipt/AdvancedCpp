#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept> 

class MandatoryCP;
class OptionalCP;

class ICPBuilder 
{
public:
    virtual ~ICPBuilder() = default; 
    virtual void buildMandatory(const MandatoryCP& cp) = 0;
    virtual void buildOptional(const OptionalCP& cp) = 0;
};

class CheckPoint 
{
protected:
    std::string name_;
    double latitude_;
    double longitude_;

public:
    CheckPoint(std::string name, double lat, double lon) 
        : name_{std::move(name)}, latitude_{lat}, longitude_{lon} 
    {
        if (lat < -90.0 || lat > 90.0) 
        {
            throw std::invalid_argument("Latitude must be between -90.0 and +90.0");
        }
        if (lon < -180.0 || lon > 180.0) 
        {
            throw std::invalid_argument("Longitude must be between -180.0 and +180.0");
        }
    }

    virtual ~CheckPoint() = default;

    std::string getName() const { return name_; }
    double getLatitude() const { return latitude_; }
    double getLongitude() const { return longitude_; }

    virtual void accept(ICPBuilder& builder) const = 0;
};


class MandatoryCP : public CheckPoint 
{
public:
    MandatoryCP(std::string name, double lat, double lon) 
        : CheckPoint(std::move(name), lat, lon) {}

    void accept(ICPBuilder& builder) const override { builder.buildMandatory(*this); }
};

class OptionalCP : public CheckPoint 
{
private:
    double penalty_hours_; 

public:
    OptionalCP(std::string name, double lat, double lon, double penalty) 
        : CheckPoint(std::move(name), lat, lon), penalty_hours_{penalty} 
    {
        if (penalty < 0.0) 
        {
            throw std::invalid_argument("Error: Penalty hours cannot be negative");
        }
    }

    double getPenalty() const { return penalty_hours_; }

    void accept(ICPBuilder& builder) const override { builder.buildOptional(*this); }
};


class Route 
{
private:
    std::vector<std::unique_ptr<CheckPoint>> cps_; 

public:
    void addCP(std::unique_ptr<CheckPoint> cp) { cps_.push_back(std::move(cp)); }

    void construct(ICPBuilder& builder) const 
    {
        for (const auto& cp : cps_) { cp->accept(builder); }
    }
};


class TextPrintBuilder : public ICPBuilder 
{
private:
    int counter_ = 1;

public:
    void buildMandatory(const MandatoryCP& cp) override 
    {
        std::cout << counter_++ << ". " 
                  << "Name: " << cp.getName() 
                  << " | Coords: (" << cp.getLatitude() << ", " << cp.getLongitude() << ")"
                  << " | Penalty: незачёт СУ\n";
    }

    void buildOptional(const OptionalCP& cp) override 
    {
        std::cout << counter_++ << ". " 
                  << "Name: " << cp.getName() 
                  << " | Coords: (" << cp.getLatitude() << ", " << cp.getLongitude() << ")"
                  << " | Penalty: " << cp.getPenalty() << " hours\n";
    }
};
 

class PenaltyCalculatorBuilder : public ICPBuilder 
{
private:
    double total_penalty_ = 0.0;

public:
    void buildMandatory(const MandatoryCP&) override { }

    void buildOptional(const OptionalCP& cp) override { total_penalty_ += cp.getPenalty(); }

    double getTotalPenalty() const { return total_penalty_; }
};

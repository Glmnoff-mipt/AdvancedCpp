#ifndef POINTCLOUD_PREPROCESSOR__FILTER_HPP_
#define POINTCLOUD_PREPROCESSOR__FILTER_HPP_

#include "Logger.h"
#include "PointCloud.h"

#include <string>
#include <cmath>
#include <unordered_map>
#include <memory>
#include <utility>

namespace pointcloud_preprocessor
{

class FilterParametr {
public:
    FilterParametr() = default;
    explicit FilterParametr(std::unordered_map<std::string, double> params) 
        : params_(std::move(params)) {}

    void ChangeParam(const std::string& param_name, double param_val) {
        params_[param_name] = param_val;
    }

    double GetParam(const std::string& param_name) const {   // обяз параметры (при отсутствии out of range)
        return params_.at(param_name);
    }

    double GetParam(const std::string& param_name, double default_val) const noexcept {   // необяз параметры (гарантия отстутств искл)
        if (auto it = params_.find(param_name); it != params_.end()) {
            return it->second;
        }
        return default_val;
    }
private:
    std::unordered_map<std::string, double> params_;
};

class PointMetricsStrategy {
public:
    virtual ~PointMetricsStrategy() = default;
    virtual double GetDistance(const PointCloud* pc, size_t index) const = 0;
    virtual double GetAzimuth(const PointCloud* pc, size_t index) const = 0;
};

class DefaultMetricsStrategy : public PointMetricsStrategy {
public:
    double GetDistance(const PointCloud* pc, size_t index) const override {
        return pc->GetPoints()[index * pc->GetPointSize() + 5];
    }
    double GetAzimuth(const PointCloud* pc, size_t index) const override {
        return pc->GetPoints()[index * pc->GetPointSize() + 6];
    }
};  

class XYZIRMetricsStrategy : public PointMetricsStrategy {
public:
    double GetDistance(const PointCloud* pc, size_t index) const override {
        const size_t offset = index * pc->GetPointSize();
        return std::hypot(pc->GetPoints()[offset], pc->GetPoints()[offset + 1], pc->GetPoints()[offset + 2]);
    }
    double GetAzimuth(const PointCloud* pc, size_t index) const override {
        const size_t offset = index * pc->GetPointSize();
        return std::atan2(pc->GetPoints()[offset + 1], pc->GetPoints()[offset]);
    }
};

/** \brief @b Filter represents the base filter class. */
class Filter
{
public:
  explicit Filter(std::string filter_name = "pointcloud_preprocessor_filter") 
      : filter_name_(std::move(filter_name)), logger_(filter_name_) 
  {
      format_strategies_["XYZIR"] = std::make_shared<XYZIRMetricsStrategy>();
      default_strategy_ = std::make_shared<DefaultMetricsStrategy>();
  }
  
  virtual ~Filter() = default;
  
  const std::string& GetFilterName() const { return filter_name_; }
  
  double GetDistance(const PointCloud* pc, size_t index) const {
      if (!pc || pc->GetSize() == 0) return 0.0;
      if (auto it = format_strategies_.find(pc->GetPointCloudType()); it != format_strategies_.end()) {
          return it->second->GetDistance(pc, index);
      }
      return default_strategy_->GetDistance(pc, index);
  }
  
  double GetAzimuth(const PointCloud* pc, size_t index) const {
      if (!pc || pc->GetSize() == 0) return 0.0;
      if (auto it = format_strategies_.find(pc->GetPointCloudType()); it != format_strategies_.end()) {
          return it->second->GetAzimuth(pc, index);
      }
      return default_strategy_->GetAzimuth(pc, index);
  }

  virtual PointCloud* Apply(PointCloud* pc) = 0;
  
  virtual void SetParams(const FilterParametr& param) {
      params_ = param;
  }

  const FilterParametr& GetFilterParam() const {
      return params_;
  }
protected:
    std::string filter_name_;
    Logger logger_;
    FilterParametr params_;

private:
    std::unordered_map<std::string, std::shared_ptr<PointMetricsStrategy>> format_strategies_;
    std::shared_ptr<PointMetricsStrategy> default_strategy_;
};

}

#endif  // POINTCLOUD_PREPROCESSOR__FILTER_HPP_
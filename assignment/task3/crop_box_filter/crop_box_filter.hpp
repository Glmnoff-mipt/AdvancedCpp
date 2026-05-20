#ifndef POINTCLOUD_PREPROCESSOR__CROP_BOX_FILTER_HPP_
#define POINTCLOUD_PREPROCESSOR__CROP_BOX_FILTER_HPP_

#include "../filter.hpp"

namespace pointcloud_preprocessor
{

class BoxRegion {
public:
    BoxRegion() = default;

    void Update(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z, bool negative) {
        min_x_ = min_x; max_x_ = max_x;
        min_y_ = min_y; max_y_ = max_y;
        min_z_ = min_z; max_z_ = max_z;
        negative_ = negative;
    }

    bool IsInside(double x, double y, double z) const {
        bool inside = (x > min_x_ && x < max_x_) &&
                      (y > min_y_ && y < max_y_) &&
                      (z > min_z_ && z < max_z_);
        return negative_ ? !inside : inside;
    }

    bool operator!=(const BoxRegion& other) const {
        return min_x_ != other.min_x_ || max_x_ != other.max_x_ ||
               min_y_ != other.min_y_ || max_y_ != other.max_y_ ||
               min_z_ != other.min_z_ || max_z_ != other.max_z_ ||
               negative_ != other.negative_;
    }

    double GetMinX() const { return min_x_; }
    double GetMaxX() const { return max_x_; }
    double GetMinY() const { return min_y_; }
    double GetMaxY() const { return max_y_; }
    double GetMinZ() const { return min_z_; }
    double GetMaxZ() const { return max_z_; }
    bool GetNegative() const { return negative_; }

private:
    double min_x_{0.0}, max_x_{0.0};
    double min_y_{0.0}, max_y_{0.0};
    double min_z_{0.0}, max_z_{0.0};
    bool negative_{false};
};

class CropBoxFilter : public Filter
{
public:
  CropBoxFilter();
  ~CropBoxFilter() override = default;

  PointCloud* Apply(PointCloud* pc) override;
  void SetParams(const FilterParametr& param) override;

private:
  BoxRegion region_;
};

}  

#endif  // POINTCLOUD_PREPROCESSOR__CROP_BOX_FILTER_HPP_

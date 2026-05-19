#include "crop_box_filter.hpp"

#include <vector>
#include <cmath>
#include <string>

namespace pointcloud_preprocessor
{

CropBoxFilter::CropBoxFilter()
: Filter("CropBoxFilter")
{
}

PointCloud* CropBoxFilter::Apply(PointCloud* pc)
{
  if (!pc || pc->point_size_ < 3) {
      return new PointCloud();
  }

  PointCloud* output_pc = new PointCloud();
  output_pc->pointcloud_type_ = pc->pointcloud_type_;
  output_pc->point_size_ = pc->point_size_;
  
  output_pc->points_.reserve(pc->points_.size());

  size_t output_points_count = 0;
  const size_t max_offset = pc->size_ * pc->point_size_;

  for (size_t offset = 0; offset + pc->point_size_ <= max_offset; offset += pc->point_size_) {
       
    double x = pc->points_[offset + 0];
    double y = pc->points_[offset + 1];
    double z = pc->points_[offset + 2];

    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
      logger_.log("Ignoring point containing NaN values");
      continue;
    }

    if (region_.IsInside(x, y, z)) {
      auto start_it = pc->points_.begin() + offset;
      output_pc->points_.insert(output_pc->points_.end(), start_it, start_it + pc->point_size_);
      output_points_count++;
    }
  }

  output_pc->size_ = output_points_count;
  return output_pc;
}

void CropBoxFilter::SetParams(const FilterParametr& param)
{
  double nx = param.GetParam("min_x", region_.GetMinX());
  double nxx = param.GetParam("max_x", region_.GetMaxX());
  double ny = param.GetParam("min_y", region_.GetMinY());
  double nyy = param.GetParam("max_y", region_.GetMaxY());
  double nz = param.GetParam("min_z", region_.GetMinZ());
  double nzz = param.GetParam("max_z", region_.GetMaxZ());
  
  bool neg = static_cast<bool>(param.GetParam("negative", region_.GetNegative() ? 1.0 : 0.0));

  BoxRegion new_region;
  new_region.Update(nx, nxx, ny, nyy, nz, nzz, neg);

  if (region_ != new_region) {
      logger_.log("[paramCallback] Setting the minimum point to: " +
        std::to_string(nx) + " " + std::to_string(ny) + " " + std::to_string(nz));
      logger_.log("[paramCallback] Setting the maximum point to: " +
        std::to_string(nxx) + " " + std::to_string(nyy) + " " + std::to_string(nzz));
      
      logger_.log("[paramCallback] Setting the filter negative flag to: " + std::string(neg ? "true" : "false"));
      
      region_ = new_region;
  }
  
  Filter::SetParams(param);
}

}  // namespace pointcloud_preprocessor
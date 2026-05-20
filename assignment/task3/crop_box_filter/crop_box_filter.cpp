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
  if (!pc || pc->GetPointSize() < 3) {
      return new PointCloud();
  }

  PointCloud* output_pc = new PointCloud();

  output_pc->Init(pc->GetPointCloudType(), 0); 
  output_pc->GetPointsMutable().reserve(pc->GetPoints().size());

  size_t output_points_count = 0;
  
  // кэшируем, чтобы не вызывать функции на каждой итерации цикла.
  const size_t point_size = pc->GetPointSize();
  const size_t max_offset = pc->GetSize() * point_size;
  const auto& input_points = pc->GetPoints(); 

  for (size_t offset = 0; offset + point_size <= max_offset; offset += point_size) {
    double x = input_points[offset + 0];
    double y = input_points[offset + 1];
    double z = input_points[offset + 2];

    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
      logger_.log("Ignoring point containing NaN values");
      continue;
    }

    if (region_.IsInside(x, y, z)) {
      auto start_it = input_points.begin() + offset;
      
      output_pc->GetPointsMutable().insert(
          output_pc->GetPointsMutable().end(), 
          start_it, 
          start_it + point_size
      );
      output_points_count++;
    }
  }

  output_pc->Init(pc->GetPointCloudType(), output_points_count);
  
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
#ifndef POINTCLOUD_H_
#define POINTCLOUD_H_

#include <string>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <algorithm>

// The lidar's pointcloud contains coordinates of point (X, Y and Z) and Intensity of ret beam for point (I) and beam ring (R)
// Also it can contain Distance (D), Azimuth (A) and Timestamp (T)

class PointCloud {
public:
    static size_t GetPointSizeForType(const std::string& pc_type) {
        static const std::unordered_map<std::string, size_t> type_to_size = {
            {"XYZIR", 5},
            {"XYZIRDAT", 8}
        };
        if (auto it = type_to_size.find(pc_type); it != type_to_size.end()) {
            return it->second;
        }
        throw std::runtime_error("Unknown pointcloud type: " + pc_type);
    }

	void Init(std::string pc_type, size_t points_count) {
		pointcloud_type_ = std::move(pc_type);
		size_ = points_count;
        
        point_size_ = GetPointSizeForType(pointcloud_type_);
        points_.assign(size_ * point_size_, 0.0);
	}

    template <typename Iterator>
    void AddPoint(Iterator begin, Iterator end, size_t ind) {
        if (static_cast<size_t>(std::distance(begin, end)) != point_size_) {
            throw std::runtime_error("Iterator range has invalid size for type " + pointcloud_type_);
        }
        if (ind >= size_) {
            throw std::out_of_range("Index out of bounds in PointCloud::AddPoint");
        }

        size_t offset = point_size_ * ind;
        std::copy(begin, end, points_.begin() + offset);
    }

    const std::string& GetPointCloudType() const { return pointcloud_type_; }
    size_t GetSize() const { return size_; }
    size_t GetPointSize() const { return point_size_; }
    
    const std::vector<double>& GetPoints() const { return points_; }
    
    std::vector<double>& GetPointsMutable() { return points_; }

private:
	std::string pointcloud_type_{""};
	size_t size_ = 0;
	size_t point_size_ = 0;
	std::vector<double> points_;
};


// For tests
inline void FillPointCloud(PointCloud* pc, size_t points_count, const std::string& point_type, const std::vector<double>& data) {
    if (!pc) return;

	pc->Init(point_type, points_count);
    
    const size_t point_size = pc->GetPointSize(); 

    if (data.size() < points_count * point_size) {
        throw std::runtime_error("Insufficient data provided to FillPointCloud");
    }

	for (size_t ind = 0; ind < points_count; ++ind) {
        auto start_it = data.begin() + point_size * ind;
		pc->AddPoint(start_it, start_it + point_size, ind);
	}
}

#endif //POINTCLOUD_H_
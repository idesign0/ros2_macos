// Copyright 2024 Ryan Friedman
#pragma once
#include <geographic_msgs/msg/geo_point.hpp>

namespace geographic_msgs
{

//! @brief Return whether a GeoPoint has latitude and longitude within expected bounds.
[[nodiscard]] inline static bool horizontalPositionValid(const geographic_msgs::msg::GeoPoint point)
{
  auto const lat_valid = point.latitude <= 90.0 && point.latitude >= -90.0;
  auto const lng_valid = point.longitude <= 180.0 && point.longitude >= -180.0;

  return lat_valid && lng_valid;
}


}  // namespace geographic_msgs

// Copyright 2024 Ryan Friedman

#include <gtest/gtest.h>
#include <geographic_msgs/msg/geo_point.hpp>
#include <geographic_msgs/validation.hpp>


TEST(ElevationServerCore, ValidPoints)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = 1.0;
  point.longitude = 10.0;
  EXPECT_TRUE(geographic_msgs::horizontalPositionValid(point));
}

TEST(ElevationServerCore, TooFarNorth)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = 90.01;
  point.longitude = 0.0;
  EXPECT_FALSE(geographic_msgs::horizontalPositionValid(point));
}

TEST(ElevationServerCore, TooFarSouth)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = -90.01;
  point.longitude = 0.0;
  EXPECT_FALSE(geographic_msgs::horizontalPositionValid(point));
}

TEST(ElevationServerCore, TooFarWest)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = 0.0;
  point.longitude = -180.1;
  EXPECT_FALSE(geographic_msgs::horizontalPositionValid(point));
}

TEST(ElevationServerCore, TooFarEast)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = 0.0;
  point.longitude = 180.1;
  EXPECT_FALSE(geographic_msgs::horizontalPositionValid(point));
}

TEST(ElevationServerCore, NorthPole)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = 90.0;
  point.longitude = 42.0;
  EXPECT_TRUE(geographic_msgs::horizontalPositionValid(point));
}

TEST(ElevationServerCore, SouthPole)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = -90.0;
  point.longitude = 42.0;
  EXPECT_TRUE(geographic_msgs::horizontalPositionValid(point));
}

TEST(ElevationServerCore, NorthPoleInvalidLng)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = 90.0;
  point.longitude = 999.0;
  EXPECT_FALSE(geographic_msgs::horizontalPositionValid(point));
}

TEST(ElevationServerCore, SouthPoleInvalidLng)
{
  geographic_msgs::msg::GeoPoint point;
  point.latitude = -90.0;
  point.longitude = -999.0;
  EXPECT_FALSE(geographic_msgs::horizontalPositionValid(point));
}

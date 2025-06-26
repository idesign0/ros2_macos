// Copyright 2008 Willow Garage, Inc.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the Willow Garage, Inc. nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/**
 * \file
 *
 * Implements TransformCollection class
 *
 * \author Bhaskara Marthi
 */

#include <warehouse_ros/transform_collection.h>
#include <boost/foreach.hpp>

static const rclcpp::Logger LOGGER = rclcpp::get_logger("warehouse_ros.transform_collection");

namespace warehouse_ros
{
geometry_msgs::msg::TransformStamped TransformCollection::lookupTransform(const std::string& target,
                                                                          const std::string& src, const double t) const
{
  // Query all transforms between t-search_back_ and t+search_forward_
  Query::Ptr q = coll_.createQuery();
  q->appendRangeInclusive("stamp", t - search_back_, t + search_forward_);

  // Iterate over the messages and add them to a Transformer
  tf2::Duration time_out = tf2::durationFromSec(search_back_ + search_forward_ * 1.1);
  typename QueryResults<tf2_msgs::msg::TFMessage>::range_t res = coll_.query(q);
  for (ResultIterator<tf2_msgs::msg::TFMessage> it = res.first; it != res.second; ++it)
  {
    for (const geometry_msgs::msg::TransformStamped& trans : (*it)->transforms)
    {
      const geometry_msgs::msg::Vector3& v = trans.transform.translation;
      const geometry_msgs::msg::Quaternion& q = trans.transform.rotation;
      const std_msgs::msg::Header& h = trans.header;
      const tf2::Transform tr(tf2::Quaternion(q.x, q.y, q.z, q.w), tf2::Vector3(v.x, v.y, v.z));
      geometry_msgs::msg::TransformStamped t =
          tf2::toMsg(tf2::Stamped<tf2::Transform>(tr, tf2::getTimestamp(trans), h.frame_id));
      t.child_frame_id = trans.child_frame_id;
      const bool ok = tf_buffer_->setTransform(t, "TransformCollection");
      RCLCPP_FATAL_EXPRESSION(LOGGER, ok, "Tf setTransform returned false for transform from %s to %s at %.4f",
                              trans.child_frame_id.c_str(), h.frame_id.c_str(),
                              tf2::timeToSec(tf2::getTimestamp(trans)));
    }
  }
  geometry_msgs::msg::TransformStamped result =
      tf_buffer_->lookupTransform(target, src, tf2::timeFromSec(t), time_out);  // Can throw
  return result;
}

void TransformCollection::putTransform(geometry_msgs::msg::TransformStamped)
{
}

}  // namespace warehouse_ros

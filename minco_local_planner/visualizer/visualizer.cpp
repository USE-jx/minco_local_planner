/**
 * @Author: Xia Yunkai
 * @Date:   2023-08-27 22:12:32
 * @Last Modified by:   Yunkai Xia
 * @Last Modified time: 2023-08-30 10:59:37
 */
#include "visualizer.h"

#include <iostream>

#include "basis/rigid2d.h"
using namespace std;

namespace visualizer {

Visualizer::Visualizer() {
  grid_map_pub_ = nh_.advertise<nav_msgs::OccupancyGrid>("grid_map", 1);
  transformed_pcd_pub_ =
      nh_.advertise<sensor_msgs::PointCloud2>("transformed_pcd", 1);

  safety_bounding_boxes_pub_ = nh_.advertise<visualization_msgs::MarkerArray>(
      "safety_bounding_boxes", 1);
  bezier_segments_pub_ =
      nh_.advertise<visualization_msgs::MarkerArray>("bezier_segments", 1);
}

void Visualizer::GridMapVis(const Pose2d &origin, const Vec2i &dim,
                            const std::vector<int8_t> &data, const double &res,
                            const std::string &frame_id) {
  nav_msgs::OccupancyGrid grid_map_ros;
  grid_map_ros.header.frame_id = frame_id;
  geometry_msgs::Pose pose;
  pose.position.x = origin.x();
  pose.position.y = origin.y();
  const auto oritation = GetQuaterion(origin);
  pose.orientation.w = oritation.w();
  pose.orientation.x = oritation.x();
  pose.orientation.y = oritation.y();
  pose.orientation.z = oritation.z();
  grid_map_ros.info.map_load_time = ros::Time::now();
  grid_map_ros.header.stamp = ros::Time::now();
  grid_map_ros.data = data;
  grid_map_ros.info.resolution = res;
  grid_map_ros.info.width = dim.x();
  grid_map_ros.info.height = dim.y();
  grid_map_ros.info.origin = pose;

  grid_map_pub_.publish(grid_map_ros);
}

void Visualizer::TransformedPcdVis(const PointCloud3d &cloud,
                                   const std::string frame_id) {
  sensor_msgs::PointCloud2 cloud_msg;
  pcl::toROSMsg(cloud, cloud_msg);
  cloud_msg.header.frame_id = frame_id;

  transformed_pcd_pub_.publish(cloud_msg);
}

void Visualizer::SafetyBoundingBoxesVis(const std::map<int, BoundingBox> &boxes,
                                        const std::string frame_id) {
  visualization_msgs::MarkerArray safety_boxes_vis;

  int id = 0;
  for (auto &box : boxes) {
    std::string box_name = box.second.Name();
    visualization_msgs::Marker box_vis;
    if (box_name == "vehicle") {
      box_vis = newLineStrip(0.01, box.second.Name(), ++id, BLACK, frame_id);
    } else if (box_name == "stop") {
      box_vis = newLineStrip(0.01, box.second.Name(), ++id, RED, frame_id);
    } else if (box_name == "creep") {
      box_vis = newLineStrip(0.01, box.second.Name(), ++id, CYAN, frame_id);
    } else if (box_name == "slow_down") {
      box_vis = newLineStrip(0.01, box.second.Name(), ++id, YELLOW, frame_id);
    } else {
      return;
    }

    for (auto &pt : box.second.GetPoints()) {
      geometry_msgs::Point point;
      point.x = pt.x();
      point.y = pt.y();
      box_vis.points.emplace_back(point);
    }
    safety_boxes_vis.markers.emplace_back(box_vis);
  }
  safety_bounding_boxes_pub_.publish(safety_boxes_vis);
}

void Visualizer::BezierSegmentsVis(const Path2d &bezier_segment_path,
                                   const Points2d &all_control_points,
                                   const std::string &frame_id) {
  if (bezier_segment_path.size() == 0 || all_control_points.size() == 0) {
    return;
  }

  visualization_msgs::MarkerArray bezier_segments_vis;
  visualization_msgs::Marker bezier_path_vis =
      newLineStrip(0.01, "bezier_path", 1, RED, frame_id);
  visualization_msgs::Marker bezier_control_points_vis =
      newSphereList(0.08, "bezier_control_points", 2, BLUE, frame_id);
  geometry_msgs::Point pt_vis;
  for (auto &pt : bezier_segment_path) {
    pt_vis.x = pt.x();
    pt_vis.y = pt.y();
    bezier_path_vis.points.emplace_back(pt_vis);
  }
  for (auto &control_point : all_control_points) {
    pt_vis.x = control_point[0];
    pt_vis.y = control_point[1];
    bezier_control_points_vis.points.emplace_back(pt_vis);
  }

  bezier_segments_vis.markers.emplace_back(bezier_path_vis);
  bezier_segments_vis.markers.emplace_back(bezier_control_points_vis);
  bezier_segments_pub_.publish(bezier_segments_vis);
}
}  // namespace visualizer
#include <cstdio>
#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/image_encodings.hpp"
#include "sensor_msgs/fill_image.hpp"
#include "asdfr_interfaces/msg/point2.hpp" // 2D point (x and y coordinates)
#include "geometry_msgs/msg/pose2_d.hpp"

#include "opencv2/opencv.hpp"
#include "cv_bridge/cv_bridge.h"

using std::placeholders::_1;
using namespace std::chrono_literals;

class JCLightTrackerNode : public rclcpp::Node
{
  rclcpp::Publisher<asdfr_interfaces::msg::Point2>::SharedPtr publisher_;
  rclcpp::Subscription<geometry_msgs::msg::Pose2D>::SharedPtr subscription_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscript_im_;
  float midY,midX = 0;
  float cx,cy = 0;



  
public:
  JCLightTrackerNode() : Node("jiwy_controller")
  {
      subscription_ = this->create_subscription<geometry_msgs::msg::Pose2D>(
      "COGL", 10, std::bind(&JCLightTrackerNode::topic_callback, this, _1));    
      subscript_im_ = this->create_subscription<sensor_msgs::msg::Image>(
      "im_COGL", 10, std::bind(&JCLightTrackerNode::image_callback, this, _1));
      publisher_ = this->create_publisher<asdfr_interfaces::msg::Point2>("setpoint",10);
  }

private:
  void topic_callback(const geometry_msgs::msg::Pose2D & pos)
  {
    RCLCPP_INFO(this->get_logger(), "I heard: '%f and %f'", pos.x, pos.y);
    float pan, tilt;

    cx = pos.x;
    cy = pos.y;

    // Output the actual position
    asdfr_interfaces::msg::Point2 jc_pos;
    jc_pos.x = pan;
    jc_pos.y = tilt;
    publisher_->publish(jc_pos);

  }

  void image_callback(const sensor_msgs::msg::Image::SharedPtr image_msg)
  {    
    // Convert the image message to an OpenCV Mat object and greyscale
    cv::Mat image = cv_bridge::toCvCopy(image_msg,  "rgb8")->image;

    midX = image.rows/2 - cx;
    midY = image.cols/2 - cy;

    // remap to centre
    cv::Mat transform = cv::Mat::eye(3,3,CV_64F);
    transform.at<double>(0,2) = midX;
    transform.at<double>(1,2) = midY;

    cv::Mat output;
    cv::warpPerspective(image, output, transform, image.size());

    cv::imshow("light follow", output );
    cv::waitKey(1);
  }
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<JCLightTrackerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
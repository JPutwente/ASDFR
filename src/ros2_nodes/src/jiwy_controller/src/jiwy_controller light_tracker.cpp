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

using std::placeholders::_1;
using namespace std::chrono_literals;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds


class JiwyControllerNode : public rclcpp::Node
{
  rclcpp::TimerBase::SharedPtr timer_;

  rclcpp::Publisher<asdfr_interfaces::msg::Point2>::SharedPtr publisher_;
  rclcpp::Subscription<geometry_msgs::msg::Pose2D::SharedPtr subscription_;
  
public:
  JiwyControllerNode() : Node("jiwy_controller")
  {
    subscription_ = this->create_subscription<geometry_msgs::msg::Pose2D>("COGL", 10, std::bind(&JiwyControllerNode::image_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<asdfr_interfaces::msg::Point2>("setpoint",10);
          timer_ = this->create_wall_timer(
      2s, std::bind(&JiwyControllerNode::image_callback, this));
  }

private:
  void image_callback()
  {
    // Output the actual position
    asdfr_interfaces::msg::Point2 pos;
    pos.x = (float)(rand() % 160 - 80)/100;
    pos.y = (float)(rand() % 120 - 60)/100;
    publisher_->publish(pos);

  }
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<JiwyControllerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
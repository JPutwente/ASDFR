#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "opencv2/opencv.hpp"
#include "cv_bridge/cv_bridge.h"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;

class GrayScalerNode : public rclcpp::Node
{
public:
  GrayScalerNode() : Node("grayscaler_node")
  {
    subscription_ = this->create_subscription<sensor_msgs::msg::Image>("image", 10, std::bind(&GrayScalerNode::image_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<sensor_msgs::msg::Image>("image_gs", 10);
  }

private:
  void image_callback(const sensor_msgs::msg::Image::SharedPtr image_msg)
  {
     
    // Convert the image message to an OpenCV Mat object and greyscale
    cv::Mat image = cv_bridge::toCvCopy(image_msg,  "mono8")->image;

    // Convert grayscaled image to msg
    sensor_msgs::msg::Image::SharedPtr msg =
        cv_bridge::CvImage(std_msgs::msg::Header(), "mono8", image)
            .toImageMsg();
    
    publisher_->publish(*msg.get());
  }
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<GrayScalerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "opencv2/opencv.hpp"
#include "cv_bridge/cv_bridge.h"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;


class ImageBrightnessNode : public rclcpp::Node
{
public:
  ImageBrightnessNode() : Node("image_brightness_node")
  {
    this->declare_parameter("darkness_threshold", 20.0);
    subscription_ = this->create_subscription<sensor_msgs::msg::Image>("image", 10, std::bind(&ImageBrightnessNode::image_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<std_msgs::msg::String>("switchstate", 10);
    
    darkness_threshold_ = this->get_parameter("darkness_threshold").as_double();
  }

private:
  void image_callback(const sensor_msgs::msg::Image::SharedPtr image_msg)
  {
     
    // Convert the image message to an OpenCV Mat object
    cv::Mat image = cv_bridge::toCvCopy(image_msg,  "mono8")->image;

    

    // Calculate the brightness value of the image
    double brightness = cv::mean(image)[0];
    
    double darkness_threshold_ = this->get_parameter("darkness_threshold").get_parameter_value().get<double>();
    RCLCPP_INFO(this->get_logger(), "threshold %f!", darkness_threshold_);

    auto message = std_msgs::msg::String();
     if (brightness < darkness_threshold_)
    {
        //RCLCPP_INFO(this->get_logger(), "Light switched off");
        message.data = "It is dark";
    }
    else
    {
        //RCLCPP_INFO(this->get_logger(), "Light switched on");
        message.data = "It is light";
    }
    publisher_->publish(message);
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
  double darkness_threshold_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ImageBrightnessNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "opencv2/opencv.hpp"
#include "cv_bridge/cv_bridge.h"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;


class BrightnessMask : public rclcpp::Node
{
public:
  BrightnessMask() : Node("brightmask_node")
  {
    this->declare_parameter("darkness_threshold", 250.0);
    darkness_threshold_ = this->get_parameter("darkness_threshold").as_double();

    subscription_ = this->create_subscription<sensor_msgs::msg::Image>("image_gs", 10, std::bind(&BrightnessMask::image_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<sensor_msgs::msg::Image>("image_msk", 10);
  }

private:
    void image_callback(const sensor_msgs::msg::Image::SharedPtr image_msg)
    {
      double darkness_threshold_ = this->get_parameter("darkness_threshold").get_parameter_value().get<double>();

      RCLCPP_INFO(this->get_logger(), "threshold %f!", darkness_threshold_);   

      // Convert the image message to an OpenCV Mat object
      cv::Mat image_gs = cv_bridge::toCvCopy(image_msg,  "mono8")->image;
      cv::Mat image_th;
      cv::threshold(image_gs, image_th, darkness_threshold_, 255, cv::THRESH_BINARY_INV);

      // show image
      cv::imshow("im_th", image_th);
      cv::waitKey(3);

      // OpenCV -> Msg
      sensor_msgs::msg::Image::SharedPtr msg =
          cv_bridge::CvImage(std_msgs::msg::Header(), "mono8", image_th)
          .toImageMsg();

      // publish
      publisher_->publish(*msg.get());

    }
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    double darkness_threshold_;

};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<BrightnessMask>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
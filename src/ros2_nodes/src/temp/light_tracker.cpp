#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "opencv2/opencv.hpp"
#include "cv_bridge/cv_bridge.h"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/pose2_d.hpp"

using std::placeholders::_1;


class LightTracker : public rclcpp::Node
{
public:
  LightTracker() : Node("lighttracker_node")
  {
    subscription_ = this->create_subscription<sensor_msgs::msg::Image>("image_msk", 10, std::bind(&LightTracker::image_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<geometry_msgs::msg::Pose2D>("COGL", 10);
    publish_im_ = this->create_publisher<sensor_msgs::msg::Image>("im_COGL", 10);

  }

private:
    void image_callback(const sensor_msgs::msg::Image::SharedPtr image_msg)
    {

        // Pos variables
        geometry_msgs::msg::Pose2D Pos;

        // Convert the image message to an OpenCV Mat object
        cv::Mat image = cv_bridge::toCvCopy(image_msg,  "mono8")->image;
       
        // Setup SimpleBlobDetector parameters.
        cv::SimpleBlobDetector::Params params;
        
        // Change thresholds
        params.minThreshold = 0;
        params.maxThreshold = 255;
        
        // Filter by Area.
        params.filterByArea = true;
        params.minArea = 100;
        params.maxArea = 2000;

        
        // Filter by Circularity
        params.filterByCircularity = true;
        params.minCircularity = 0.5;
        params.maxCircularity = 1;

        // Filter by Convexity
        params.filterByConvexity = false;
        params.minConvexity = 0.0;
        
        // Filter by Inertia
        params.filterByInertia = false;
        params.minInertiaRatio = 0.01;

        std::vector<cv::KeyPoint> keypoints;
        
        // Set up detector with params
        cv::Ptr<cv::FeatureDetector> featureDetector(
        cv::SimpleBlobDetector::create(params));    

        // You can use the detector this way
        featureDetector->detect(image, keypoints);
        
        // Draw detected blobs as red circles.
        // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
        cv::Mat im_with_keypoints;
        cv::drawKeypoints( image, keypoints, im_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

        // Show blobs
        cv::imshow("keypoints", im_with_keypoints );
        cv::waitKey(1);

        for (int i=0; i<(int)keypoints.size(); i++){
            Pos.x = keypoints[i].pt.x; 
            Pos.y = keypoints[i].pt.y;
        }

        // publish
        publisher_->publish(Pos);

        // Convert COGL image to msg
        sensor_msgs::msg::Image::SharedPtr im_msg =
            cv_bridge::CvImage(std_msgs::msg::Header(), "rgb8", im_with_keypoints)
                .toImageMsg();
        
        publish_im_->publish(*im_msg.get());
    }
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<geometry_msgs::msg::Pose2D>::SharedPtr publisher_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publish_im_;

};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<LightTracker>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
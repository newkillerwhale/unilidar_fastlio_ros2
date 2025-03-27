#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"

class OdomToTF : public rclcpp::Node {
public:
    OdomToTF() : Node("odom_to_tf") {
        // 初始化订阅者，订阅 /Odometry 话题
        subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/Odometry", 10,
            std::bind(&OdomToTF::odom_callback, this, std::placeholders::_1));
        
        // 初始化 TF 广播器
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        geometry_msgs::msg::TransformStamped transform;

        // 设置时间戳（使用消息中的时间戳）
        transform.header.stamp = msg->header.stamp;
        // transform.header.stamp = this->now();  // 或使用当前时间（根据需求选择）

        // 设置坐标系名称
        transform.header.frame_id = "odom";  // 通常是 "odom"
        transform.child_frame_id = "livox_frame";    // 通常是 "base_link"

        // 填充位置和姿态
        transform.transform.translation.x = msg->pose.pose.position.x;
        transform.transform.translation.y = msg->pose.pose.position.y;
        transform.transform.translation.z = msg->pose.pose.position.z;
        transform.transform.rotation = msg->pose.pose.orientation;

        // 广播 TF
        tf_broadcaster_->sendTransform(transform);
    }

    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<OdomToTF>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

/*
 * debug.cpp
 * Debug tool
 * Author MapIV Sekino
 * Ver 1.00 2019/3/28
 */

#include "ros/ros.h"
#include "eagleye_msgs/Debug.h"
#include "eagleye_msgs/Distance.h"
#include "eagleye_msgs/Heading.h"
#include "eagleye_msgs/Position.h"
#include "eagleye_msgs/Status.h"
#include "eagleye_msgs/VelocityScaleFactor.h"
#include "eagleye_msgs/YawrateOffset.h"
#include "rtklib_msgs/RtklibNav.h"
#include "sensor_msgs/NavSatFix.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/Vector3Stamped.h"

static rtklib_msgs::RtklibNav rtklib_nav;
static sensor_msgs::NavSatFix fix;
static geometry_msgs::TwistStamped velocity;
static eagleye_msgs::VelocityScaleFactor velocity_scale_factor;
static eagleye_msgs::Distance distance;
static eagleye_msgs::Heading heading_1st;
static eagleye_msgs::Heading heading_interpolate_1st;
static eagleye_msgs::Heading heading_2nd;
static eagleye_msgs::Heading heading_interpolate_2nd;
static eagleye_msgs::Heading heading_3rd;
static eagleye_msgs::Heading heading_interpolate_3rd;
static eagleye_msgs::YawrateOffset yawrate_offset_stop;
static eagleye_msgs::YawrateOffset yawrate_offset_1st;
static eagleye_msgs::YawrateOffset yawrate_offset_2nd;
static eagleye_msgs::Position enu_relative_pos;
static geometry_msgs::Vector3Stamped enu_vel;
static eagleye_msgs::Position enu_absolute_pos;
static eagleye_msgs::Position enu_absolute_pos_interpolate;
static sensor_msgs::NavSatFix eagleye_fix;
static geometry_msgs::TwistStamped eagleye_twist;

static ros::Publisher pub;
static eagleye_msgs::Debug debug;

void rtklib_nav_callback(const rtklib_msgs::RtklibNav::ConstPtr& msg)
{
  rtklib_nav.header = msg->header;
  rtklib_nav.tow = msg->tow;
  rtklib_nav.ecef_pos = msg->ecef_pos;
  rtklib_nav.ecef_vel = msg->ecef_vel;
  rtklib_nav.status = msg->status;
}

void fix_callback(const sensor_msgs::NavSatFix::ConstPtr& msg)
{
  fix.header = msg->header;
  fix.status = msg->status;
  fix.latitude = msg->latitude;
  fix.longitude = msg->longitude;
  fix.altitude = msg->altitude;
  fix.position_covariance = msg->position_covariance;
  fix.position_covariance_type = msg->position_covariance_type;
}

void velocity_callback(const geometry_msgs::TwistStamped::ConstPtr& msg)
{
  velocity.header = msg->header;
  velocity.twist = msg->twist;
}

void velocity_scale_factor_callback(const eagleye_msgs::VelocityScaleFactor::ConstPtr& msg)
{
  velocity_scale_factor.header = msg->header;
  velocity_scale_factor.scale_factor = msg->scale_factor;
  velocity_scale_factor.correction_velocity = msg->correction_velocity;
  velocity_scale_factor.status = msg->status;
}

void distance_callback(const eagleye_msgs::Distance::ConstPtr& msg)
{
  distance.header = msg->header;
  distance.distance = msg->distance;
  distance.status = msg->status;
}

void heading_1st_callback(const eagleye_msgs::Heading::ConstPtr& msg)
{
  heading_1st.header = msg->header;
}

void heading_interpolate_1st_callback(const eagleye_msgs::Heading::ConstPtr& msg)
{
  heading_interpolate_1st.header = msg->header;
  heading_interpolate_1st.heading_angle = msg->heading_angle;
  heading_interpolate_1st.status = msg->status;
}

void heading_2nd_callback(const eagleye_msgs::Heading::ConstPtr& msg)
{
  heading_2nd.header = msg->header;
  heading_2nd.heading_angle = msg->heading_angle;
  heading_2nd.status = msg->status;
}

void heading_interpolate_2nd_callback(const eagleye_msgs::Heading::ConstPtr& msg)
{
  heading_interpolate_2nd.header = msg->header;
  heading_interpolate_2nd.heading_angle = msg->heading_angle;
  heading_interpolate_2nd.status = msg->status;
}

void heading_3rd_callback(const eagleye_msgs::Heading::ConstPtr& msg)
{
  heading_3rd.header = msg->header;
  heading_3rd.heading_angle = msg->heading_angle;
  heading_3rd.status = msg->status;
}

void heading_interpolate_3rd_callback(const eagleye_msgs::Heading::ConstPtr& msg)
{
  heading_interpolate_3rd.header = msg->header;
  heading_interpolate_3rd.heading_angle = msg->heading_angle;
  heading_interpolate_3rd.status = msg->status;
}

void yawrate_offset_stop_callback(const eagleye_msgs::YawrateOffset::ConstPtr& msg)
{
  yawrate_offset_stop.header = msg->header;
  yawrate_offset_stop.yawrate_offset = msg->yawrate_offset;
  yawrate_offset_stop.status = msg->status;
}

void yawrate_offset_1st_callback(const eagleye_msgs::YawrateOffset::ConstPtr& msg)
{
  yawrate_offset_1st.header = msg->header;
  yawrate_offset_1st.yawrate_offset = msg->yawrate_offset;
  yawrate_offset_1st.status = msg->status;
}

void yawrate_offset_2nd_callback(const eagleye_msgs::YawrateOffset::ConstPtr& msg)
{
  yawrate_offset_2nd.header = msg->header;
  yawrate_offset_2nd.yawrate_offset = msg->yawrate_offset;
  yawrate_offset_2nd.status = msg->status;
}

void enu_relative_pos_callback(const eagleye_msgs::Position::ConstPtr& msg)
{
  enu_relative_pos.header = msg->header;
  enu_relative_pos.enu_pos = msg->enu_pos;
  enu_relative_pos.ecef_base_pos = msg->ecef_base_pos;
  enu_relative_pos.status = msg->status;
}

void enu_vel_callback(const geometry_msgs::Vector3Stamped::ConstPtr& msg)
{
  enu_vel.header = msg->header;
  enu_vel.vector = msg->vector;
}

void enu_absolute_pos_callback(const eagleye_msgs::Position::ConstPtr& msg)
{
  enu_absolute_pos.header = msg->header;
  enu_absolute_pos.enu_pos = msg->enu_pos;
  enu_absolute_pos.ecef_base_pos = msg->ecef_base_pos;
  enu_absolute_pos.status = msg->status;
}

void enu_absolute_pos_interpolate_callback(const eagleye_msgs::Position::ConstPtr& msg)
{
  enu_absolute_pos_interpolate.header = msg->header;
  enu_absolute_pos_interpolate.enu_pos = msg->enu_pos;
  enu_absolute_pos_interpolate.ecef_base_pos = msg->ecef_base_pos;
  enu_absolute_pos_interpolate.status = msg->status;
}

void eagleye_fix_callback(const sensor_msgs::NavSatFix::ConstPtr& msg)
{
  eagleye_fix.header = msg->header;
  eagleye_fix.status = msg->status;
  eagleye_fix.latitude = msg->latitude;
  eagleye_fix.longitude = msg->longitude;
  eagleye_fix.altitude = msg->altitude;
  eagleye_fix.position_covariance = msg->position_covariance;
  eagleye_fix.position_covariance_type = msg->position_covariance_type;
}

void eagleye_twist_callback(const geometry_msgs::TwistStamped::ConstPtr& msg)
{
  eagleye_twist.header = msg->header;
  eagleye_twist.twist = msg->twist;
}

void imu_callback(const sensor_msgs::Imu::ConstPtr& msg)
{
  debug.header.stamp = ros::Time::now();
  debug.header.frame_id = "";
  debug.imu.header = msg->header;
  debug.imu.orientation = msg->orientation;
  debug.imu.orientation_covariance = msg->orientation_covariance;
  debug.imu.angular_velocity = msg->angular_velocity;
  debug.imu.angular_velocity_covariance = msg->angular_velocity_covariance;
  debug.imu.linear_acceleration = msg->linear_acceleration;
  debug.imu.linear_acceleration_covariance = msg->linear_acceleration_covariance;
  debug.rtklib_nav = rtklib_nav;
  debug.fix = fix;
  debug.velocity = velocity;
  debug.velocity_scale_factor = velocity_scale_factor;
  debug.distance = distance;
  debug.heading_1st = heading_1st;
  debug.heading_interpolate_1st = heading_interpolate_1st;
  debug.heading_2nd = heading_2nd;
  debug.heading_interpolate_2nd = heading_interpolate_2nd;
  debug.heading_3rd = heading_3rd;
  debug.heading_interpolate_3rd = heading_interpolate_3rd;
  debug.yawrate_offset_stop = yawrate_offset_stop;
  debug.yawrate_offset_1st = yawrate_offset_1st;
  debug.yawrate_offset_2nd = yawrate_offset_2nd;
  debug.enu_relative_pos = enu_relative_pos;
  debug.enu_vel = enu_vel;
  debug.enu_absolute_pos = enu_absolute_pos;
  debug.enu_absolute_pos_interpolate = enu_absolute_pos_interpolate;
  debug.eagleye_fix = eagleye_fix;
  debug.eagleye_twist = eagleye_twist;

  std::cout<<"eagleye status"<<std::endl;
  std::cout<<"imu(input)"<<std::endl;
  std::cout<<"linear_acceleration x "<<debug.imu.linear_acceleration.x<<" m/s^2"<<std::endl;
  std::cout<<"linear acceleration y "<<debug.imu.linear_acceleration.y<<" m/s^2"<<std::endl;
  std::cout<<"linear acceleration z "<<debug.imu.linear_acceleration.z<<" m/s^2"<<std::endl;
  std::cout<<"angular_velocity x "<<debug.imu.angular_velocity.x<<" rad/s"<<std::endl;
  std::cout<<"angular_velocity y "<<debug.imu.angular_velocity.y<<" rad/s"<<std::endl;
  std::cout<<"angular_velocity z "<<debug.imu.angular_velocity.z<<" rad/s"<<std::endl;
  std::cout<<"velocity(input)"<<std::endl;
  std::cout<<"velocity "<<debug.velocity.twist.linear.x<<" m/s"<<std::endl;
  std::cout<<"rtklib(input)"<<std::endl;
  std::cout<<"time of week "<<debug.rtklib_nav.tow<<" ms"<<std::endl;
  std::cout<<"longitude "<<debug.rtklib_nav.status.longitude<<" deg"<<std::endl;
  std::cout<<"latitude "<<debug.rtklib_nav.status.latitude<<" deg"<<std::endl;
  std::cout<<"altitude "<<debug.rtklib_nav.status.altitude<<" deg"<<std::endl;

  pub.publish(debug);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "debug");

  ros::NodeHandle n;
  ros::Subscriber sub1 = n.subscribe("/imu/data_raw", 1000, imu_callback);
  ros::Subscriber sub2 = n.subscribe("/rtklib_nav", 1000, rtklib_nav_callback);
  ros::Subscriber sub3 = n.subscribe("/fix", 1000, fix_callback);
  ros::Subscriber sub4 = n.subscribe("/can_twist", 1000, velocity_callback);
  ros::Subscriber sub5 = n.subscribe("/eagleye/velocity_scale_factor", 1000, velocity_scale_factor_callback);
  ros::Subscriber sub6 = n.subscribe("/eagleye/distance", 1000, distance_callback);
  ros::Subscriber sub7 = n.subscribe("/eagleye/heading_1st", 1000, heading_1st_callback);
  ros::Subscriber sub8 = n.subscribe("/eagleye/heading_interpolate_1st", 1000, heading_interpolate_1st_callback);
  ros::Subscriber sub9 = n.subscribe("/eagleye/heading_2nd", 1000, heading_2nd_callback);
  ros::Subscriber sub10 = n.subscribe("/eagleye/heading_interpolate_2nd", 1000, heading_interpolate_2nd_callback);
  ros::Subscriber sub11 = n.subscribe("/eagleye/heading_3rd", 1000, heading_3rd_callback);
  ros::Subscriber sub12 = n.subscribe("/eagleye/heading_interpolate_3rd", 1000, heading_interpolate_3rd_callback);
  ros::Subscriber sub13 = n.subscribe("/eagleye/yawrate_offset_stop", 1000, yawrate_offset_stop_callback);
  ros::Subscriber sub14 = n.subscribe("/eagleye/yawrate_offset_1st", 1000, yawrate_offset_1st_callback);
  ros::Subscriber sub15 = n.subscribe("/eagleye/yawrate_offset_2nd", 1000, yawrate_offset_2nd_callback);
  ros::Subscriber sub16 = n.subscribe("/eagleye/enu_relative_pos", 1000, enu_relative_pos_callback);
  ros::Subscriber sub17 = n.subscribe("/eagleye/enu_vel", 1000, enu_vel_callback);
  ros::Subscriber sub18 = n.subscribe("/eagleye/enu_absolute_pos", 1000, enu_absolute_pos_callback);
  ros::Subscriber sub19 = n.subscribe("/eagleye/enu_absolute_pos_interpolate", 1000, enu_absolute_pos_interpolate_callback);
  ros::Subscriber sub20 = n.subscribe("/eagleye/fix", 1000, eagleye_fix_callback);
  ros::Subscriber sub21 = n.subscribe("/eagleye/twist", 1000, eagleye_twist_callback);

  pub = n.advertise<eagleye_msgs::Debug>("/eagleye/debug", 1000);

  ros::spin();

  return 0;
}

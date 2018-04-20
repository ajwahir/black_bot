	#include <ros/ros.h>
	#include <boost/lexical_cast.hpp>
	#include <iostream>
	#include <stdio.h>
	#include <geometry_msgs/Twist.h>
	#include <nav_msgs/Odometry.h>
	#include <iomanip>
	#include <fstream>
	#include <signal.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <string.h>
	#include <termios.h>
	#include <unistd.h>
	#include <cmath>
	#include <tf/transform_broadcaster.h>


	void mySigintHandler(int sig)
	{
	  // Do some custom action.
	  // For example, publish a stop message to some other nodes.

	  // All the default sigint handler does is call shutdown()
	  ros::shutdown();
	}

	double deg2rad(double deg)
	  {
	    return deg * M_PI / 180;
	  }

	
	double vx,w;
	std::string x_str ,y_str;
	void twist_callback(const geometry_msgs::Twist::ConstPtr& msg)
	{
	    vx = msg->linear.x;
	    w = msg->angular.z;
	}


	int main(int argc, char **argv)
	{
	    ros::init(argc, argv, "wheel_odometry");
	    ROS_INFO("Wheel odometry driver is running");
	    ros::NodeHandle nh("~");
	    signal(SIGINT, mySigintHandler);

	    ros::Subscriber twist_sub = nh.subscribe("/black_usb/feedback_twist", 100,twist_callback);
	    ros::Publisher odom_pub = nh.advertise<nav_msgs::Odometry>("black_odom", 10);
	    tf::TransformBroadcaster odom_broadcaster;


	    ros::Rate loop_rate(10);
    	float r=0.15,l=0.55;

    	double x = 0.0;
		double y = 0.0;

		  
		double vy = 0;
		double theta =0;

    	ros::Time current_time, last_time;
  		current_time = ros::Time::now();
  		last_time = ros::Time::now();

    	while (ros::ok())
    	{

    	current_time = ros::Time::now();

	    //compute odometry in a typical way given the velocities of the robot
	    double dt = (current_time - last_time).toSec();
	    double delta_x = (vx * cos(theta)) * dt;
	    double delta_y = (vx * sin(theta)) * dt;
	    double delta_th = w * dt;

	    // std::cout<<theta<<std::endl;


	    x += delta_x;
	    y += delta_y;
	    theta += delta_th;
	    theta = std::fmod(theta,2*3.14159265359);
	    last_time=current_time;

	    //since all odometry is 6DOF we'll need a quaternion created from yaw
	    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(theta);

	    //first, we'll publish the transform over tf
	    geometry_msgs::TransformStamped odom_trans;
	    odom_trans.header.stamp = current_time;
	    odom_trans.header.frame_id = "wheel_odom";
	    odom_trans.child_frame_id = "base_link";

	    odom_trans.transform.translation.x = x;
	    odom_trans.transform.translation.y = y;
	    odom_trans.transform.translation.z = 0.0;
	    odom_trans.transform.rotation = odom_quat;

	    //send the transform
	    //Commented the following out in order to use the robot_localisation 
	    // odom_broadcaster.sendTransform(odom_trans);

	    //next, we'll publish the odometry message over ROS
	    nav_msgs::Odometry odom;

	    for (int i=0;i<36;i++){
			if(i==0 || (i>5 && i%7 == 0))
			odom.pose.covariance[i]=0.01;
			else
			odom.pose.covariance[i]=0.00;
	   }

	    odom.header.stamp = current_time;
	    odom.header.frame_id = "wheel_odom";

	    //set the position
	    odom.pose.pose.position.x = x;
	    odom.pose.pose.position.y = y;
	    odom.pose.pose.position.z = 0.0;
	    odom.pose.pose.orientation = odom_quat;

	    //set the velocity
	    odom.child_frame_id = "base_link";
	    odom.twist.twist.linear.x = vx;
	    odom.twist.twist.linear.y = 0;
	    odom.twist.twist.angular.z = w;

	    //publish the message
	    odom_pub.publish(odom);

        ros::spinOnce();
        loop_rate.sleep();


    }
    return 0;
}
	
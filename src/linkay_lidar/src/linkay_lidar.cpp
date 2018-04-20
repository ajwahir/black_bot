	#include <ros/ros.h>
	#include <boost/lexical_cast.hpp>
	#include <iostream>
	#include <stdio.h>
	#include <sensor_msgs/LaserScan.h>
	#include <iomanip>
	#include <fstream>
	#include <signal.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <string.h>
	#include <termios.h>
	#include <unistd.h>

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

	void FloatToHex(float floatNum,unsigned char* byteArry)
	{
		int i=0;
	    char* pchar=(char*)&floatNum;
	    for(i=0;i<sizeof(float);i++)
	    {
			*byteArry=*pchar;
			pchar++;
			byteArry++;

	    }
	}

	float BytesToFloat(unsigned char *Byte)//,int num)
	{
			return *((float*)Byte);
	}


	int set_interface_attribs(int fd, int speed)
	{
	    struct termios tty;

	    if (tcgetattr(fd, &tty) < 0) {
		printf("Error from tcgetattr: %s\n", strerror(errno));
		return -1;
	    }

	    cfsetospeed(&tty, (speed_t)speed);
	    cfsetispeed(&tty, (speed_t)speed);

	    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
	    tty.c_cflag &= ~CSIZE;
	    tty.c_cflag |= CS8;         /* 8-bit characters */
	    tty.c_cflag &= ~PARENB;     /* no parity bit */
	    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
	    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

	    /* setup for non-canonical mode */
	    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	    tty.c_oflag &= ~OPOST;

	    /* fetch bytes as they become available */
	    tty.c_cc[VMIN] = 1;
	    tty.c_cc[VTIME] = 1;

	    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	    }
	    return 0;
	}

	char n;

	int configure_port(int fd)      // configure the port
	{
		struct termios port_settings;      // structure to store the port settings in

		cfsetispeed(&port_settings, B115200);    // set baud rates
		cfsetospeed(&port_settings, B115200);

		port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits
		port_settings.c_cflag &= ~CSTOPB;
		port_settings.c_cflag &= ~CSIZE;
		port_settings.c_cflag |= CS8;

		tcsetattr(fd, TCSANOW, &port_settings);    // apply the settings to the port
		// ioctl(fd, TCFLSH, 2);
		return(fd);

	}

	int main(int argc, char **argv)
	{
	    ros::init(argc, argv, "linkay_lidar");
	    ROS_INFO("Linkay laser is now running");
	    ros::NodeHandle nh("~");
	    signal(SIGINT, mySigintHandler);

	    ros::Publisher usb_laser_pub = nh.advertise<sensor_msgs::LaserScan>("black_laser", 100);


	    ros::Rate loop_rate(10);
	    std::ofstream us;
		// us.open( "/dev/ttyUSB1");
		// us.open( "/dev/pts/7");
		char *portname = "/dev/ttyUSB1";
		
		int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    	// set_interface_attribs(fd, B115200);
    	configure_port(fd);

    	sensor_msgs::LaserScan laser_buff;
    	float distance_float, current_steps, prev_steps=0;
    	int pub_flag=0;
    	int num_ranges = 200, step_difference = 16;
    	float fov = 6.28, angle_del_middle = deg2rad(90);


   		while (ros::ok())
    	{	

		laser_buff.header.frame_id = "black_laser";
		laser_buff.range_min = 0.2;
		laser_buff.range_max = 20;
		laser_buff.angle_min = -1*fov/2.0;
		laser_buff.angle_max = fov/2.0;
		laser_buff.scan_time = 0.103448;


		// num_ranges = (laser_buff.angle_max-laser_buff.angle_min)/laser_buff.angle_increment;

		laser_buff.time_increment = laser_buff.scan_time/num_ranges;
		laser_buff.ranges.resize(num_ranges);
		laser_buff.intensities.resize(num_ranges);
		laser_buff.angle_increment = (laser_buff.angle_max-laser_buff.angle_min)/num_ranges;


		while(!pub_flag){
			unsigned char buf_read[10],step_count[4],distance[4];
			int n = read( fd, &buf_read , sizeof  buf_read);
			// std::cout<<n<<std::endl;
			if(n>0){
				if(buf_read[0]=='$' && buf_read[9]=='*'){
					step_count[0] = buf_read[1];
					step_count[1] = buf_read[2];
					step_count[2] = buf_read[3];
					step_count[3] = buf_read[4];
					distance[0] = buf_read[5];
					distance[1] = buf_read[6];
					distance[2] = buf_read[7];
					distance[3] = buf_read[8];      		
					current_steps=BytesToFloat(step_count);
					// std::cout << "Current Steps: " << current_steps << std::endl;
					distance_float=BytesToFloat(distance);
					// std::cout << "Distance : " << distance_float << std::endl;
					if((current_steps-prev_steps)>=0){
						if(((int(current_steps/16)*laser_buff.angle_increment-fov/2)< (angle_del_middle/2)) && ((int(current_steps/16)*laser_buff.angle_increment-fov/2) > (-1*angle_del_middle/2))){
							laser_buff.ranges[int(current_steps/16)] = std::numeric_limits<float>::infinity();;
						}
						else{
							laser_buff.ranges[int(current_steps/16)] = distance_float*0.01;
							prev_steps = current_steps;
						}
						
					}
					else{
						prev_steps = current_steps;
						pub_flag=1;
						// std::cout << "Flag : " << pub_flag << std::endl;
					}
				}

			}
		 }

	      	if(pub_flag==1){
	      		laser_buff.header.stamp = ros::Time::now();

	      		usb_laser_pub.publish(laser_buff);
	      		pub_flag=0;
	      	}
	        ros::spinOnce();
	        loop_rate.sleep();
    }
    return 0;
}

//TESTING


// int main(int argc, char** argv){
//   ros::init(argc, argv, "laser_scan_publisher");

//   ros::NodeHandle n;
//   ros::Publisher scan_pub = n.advertise<sensor_msgs::LaserScan>("scan", 50);

//   unsigned int num_readings = 100;
//   double laser_frequency = 40;
//   double ranges[num_readings];
//   double intensities[num_readings];

//   int count = 0;
//   ros::Rate r(1.0);
//   while(n.ok()){
//     //generate some fake data for our laser scan
//     for(unsigned int i = 0; i < num_readings; ++i){
//       ranges[i] = count;
//       intensities[i] = 100 + count;
//     }
//     ros::Time scan_time = ros::Time::now();

//     //populate the LaserScan message
//     sensor_msgs::LaserScan scan;
//     scan.header.stamp = scan_time;
//     scan.header.frame_id = "laser_frame";
//     scan.angle_min = -1.57;
//     scan.angle_max = 1.57;
//     scan.angle_increment = 3.14 / num_readings;
//     scan.time_increment = (1 / laser_frequency) / (num_readings);
//     scan.range_min = 0.0;
//     scan.range_max = 100.0;

//     scan.ranges.resize(num_readings);
//     scan.intensities.resize(num_readings);
//     for(unsigned int i = 0; i < num_readings; ++i){
//       scan.ranges[i] = ranges[i];
//       scan.intensities[i] = 0;
//     }

//     scan_pub.publish(scan);
//     ++count;
//     r.sleep();
//   }
// }
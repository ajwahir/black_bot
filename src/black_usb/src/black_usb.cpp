	#include <ros/ros.h>
	#include <boost/lexical_cast.hpp>
	#include <iostream>
	#include <stdio.h>
	#include <geometry_msgs/Twist.h>
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
	double x,w;
	std::string x_str ,y_str;
	void twist_callback(const geometry_msgs::Twist::ConstPtr& msg)
	{
	    x = msg->linear.x;
	    w = msg->angular.z;
	    // w=w*0.25;
	    // if(w>0.25)
	    // 	w=0.25;
	    // else if(w<-0.25)
	    // 	w=-0.25;
	}

	int configure_port(int fd)      // configure the port
	{
		struct termios port_settings;      // structure to store the port settings in

		cfsetispeed(&port_settings, B57600);    // set baud rates
		cfsetospeed(&port_settings, B57600);

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
	    ros::init(argc, argv, "black_usb");
	    ROS_INFO("Black serial driver is running");
	    ros::NodeHandle nh("~");
	    signal(SIGINT, mySigintHandler);

	    ros::Subscriber twist_sub = nh.subscribe("/cmd_vel", 100,twist_callback);
	    ros::Publisher usb_twist_pub = nh.advertise<geometry_msgs::Twist>("feedback_twist", 100);


	    ros::Rate loop_rate(10);
	    std::ofstream us;
		char *portname = "/dev/ttyACM8";
		int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    	// set_interface_attribs(fd, B115200);
   		configure_port(fd);
    	float linear_vel=0,omega=0,left_vel,right_vel;
    	float r=0.15,l=0.55;

    	geometry_msgs::Twist twist_feedback;

    	while (ros::ok())
    	{

    	unsigned char buf[4],buf1[4];

    	float phi1_dot = x+(l*w)/r;
    	float phi2_dot = x-(l*w)/r;

    	if(phi2_dot>0.5)
    		phi2_dot=0.5;
    	if(phi2_dot<-0.5)
    		phi2_dot=-0.5;

    	if(phi1_dot>0.5)
    		phi1_dot=0.5;
    	if(phi1_dot<-0.5)
    		phi1_dot=-0.5;

    	FloatToHex(phi2_dot,buf);
    	FloatToHex(phi1_dot,buf1);

    	// std::cout<<"phi1 : "<<phi1_dot<<" , "<<phi2_dot<<std::endl;
	
    	write(fd,"$",1);
    	write(fd,&buf,4);
    	write(fd,&buf1,4);
    	write(fd,"*",1);

//////////////////////////////////////////////////////////////////////

      //FOR READING FEEDBACK
      unsigned char buf_read[10],left[4],right[4];
      int n = read( fd, &buf_read , sizeof  buf_read);
      // std::cout<<n<<std::endl;
      if(n>0){
      	if(buf_read[0]=='$' && buf_read[9]=='*'){
      		left[0] = buf_read[1];
      		left[1] = buf_read[2];
      		left[2] = buf_read[3];
      		left[3] = buf_read[4];
  			right[0] = buf_read[5];
      		right[1] = buf_read[6];
      		right[2] = buf_read[7];
      		right[3] = buf_read[8];

      		left_vel = BytesToFloat(left);
      		right_vel = BytesToFloat(right);      		
      		linear_vel=((left_vel+right_vel)*0.5*r*31.4/980);
      		// std::cout << "Steering: " << kk << std::endl;
      		omega=((right_vel-left_vel)*0.5*r*31.4/980/l);
      		// std::cout << "velocity : " << kk << std::endl;
      		

      	}

      }

    
      	twist_feedback.linear.x=linear_vel;
  		twist_feedback.linear.y=0;
  		twist_feedback.linear.z=0;
  		twist_feedback.angular.x=0;
  		twist_feedback.angular.y=0;
  		twist_feedback.angular.z=omega;
  		// twist_feedback.angular.z=0;


  		usb_twist_pub.publish(twist_feedback);

  		///////////////////////////////////////////////////////////////

        ros::spinOnce();
        loop_rate.sleep();


    }
    return 0;
}

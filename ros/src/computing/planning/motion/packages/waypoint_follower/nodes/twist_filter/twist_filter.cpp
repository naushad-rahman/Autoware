/*
 *  Copyright (c) 2015, Nagoya University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Autoware nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ros/ros.h>
#include <geometry_msgs/TwistStamped.h>

#include <iostream>
#include <chrono>

#define LOOP_RATE 100
#define FILTER_SIZE 3


//Publisher
static ros::Publisher g_twist_pub;


void TwistCmdCallback(const geometry_msgs::TwistStampedConstPtr &msg)
{
  geometry_msgs::TwistStamped twist;

  static std::list<geometry_msgs::Twist> twist_list;
  twist_list.push_back(msg->twist);

  if(twist_list.size() != FILTER_SIZE)
    return;
  else{
    for(std::list<geometry_msgs::Twist>::const_iterator it = twist_list.begin(); it != twist_list.end();it++){
      twist.twist.linear.x += it->linear.x;
      twist.twist.angular.z += it->angular.z;
    }
    twist.twist.linear.x /= FILTER_SIZE;
    twist.twist.angular.z /= FILTER_SIZE;

    twist.header = msg->header;
    g_twist_pub.publish(twist);

    twist_list.pop_front();
  }

}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "twist_filter");

    ros::NodeHandle nh;
    ros::NodeHandle private_nh("~");
    ros::Subscriber twist_sub = nh.subscribe("twist_raw", 1, TwistCmdCallback);
    g_twist_pub = nh.advertise<geometry_msgs::TwistStamped>("twist_cmd", 1000);

    ros::spin();



    return 0;
}
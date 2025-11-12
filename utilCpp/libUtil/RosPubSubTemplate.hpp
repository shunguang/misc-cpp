/*
 *------------------------------------------------------------------------
 * abc.h
 *
 * No  any Corporation or client funds were used to develop this code.
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * Copyright (c) 2022 Shunguang Wu
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *-------------------------------------------------------------------------
 */

/// Node to receive autopilot data
//a pub-sub template with call back function

#include "std_msgs/String.h"
#include <app/concurrency/DataDeque.hpp>
#include <gt_uav_msgs/Attitude.h>
#include <gt_uav_msgs/SimpleUTMPose.h>
#include <ros/ros.h>

#include <memory>

class RosPubSubTemplate : public app::Autopilot {
public:
	RosPubSubTemplate(ros::NodeHandle &node)
      : app::Autopilot(app::Config()) {
    utm_sub_ = node.subscribe<gt_uav_msgs::SimpleUTMPose>(
        "/poseOut", 10,
        std::bind(&RosPubSubTemplate::callback, this, std::placeholders::_1));

    atti_sub_ = node.subscribe<gt_uav_msgs::Attitude>(
        "/Attitude", 10,
        std::bind(&RosPubSubTemplate::attitude_callback, this,
                  std::placeholders::_1));
  }

  virtual void connect() {}

  virtual void disconnect() {}

  void callback(const gt_uav_msgs::SimpleUTMPose::ConstPtr msg) {
    app::AutopilotState state;
    state.timestamp = msg->rostime * 1000;
    state.attitude = attitude_.getStateNear(state.timestamp);
    state.gps_tag.lat = msg->lat;
    state.gps_tag.lon = msg->lon;

    state.gps_tag.relative_alt = msg->haglalt;
    state.gps_tag.alt = msg->baroalt;
    state.est_agl_m = state.gps_tag.relative_alt;
    state.gps_tag.timestamp = state.timestamp;

    // jreid guesses at other components
    state.gps_tag.velocity = {msg->vx, msg->vy, msg->vz};

    //std::cout << std::fixed << std::setprecision(6) << "AP " << msg->lat << " " << msg->lon << " " << state.gps_tag.relative_alt << std::endl;

    state_src_.broadcast(state);
  }

  void attitude_callback(const gt_uav_msgs::Attitude::ConstPtr msg) {
    app::Attitude attitude;
    attitude.timestamp = msg->rostime * 1000;
    attitude.roll = msg->roll;
    attitude.pitch = msg->pitch;
    attitude.yaw = msg->yaw;
    attitude.roll_rate = msg->rollspeed;
    attitude.pitch_rate = msg->pitchspeed;
    attitude.yaw_rate = msg->yawspeed;
    attitude_.addNewState(attitude);
  }

  virtual bool startMission() {}
  virtual bool resumeMission() {}
  virtual void stopMission() {}
  virtual void setServo(int servo_num, float percentage) {}

  virtual void startExternalControl() {}

  virtual void
  setTranslateGoal(cv::Point3f goal_meters,
                   const app::DescentInfo &descent_info) {}

  virtual void
  updateTargetFollow(const app::ObjectPose &target,
                     const app::PlatformInfo &platform,
                     app::YawInfo desired_yaw,
                     const app::DescentInfo &descent_info) {}

private:
  ros::Subscriber utm_sub_;
  ros::Subscriber atti_sub_;

  app::DataDeque<app::Attitude> attitude_;
};

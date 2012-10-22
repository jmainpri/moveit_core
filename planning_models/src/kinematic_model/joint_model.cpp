/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Ioan Sucan, E. Gil Jones */

#include <moveit/planning_models/kinematic_model.h>

planning_models::KinematicModel::JointModel::JointModel(const std::string& name) :
  name_(name), type_(UNKNOWN), max_velocity_(0.0), parent_link_model_(NULL), child_link_model_(NULL),
  mimic_(NULL), mimic_factor_(1.0), mimic_offset_(0.0), distance_factor_(1.0), tree_index_(-1)
{
}

planning_models::KinematicModel::JointModel::~JointModel(void)
{
}

bool planning_models::KinematicModel::JointModel::getVariableBounds(const std::string& variable, std::pair<double, double>& bounds) const
{
  std::map<std::string, unsigned int>::const_iterator it = variable_index_.find(variable);
  if (it == variable_index_.end())
  {
    logWarn("Could not find variable '%s' to get bounds for within joint '%s'", variable.c_str(), name_.c_str());
    return false;
  }
  bounds = variable_bounds_[it->second];
  return true;
}

void planning_models::KinematicModel::JointModel::getDefaultValues(std::map<std::string, double> &values, const Bounds &bounds) const
{
  std::vector<double> defv;
  defv.reserve(variable_names_.size());
  getDefaultValues(defv, bounds);
  for (std::size_t i = 0 ; i < variable_names_.size() ; ++i)
    values[variable_names_[i]] = defv[i];
}

void planning_models::KinematicModel::JointModel::getRandomValues(random_numbers::RandomNumberGenerator &rng, std::map<std::string, double> &values, const Bounds &bounds) const
{
  std::vector<double> rv;
  rv.reserve(variable_names_.size());
  getRandomValues(rng, rv, bounds);
  for (std::size_t i = 0 ; i < variable_names_.size() ; ++i)
    values[variable_names_[i]] = rv[i];
}

std::vector<moveit_msgs::JointLimits> planning_models::KinematicModel::JointModel::getVariableLimits(void) const 
{
  std::vector<moveit_msgs::JointLimits> ret_vec;
  for (std::size_t i = 0; i < variable_names_.size(); ++i)
  {
    moveit_msgs::JointLimits lim;
    lim.joint_name = variable_names_[i];
    lim.has_position_limits = true;
    lim.min_position = variable_bounds_[i].first;
    lim.max_position = variable_bounds_[i].second;
    if (max_velocity_ != 0.0)
      lim.has_velocity_limits = true;
    else
      lim.has_velocity_limits = false;
    lim.max_velocity = max_velocity_;
    lim.has_acceleration_limits = false;
    lim.max_acceleration = 0.0;
    ret_vec.push_back(lim);
  }
  return ret_vec;
}

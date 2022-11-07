/*
 * @Author: ding.yin
 * @Date: 2022-11-07 14:30:16
 * @Last Modified by: ding.yin
 * @Last Modified time: 2022-11-07 15:43:16
 */
#ifndef _BACK_END_H_
#define _BACK_END_H_

#include <deque>
#include <fstream>
#include <string>

#include "sensor_data/cloud_data.hpp"
#include "sensor_data/key_frame.hpp"
#include "sensor_data/loop_pose.hpp"
#include "sensor_data/pose_data.hpp"

#include "models/optimizer/optimizer_interface.hpp"

namespace avp_mapping {
class BackEnd {
public:
  BackEnd();

  bool update(const CloudData &cloud_data, const PoseData &vidar_odom,
              const PoseData &reliable_pose);

  bool insertLoopPose(const LoopPose &loop_pose);

  bool forceOptimize();

  bool getOptimizedKFs(std::deque<KeyFrame> &key_frames_deque);

  bool hasNewKeyFrame();

  bool hasNewOptimized();

  bool getLatesetKeyFrame(KeyFrame &kf);

  bool getLatesetKeyReliableOdom(KeyFrame &kf);

private:
  bool initWithConfig();

  bool initParam(const YAML::Node &config_node);

  bool initGraphOptimizer(const YAML::Node &config_node);

  bool initDataPath(const YAML::Node &config_node);

  void resetParam();

  bool savePose(std::ofstream &ofs, const Eigen::Matrix4f &pose);

  bool addNodeAndEdge(const PoseData &reliable_odom);

  bool maybeNewKeyFrame(const CloudData &cloud_data, const PoseData &vidar_odom,
                        const PoseData &reliable_pose);

  bool maybeOptimized();

  bool saveOptimizedPose();

private:
  std::string key_frame_path_ = "";
  std::string traj_path_ = "";

  std::ofstream ground_truth_ofs_;
  std::ofstream vidar_odom_ofs_;
  std::ofstream optimized_pose_ofs_;

  float key_frame_dist_ = 2.0;

  bool has_new_key_frame_ = false;
  bool has_new_optimized_ = false;

  KeyFrame current_key_frame_;
  KeyFrame current_key_reliable_odom_;

  std::deque<KeyFrame> key_frames_deque_;
  std::deque<Eigen::Matrix4f> optimized_pose_;

  std::shared_ptr<OptimizerInterface> optimizer_ptr_;
  
  class GraphOptimizerConfig {
  public:
    GraphOptimizerConfig() {
      vidar_odom_edge_noise.resize(6);
      close_loop_noise.resize(6);
      reliable_odom_noise.resize(3);
    }

  public:
    bool use_gnss = true;
    bool use_loop_close = false;

    Eigen::VectorXd vidar_odom_edge_noise;
    Eigen::VectorXd close_loop_noise;
    Eigen::VectorXd reliable_odom_noise;

    int optimize_step_with_key_frame = 100;
    int optimize_step_with_gnss = 100;
    int optimize_step_with_loop = 10;
  };
  GraphOptimizerConfig graph_optimizer_config_;

  int loop_pose_cnt_ = 0;
  int key_frame_cnt_ = 0;
  int reliable_odom_cnt_ = 0;
};

} // namespace avp_mapping

#endif // _BACK_END_H_
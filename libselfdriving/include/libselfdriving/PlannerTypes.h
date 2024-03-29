/* -------------------------------------------------------------------------
 *   SelfDriving C++ library based on PTGs and mrpt-nav
 * Copyright (C) 2019 Jose Luis Blanco, University of Almeria
 * See LICENSE for license information.
 * ------------------------------------------------------------------------- */

#pragma once

#include <mrpt/maps/CSimplePointsMap.h>
#include <mrpt/math/TPolygon2D.h>
#include <mrpt/math/TPose2D.h>
#include <mrpt/math/TTwist2D.h>
#include <mrpt/nav/tpspace/CParameterizedTrajectoryGenerator.h>
#include <memory>
#include <vector>

namespace selfdrive
{
using ptg_t = mrpt::nav::CParameterizedTrajectoryGenerator;

struct RobotShape
{
    mrpt::math::TPolygon2D robot_shape;  //!< 2D robot shape
    double                 robot_radius{-1.0};  //!< Radius of circ. robot
};

struct SE2_KinState
{
    mrpt::math::TPose2D  pose{0, 0, 0};  //!< global pose
    mrpt::math::TTwist2D vel{0, 0, 0};  //!< global velocity

    std::string asString() const
    {
        return std::string("p=") + pose.asString() + std::string(" v=") +
               vel.asString();
    }
};

class ObstacleSource
{
   public:
    using Ptr = std::shared_ptr<ObstacleSource>;

    ObstacleSource() = default;
    ObstacleSource(const mrpt::maps::CSimplePointsMap& staticObstacles);
    virtual ~ObstacleSource();

    virtual mrpt::maps::CSimplePointsMap::Ptr obstacles(
        mrpt::system::TTimeStamp t = mrpt::system::TTimeStamp());

    virtual bool dynamic() const { return false; }

   private:
    mrpt::maps::CSimplePointsMap::Ptr static_obs_{};
};

struct TrajectoriesAndRobotShape
{
    void initFromConfigFile(
        mrpt::config::CConfigFileBase& cfg, const std::string& section);

    std::vector<std::shared_ptr<ptg_t>> ptgs;  //!< Allowed movement sets

    /** The robot 2D shape model. Only one of `robot_shape` or
     * `robot_shape_circular_radius` will be used in each PTG */
    mrpt::math::CPolygon robotShape;

    /** Radius of the robot if approximated as a circle. Only one of
     * `robot_shape` or `robot_shape_circular_radius` will be used in each PTG
     */
    double robotShapeCircularRadius{.0};
};

struct PlannerInput
{
    RobotShape                robot_shape;
    SE2_KinState              state_start, state_goal;
    ObstacleSource::Ptr       obstacles;
    double                    min_step_len{0.25};  //!< [meters]
    TrajectoriesAndRobotShape ptgs;
};

struct NavPlanAction
{
    SE2_KinState state_from, state_to;
    double       estimated_exec_time{.0};
    int          ptg_index{-1}, ptg_path_index{-1};
    double       ptg_path_alpha{-1.0}, ptg_to_d{-1.0};
    double       ptg_speed_scale{1.0};

    mrpt::nav::CParameterizedTrajectoryGenerator::TNavDynamicState
        getPTGDynState() const;
};

struct NavPlan
{
    PlannerInput               original_input;
    bool                       success{false};
    std::vector<NavPlanAction> actions;
};

}  // namespace selfdrive

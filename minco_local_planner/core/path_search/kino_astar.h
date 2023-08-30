/**
 * @Author: Yunkai Xia
 * @Date:   2023-08-30 14:18:30
 * @Last Modified by:   Yunkai Xia
 * @Last Modified time: 2023-08-30 17:46:53
 */
#include <stdint.h>

#ifndef __KINO_ASTAR_H__
#define __KINO_ASTAR_H__
#include "path_search.h"
namespace minco_local_planner::path_search {
using namespace path_search;
using namespace basis;
class KinoAstar : public PathSearch {
 public:
  KinoAstar();

  virtual int Search(const VehiclePose& start_pos, const Vec2d& init_ctrl,
                     const VehiclePose& end_pos) override;

  virtual void Reset() override;

 private:
  Points2d car_vertex_;
};
}  // namespace minco_local_planner::path_search

#endif /* __KINO_ASTAR_H__ */
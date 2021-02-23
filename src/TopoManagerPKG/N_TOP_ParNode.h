//-------------------------------------------------------------------------
//   Copyright 2002-2021 National Technology & Engineering Solutions of
//   Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
//   NTESS, the U.S. Government retains certain rights in this software.
//
//   This file is part of the Xyce(TM) Parallel Electrical Simulator.
//
//   Xyce(TM) is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   Xyce(TM) is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with Xyce(TM).
//   If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Purpose        :
//
// Special Notes  :
//
// Creator        : Rob Hoekstra
//
// Creation Date  : 7/17/01
//
//
//
//
//-----------------------------------------------------------------------------

#ifndef  _N_TOP_PARNODE_H
#define  _N_TOP_PARNODE_H

#include <string>
#include <set>
#include <iosfwd>

#include <N_TOP_Node.h>
#include <N_UTL_Pack.h>

namespace Xyce {
namespace Topo {

//-----------------------------------------------------------------------------
// Class         : ParNode
// Purpose       :
// Special Notes :
// Creator       : Rob Hoekstra, SNL, Parallel Computational Sciences
// Creation Date : 7/17/01
//-----------------------------------------------------------------------------
class ParNode : public Node
{
  friend class Pack<ParNode>;

public:

  // Constructor
  ParNode( const NodeID & nid = NodeID("", -1),
                 const bool & owned = true,
                 const int & proc = 0)
  : Node(nid, owned),
    proc_(proc)
  {}

  // Constructor
  ParNode(const Node & node, const int & proc = 0)
  : Node(node),
    proc_(proc)
  {}

  // Destructor
  ~ParNode() { }

  // Simple copy construction, autogenerated by compiler.
  bool operator==(const ParNode & right) const
  { return (nodeID_ == right.nodeID_); }

  // Non-equality operator.
  bool operator!=(const ParNode & right) const
  { return (nodeID_ != right.nodeID_); }

  const int & proc() const { return proc_; }

protected:
  int proc_;

  std::ostream & put(std::ostream & os) const;
  friend std::ostream & operator<<(std::ostream & os, const ParNode & pn);

};

} // namespace Topo
} // namespace Xyce

#endif

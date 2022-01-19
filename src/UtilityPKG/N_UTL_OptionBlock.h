//-------------------------------------------------------------------------
//   Copyright 2002-2022 National Technology & Engineering Solutions of
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
// Creator        : Rob Hoekstra, SNL, Parallel Computational Sciences
//
// Creation Date  : 05/15/01
//
//
//
//
//-----------------------------------------------------------------------------

#ifndef Xyce_N_UTL_OptionBlock_h
#define Xyce_N_UTL_OptionBlock_h

#include <string>
#include <list>
#include <iosfwd>

#include <N_UTL_fwd.h>
#include <N_UTL_Pack.h>
#include <N_UTL_Param.h>
#include <N_UTL_NetlistLocation.h>

namespace Xyce {
namespace Util {

//-----------------------------------------------------------------------------
// Class         : OptionBlock
// Purpose       :
// Special Notes :
// Creator       : Rob Hoekstra, SNL, Parallel Computational Sciences
// Creation Date : 5/15/01
//-----------------------------------------------------------------------------
class OptionBlock
{
  friend class Pack<OptionBlock>;

  public:
    enum ExpressionsAllowed {
      NO_EXPRESSIONS = false,
      ALLOW_EXPRESSIONS = true
    };

    OptionBlock(const std::string &name = "internal", ExpressionsAllowed expressions_allowed = ALLOW_EXPRESSIONS, const NetlistLocation &netlist_location = NetlistLocation())
      : name_(name),
        expressionsAllowed_(expressions_allowed),
        netlistLocation_(netlist_location),
        params_()
    {}

    OptionBlock(const std::string &name, ExpressionsAllowed expressions_allowed, const std::string &netlist_filename, int line_number)
      : name_(name),
        expressionsAllowed_(expressions_allowed),
        netlistLocation_(netlist_filename, line_number),
        params_()
    {}

    OptionBlock(const OptionBlock & right)
      : name_(right.name_),
        expressionsAllowed_(right.expressionsAllowed_),
        netlistLocation_(right.netlistLocation_),
        params_(right.params_)
    {}

    virtual ~OptionBlock()
    {}

    // // Assignment operator.
    OptionBlock & operator=(const OptionBlock & right);

    // void setExpressionsAllowed(ExpressionsAllowed expression_allowed) {
    //   expressionsAllowed_ = expression_allowed;
    // }

    //beginning of params
    ParamList::iterator begin()
    {
      return params_.begin();
    } 

    //end of params 
    ParamList::iterator end()
    {
      return params_.end();
    }

    bool getExpressionsAllowed() const {
      return expressionsAllowed_ == ALLOW_EXPRESSIONS;
    }

    ParamList::const_iterator begin() const
    {
      return params_.begin();
    }

    //end of params
    ParamList::const_iterator end() const
    {
      return params_.end();
    }

    ParamList::size_type size() const {
      return params_.size();
    }

    void addParam(const Util::Param &parameter);

    void removeParam(const std::string& tagName);

    const std::string &getName() const 
    {
      return name_;
    }

    const NetlistLocation &getNetlistLocation() const
    {
      return netlistLocation_;
    }

  private:
    std::string           name_;
    ExpressionsAllowed    expressionsAllowed_;
    NetlistLocation       netlistLocation_;
    ParamList             params_;                        ///< Must allow multiple duplicate entries
};

// compare contained lists
// the equity operator just compares the "name" field.  This
// will compare the Param lists as well.
bool compareParamLists(const OptionBlock &s0, const OptionBlock & s1);

std::ostream &operator<<(std::ostream &os, const OptionBlock &options);

} // namespace Util
} // namespace Xyce


#endif

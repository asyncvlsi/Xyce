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

//-------------------------------------------------------------------------
//
// Purpose : Declaration of the ReactionLexer class for the reaction
//           parsing package
//
// Special Notes  :
//
// Creator        : Thomas V. Russo, SNL, Electrical and Microsystems Modeling
//
// Creation Date  : 08/10/2006
//
//
//
//
//-------------------------------------------------------------------------

#ifndef N_DEV_ReactionLexer_H
#define N_DEV_ReactionLexer_H

#include <iostream>
#include <string>
#include <map>

namespace Xyce {
namespace Device {

class ReactionLexer: public yyFlexLexer
{
public:
  ReactionLexer(
    const NetlistLocation &     netlist_location,
    const std::string &         reaction_filename,
    std::istream *              input = 0,
    std::ostream *              output = 0)
    : yyFlexLexer(input,output),
      netlistLocation_(netlist_location),
      reactionFilename_(reaction_filename)
  {}

  virtual ~ReactionLexer()
  {}

  int getToken(XyceDevice::ReactionParser::semantic_type *lvalp, XyceDevice::location *llocp, std::map<std::string,int> &theSpecies);

  const NetlistLocation         netlistLocation_;
  const std::string &           reactionFilename_;
};

}
}

#endif

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
// Creator        : Eric Keiter
//
// Creation Date  : 5/26/00
//
//
//
//
//-----------------------------------------------------------------------------

#ifndef  Xyce_N_TIA_Dummy_h
#define  Xyce_N_TIA_Dummy_h

enum Vector_Tag
{
  _SOLUTION_VECTOR, // 0
  _STATE_VECTOR // 1
};

//-----------------------------------------------------------------------------
// Class         : N_TIA_TimeIntegrationAlgorithm
// Purpose       :
// Special Notes :
// Creator       : Eric Keiter, SNL, Parallel Computational Sciences
// Creation Date : 5/26/00
//-----------------------------------------------------------------------------

class N_TIA_TimeIntegrationAlgorithm
{
public:

  // Constructor
  N_TIA_TimeIntegrationAlgorithm() : dt(1.0e-6), time(0.0) { };

  // Destructor
  ~N_TIA_TimeIntegrationAlgorithm() { };

  double partialTimeDerivative(const int itag)
  {
    return (1.0 / dt);
  }

  double timeDerivative(const int index, const int itag)
  {
    return (1.0 / dt);
  }

  double getTime()
  {
    return time;
  }

public :
  int iIntegrationScheme;
  double dt;
  double time;
};

#endif // Xyce_N_TIA_Dummy_h

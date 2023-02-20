//-------------------------------------------------------------------------
//   Copyright 2002-2023 National Technology & Engineering Solutions of
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
// Purpose        : Neuron classes.
//
// Special Notes  :
//
// Creator        : Richard Schiek, SNL, Electrical and Microsystem Modeling
//
// Creation Date  : 01/02/08
//
//
//
//
//-----------------------------------------------------------------------------

#ifndef Xyce_N_DEV_Neuron2_h
#define Xyce_N_DEV_Neuron2_h

#include <Sacado_No_Kokkos.hpp>

#include <N_DEV_Configuration.h>
#include <N_DEV_DeviceBlock.h>
#include <N_DEV_DeviceInstance.h>
#include <N_DEV_DeviceModel.h>

#include <N_DEV_Neuron.h>

namespace Xyce {
namespace Device {
namespace Neuron2 {

class Model;
class Instance;

struct Traits : public DeviceTraits<Model, Instance, Neuron::Traits>
{
  static const char *name() {return "Neuron";}
  static const char *deviceTypeName() {return "YNEURON level 2";}
  static int numNodes() {return 2;}
  static bool modelRequired() {return true;}
  static bool isLinearDevice() {return false;}

  static Device *factory(const Configuration &configuration, const FactoryBlock &factory_block);
  static void loadModelParameters(ParametricData<Model> &model_parameters);
  static void loadInstanceParameters(ParametricData<Instance> &instance_parameters);
};

//-----------------------------------------------------------------------------
// Class         : Instance
// Purpose       : This is class refers to a single instance of the
//                 Neuron device.  It has two nodes associated with it, a
//                 positive and a negative node.   See the NeuronInstance
//                 class for a more detailed explanation.
// Special Notes :
// Creator       : Richard Schiek, SNL, Electrical and Microsystem Modeling
// Creation Date : 01/02/08
//-----------------------------------------------------------------------------
class Instance : public DeviceInstance
{
  friend class ParametricData<Instance>;
  friend class Model;
  friend struct Traits;
    
public:
  static std::vector< std::vector<int> > jacStamp;

  Instance(
     const Configuration &       configuration,
     const InstanceBlock &       IB,
     Model &                     Miter,
     const FactoryBlock &        factory_block);

  ~Instance();

private:
  Instance(const Instance &right);
  Instance &operator=(const Instance &);

public:
  void registerLIDs( const std::vector<int> & intLIDVecRef,
                     const std::vector<int> & extLIDVecRef );
  void registerStateLIDs( const std::vector<int> & staLIDVecRef );

  void loadNodeSymbols(Util::SymbolTable &symbol_table) const; // override

  const std::vector< std::vector<int> > & jacobianStamp() const;
  void registerJacLIDs( const std::vector< std::vector<int> > & jacLIDVec );

  bool processParams ();
  bool updateTemperature(const double & temp_tmp);

  bool updateIntermediateVars ();
  bool updatePrimaryState ();
  bool updateSecondaryState ();
  bool setIC ();

  void varTypes( std::vector<char> & varTypeVec );

  // load functions, residual:
  bool loadDAEQVector ();
  bool loadDAEFVector ();

  void auxDAECalculations ();

  // load functions, Jacobian:
  bool loadDAEdQdx ();
  bool loadDAEdFdx ();

protected:
private:
  // These functions represents the equations that need to be solved
  // for this device.  Since Xyce loads an F and Q contribution, the
  // equations are broken up into their F and Q components.  Thus there
  // is a kcl1EquF() and kcl1EquQ().  Automatic differentiation will
  // be used to generate all the derivatives of these equations for the
  // dF/dX and dQ/dX loads

  // first we list some utility functions for calculating coefficients
  //
  // These functions expect V to be in milli-volts and then return values that
  // are in 1/ms.  Thus the extra factor's of 1000 here and there

  // potassium current, functions for activator equation
  template <typename ScalarT>
  static ScalarT alphaN( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = 1000.0 * (0.02 * (vScaled + 45.7)) / (1.0 - std::exp(-0.1*(vScaled+45.7)));
    // result.  the 1000 factor is to change from 1/ms to 1/s
    return r;
  }

  template <typename ScalarT>
  static ScalarT betaN( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = 1000.0 * 0.25 * std::exp( -0.0125 * (vScaled + 55.7));
    // result.  the 1000 factor is to change from 1/ms to 1/s
    return r;
  }

  // sodium current, functions for activator equation
  template <typename ScalarT>
  static ScalarT alphaM( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = 1000.0 * (0.38 * (vScaled + 29.7)) / (1.0 - std::exp(-0.1*(vScaled+29.7)));
    // result.  the 1000 factor is to change from 1/ms to 1/s
    return r;
  }

  template <typename ScalarT>
  static ScalarT betaM( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = 1000.0 * 15.2 * std::exp( -0.0556 * (vScaled + 54.7));
    // result.  the 1000 factor is to change from 1/ms to 1/s
    return r;
  }

  template <typename ScalarT>
  static ScalarT alphaH( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = 1000.0 * 0.266 * std::exp( -0.05 * (vScaled + 48.0));
    // result.  the 1000 factor is to change from 1/ms to 1/s
    return r;
  }

  template <typename ScalarT>
  static ScalarT betaH( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = 1000.0 * 3.8 / (1.0 + std::exp(-0.1*(vScaled+18.0)));
    // result.  the 1000 factor is to change from 1/ms to 1/s
    return r;
  }

  // a-current functions
  template <typename ScalarT>
  static ScalarT aInf( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = std::pow( ((0.0761 * std::exp(0.0314 * (vScaled+94.22))) / (1.0+std::exp(0.0346*(vScaled+1.17)))), 1.0/3.0);
    return r;
  }

  template <typename ScalarT>
  static ScalarT aTau( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = (0.3632 + 1.158 / (1.0 + std::exp(0.0497 * (vScaled + 55.96)))) / 1000.0;
    // the 1000.0 factor is to change ms to s.
    return r;
  }

  template <typename ScalarT>
  static ScalarT bInf( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = std::pow( (1.0 / (1.0 + std::exp(0.0688*(vScaled+53.3)))), 4.0);
    return r;
  }

  template <typename ScalarT>
  static ScalarT bTau( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = (1.24 + 2.678 / (1.0 + std::exp(0.0624 * (vScaled + 50.0)))) / 1000.0;
    return r;
  }

  // transient calcium functions
  template <typename ScalarT>
  static ScalarT M_Inf( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = 1.0/(1.0 + std::exp(-(vScaled+57)/6.2));
    return r;
  }

  template <typename ScalarT>
  static ScalarT M_Tau( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = (0.612 + 1.0/(std::exp(-(vScaled+132)/16.7) + std::exp((vScaled+16.8)/18.2)) ) / 1000.0;
    return r;
  }

  template <typename ScalarT>
  static ScalarT H_Inf( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = 1.0 / (1.0 + std::exp((vScaled+81)/4.0));
    return r;
  }

  template <typename ScalarT>
  static ScalarT H_Tau( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r;
    if( vScaled < -80.0 )
    {
      r = std::exp( (vScaled + 467)/66.6 ) / 1000.0;
    }
    else
    {
      r = ( 28.0 + std::exp(-(vScaled+22.0)/10.5)) / 1000.0;
    }
    return r;
  }

  // Calcium dependent Potassium conductances
  template <typename ScalarT>
  static ScalarT C_Inf( const ScalarT Vin, const ScalarT CaConc)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = (CaConc / (CaConc + 3.0)) * (1.0 / (1.0 + std::exp(-(vScaled+28.3)/12.6 )));
    return r;
  }

  template <typename ScalarT>
  static ScalarT C_Tau( const ScalarT Vin)
  {
    ScalarT vScaled = 1000.0 * Vin;  // convert voltage to milli-volts
    ScalarT r = (90.3 - 75.1/(1.0 + std::exp(-(vScaled+46)/22.7))) / 1000.0;
    return r;
  }


  // now the device equations
  // KCL equation 1
  template <typename ScalarT>
  static ScalarT kcl1EquF( const ScalarT& Vn1, const ScalarT& Vn2, const ScalarT& n, const ScalarT& m, const ScalarT& h,
                           const ScalarT& a, const ScalarT& b, const ScalarT& MC, const ScalarT& HC, const ScalarT& CC,
                           const ScalarT& memG, const ScalarT& restV, const ScalarT& Kg, const ScalarT& Ke, const ScalarT& NaG, const ScalarT& NaE,
                           const ScalarT& Ag, const ScalarT& Ae, const ScalarT& CaTg, const ScalarT& CaE, const ScalarT& KCaG)
  {
    ScalarT powN = n * n * n * n;
    ScalarT powM = m * m * m;
    ScalarT powA = a * a * a;
    ScalarT powMC = MC * MC;
    ScalarT powCC = CC * CC * CC * CC;
    ScalarT r = memG * (Vn1 - Vn2 - restV) + Kg * powN * (Vn1 - Vn2 - Ke ) + NaG * powM * h * (Vn1 - Vn2 - NaE )
      + Ag * powA * b * (Vn1 - Vn2 - Ae) + CaTg * powMC * HC * (Vn1 - Vn2 - CaE) + KCaG * powCC * (Vn1 - Vn2 - Ke);
    return r;
  }

  template <typename ScalarT>
  static ScalarT kcl1EquQ( const ScalarT& Vn1, const ScalarT& Vn2, const ScalarT& memC )
  {
    ScalarT r = memC * (Vn1 - Vn2);
    return r;
  }

  // KCL equation 2 -- -1 * equation 1 because of device symmetry
  template <typename ScalarT>
  static ScalarT kcl2EquF( const ScalarT& Vn1, const ScalarT& Vn2, const ScalarT& n, const ScalarT& m, const ScalarT& h,
                           const ScalarT& a, const ScalarT& b, const ScalarT& MC, const ScalarT& HC, const ScalarT& CC,
                           const ScalarT& memG, const ScalarT& restV, const ScalarT& Kg, const ScalarT& Ke, const ScalarT& NaG, const ScalarT& NaE,
                           const ScalarT& Ag, const ScalarT& Ae, const ScalarT& CaTg, const ScalarT& CaE, const ScalarT& KCaG)
  {
    ScalarT powN = n * n * n * n;
    ScalarT powM = m * m * m;
    ScalarT powA = a * a * a;
    ScalarT powMC = MC * MC;
    ScalarT powCC = CC * CC * CC * CC;
    ScalarT r = -1.0 * (memG * (Vn1 - Vn2 - restV) + Kg * powN * (Vn1 - Vn2 - Ke ) + NaG * powM * h * (Vn1 - Vn2 - NaE )
                        + Ag * powA * b * (Vn1 - Vn2 - Ae) + CaTg * powMC * HC * (Vn1 - Vn2 - CaE) + KCaG * powCC * (Vn1 - Vn2 - Ke) );
    return r;
  }

  template <typename ScalarT>
  static ScalarT kcl2EquQ( const ScalarT& Vn1, const ScalarT& Vn2, const ScalarT& memC )
  {
    ScalarT r = -1.0 * memC * (Vn1 - Vn2);
    return r;
  }

  // n conservation equation
  template <typename ScalarT>
  static ScalarT nEquF( const ScalarT& Vn1, const ScalarT& n, const ScalarT& Vrest )
  {
    ScalarT vDiff = Vn1; // - Vrest
    ScalarT alpha = alphaN<ScalarT>(vDiff);
    ScalarT beta = betaN<ScalarT>(vDiff);
    ScalarT r = (alpha + beta) * n - alpha;
    return r;
  }

  template <typename ScalarT>
  static ScalarT nEquQ( const ScalarT& n )
  {
    ScalarT r = n;
    return r;
  }

  // m conservation equation
  template <typename ScalarT>
  static ScalarT mEquF( const ScalarT& Vn1, const ScalarT& m, const ScalarT& Vrest )
  {
    ScalarT vDiff = Vn1; // - Vrest
    ScalarT alpha = alphaM<ScalarT>(vDiff);
    ScalarT beta = betaM<ScalarT>(vDiff);
    ScalarT r = (alpha + beta) * m - alpha;
    return r;
  }

  template <typename ScalarT>
  static ScalarT mEquQ( const ScalarT& m )
  {
    ScalarT r = m;
    return r;
  }

  // h conservation equation
  template <typename ScalarT>
  static ScalarT hEquF( const ScalarT& Vn1, const ScalarT& h, const ScalarT& Vrest )
  {
    ScalarT vDiff = Vn1; // - Vrest
    ScalarT alpha = alphaH<ScalarT>(vDiff);
    ScalarT beta = betaH<ScalarT>(vDiff);
    ScalarT r = (alpha + beta) * h - alpha;
    return r;
  }

  template <typename ScalarT>
  static ScalarT hEquQ( const ScalarT& h )
  {
    ScalarT r = h;
    return r;
  }

  // a conservation equation
  template <typename ScalarT>
  static ScalarT aEquF( const ScalarT& Vn1, const ScalarT& a, const ScalarT& Vrest )
  {
    ScalarT vDiff = Vn1; // - Vrest
    ScalarT Inf = aInf<ScalarT>(vDiff);
    ScalarT Tau = aTau<ScalarT>(vDiff);
    ScalarT r = (a - Inf)/Tau;
    return r;
  }

  template <typename ScalarT>
  static ScalarT aEquQ( const ScalarT& a )
  {
    ScalarT r = a;
    return r;
  }

  // b conservation equation
  template <typename ScalarT>
  static ScalarT bEquF( const ScalarT& Vn1, const ScalarT& b, const ScalarT& Vrest )
  {
    ScalarT vDiff = Vn1; // - Vrest
    ScalarT Inf = bInf<ScalarT>(vDiff);
    ScalarT Tau = bTau<ScalarT>(vDiff);
    ScalarT r = (b - Inf)/Tau;
    return r;
  }

  template <typename ScalarT>
  static ScalarT bEquQ( const ScalarT& b )
  {
    ScalarT r = b;
    return r;
  }

  // M conservation equation
  template <typename ScalarT>
  static ScalarT M_EquF( const ScalarT& Vn1, const ScalarT& M, const ScalarT& Vrest )
  {
    ScalarT vDiff = Vn1; // - Vrest
    ScalarT Inf = M_Inf<ScalarT>(vDiff);
    ScalarT Tau = M_Tau<ScalarT>(vDiff);
    ScalarT r = (M - Inf)/Tau;
    return r;
  }

  template <typename ScalarT>
  static ScalarT M_EquQ( const ScalarT& M )
  {
    ScalarT r = M;
    return r;
  }

  // H conservation equation
  template <typename ScalarT>
  static ScalarT H_EquF( const ScalarT& Vn1, const ScalarT& H, const ScalarT& Vrest )
  {
    ScalarT vDiff = Vn1; // - Vrest
    ScalarT Inf = H_Inf<ScalarT>(vDiff);
    ScalarT Tau = H_Tau<ScalarT>(vDiff);
    ScalarT r = (H - Inf)/Tau;
    return r;
  }

  template <typename ScalarT>
  static ScalarT H_EquQ( const ScalarT& H )
  {
    ScalarT r = H;
    return r;
  }

  // C conservation equation
  template <typename ScalarT>
  static ScalarT C_EquF( const ScalarT& Vn1, const ScalarT& C, const ScalarT& CaConc, const ScalarT& Vrest )
  {
    ScalarT vDiff = Vn1; // - Vrest
    ScalarT Inf = C_Inf<ScalarT>(vDiff, CaConc);
    ScalarT Tau = C_Tau<ScalarT>(vDiff);
    ScalarT r = (C - Inf)/Tau;
    return r;
  }

  template <typename ScalarT>
  static ScalarT C_EquQ( const ScalarT& C )
  {
    ScalarT r = C;
    return r;
  }

  // Calcium conservation equation
  template <typename ScalarT>
  static ScalarT Ca_EquF( const ScalarT& Vn1, const ScalarT& Vn2, const ScalarT& MC, const ScalarT& HC, const ScalarT& Ca,
                          const ScalarT& CaTg, const ScalarT& CaE, const ScalarT& CaGamma, const ScalarT& CaTau )
  {
    ScalarT r = CaGamma * CaTg * MC * MC * HC * (Vn1 - Vn2 - CaE) + Ca / CaTau;
    return r;
  }

  template <typename ScalarT>
  static ScalarT Ca_EquQ( const ScalarT& Ca)
  {
    ScalarT r = Ca;
    return r;
  }

public:
  // iterator reference to the Neuron model which owns this instance.
  // Getters and setters
  Model &getModel() 
  {
    return model_;
  }

private:

  Model &       model_;         //< Owning model

  // derrived quantities computed in updateIntermediateVars
  // and used in the load functions
  double kcl1Fvalue, kcl1Qvalue;
  double kcl2Fvalue, kcl2Qvalue;
  double nEquFvalue, nEquQvalue;
  double mEquFvalue, mEquQvalue;
  double hEquFvalue, hEquQvalue;
  double aEquFvalue, aEquQvalue;
  double bEquFvalue, bEquQvalue;
  double M_EquFvalue, M_EquQvalue;
  double H_EquFvalue, H_EquQvalue;
  double cEquFvalue, cEquQvalue;
  double CaEquFvalue, CaEquQvalue;

  double dkcl1F_dV1, dkcl1F_dV2, dkcl1F_dn, dkcl1F_dm, dkcl1F_dh,
    dkcl1F_da, dkcl1F_db, dkcl1F_dM, dkcl1F_dH, dkcl1F_dc, dkcl1Q_dV1, dkcl1Q_dV2;
  double dkcl2F_dV1, dkcl2F_dV2, dkcl2F_dn, dkcl2F_dm, dkcl2F_dh,
    dkcl2F_da, dkcl2F_db, dkcl2F_dM, dkcl2F_dH, dkcl2F_dc, dkcl2Q_dV1, dkcl2Q_dV2;
  double dnF_dV1, dnF_dn, dnQ_dn;
  double dmF_dV1, dmF_dm, dmQ_dm;
  double dhF_dV1, dhF_dh, dhQ_dh;
  double daF_dV1, daF_da, daQ_da;
  double dbF_dV1, dbF_db, dbQ_db;
  double dMF_dV1, dMF_dM, dMQ_dM;
  double dHF_dV1, dHF_dH, dHQ_dH;
  double dcF_dV1, dcF_dc, dcF_dCa, dcQ_dc;
  double dCaF_dV1, dCaF_dV2, dCaF_dM, dCaF_dH, dCaF_dCa, dCaQ_dCa;

  // state variables
  double potassiumCurrent;
  double sodiumCurrent;

  // local state indices (offsets)
  int li_KCurrentState;
  int li_NaCurrentState;

  // local solution indices (offsets)
  int li_Pos;      // local index to positive node on this device
  int li_Neg;      // local index to negative node on this device
  int li_nPro;     // local index to n promoter value (Na current)
  int li_mPro;     // local index to m promoter value (K current)
  int li_hPro;     // local index to h promoter value (K current)
  int li_aPro;     // local index to
  int li_bPro;     // local index
  int li_M_Pro;    // local index
  int li_H_Pro;    // local index
  int li_cPro;     // local index
  int li_CaPro;    // local index

  // Matrix equation index variables:

  // Offset variables corresponding to the above declared indices.
  int APosEquPosNodeOffset;
  int APosEquNegNodeOffset;
  int APosEquNNodeOffset;
  int APosEquMNodeOffset;
  int APosEquHNodeOffset;
  int APosEquANodeOffset;
  int APosEquBNodeOffset;
  int APosEquM_NodeOffset;
  int APosEquH_NodeOffset;
  int APosEquCNodeOffset;

  int ANegEquPosNodeOffset;
  int ANegEquNegNodeOffset;
  int ANegEquNNodeOffset;
  int ANegEquMNodeOffset;
  int ANegEquHNodeOffset;
  int ANegEquANodeOffset;
  int ANegEquBNodeOffset;
  int ANegEquM_NodeOffset;
  int ANegEquH_NodeOffset;
  int ANegEquCNodeOffset;

  int ANEquPosNodeOffset;
  int ANEquNNodeOffset;

  int AMEquPosNodeOffset;
  int AMEquMNodeOffset;

  int AHEquPosNodeOffset;
  int AHEquHNodeOffset;

  int AAEquPosNodeOffset;
  int AAEquANodeOffset;

  int ABEquPosNodeOffset;
  int ABEquBNodeOffset;

  int AM_EquPosNodeOffset;
  int AM_EquM_NodeOffset;

  int AH_EquPosNodeOffset;
  int AH_EquH_NodeOffset;

  int ACEquPosNodeOffset;
  int ACEquCNodeOffset;
  int ACEquCaNodeOffset;

  int ACaEquPosNodeOffset;
  int ACaEquNegNodeOffset;
  int ACaEquM_NodeOffset;
  int ACaEquH_NodeOffset;
  int ACaEquCaNodeOffset;
};

//-----------------------------------------------------------------------------
// Class         : Model
// Purpose       :
// Special Notes :
// Creator       : Richard Schiek, SNL, Electrical and Microsystem Modeling
// Creation Date : 01/02/08
//-----------------------------------------------------------------------------
class Model : public DeviceModel
{
  typedef std::vector<Instance *> InstanceVector;

  friend class ParametricData<Model>;
  friend class Instance;
  friend struct Traits;
    
public:
  Model(
     const Configuration &       configuration,
     const ModelBlock &          MB,
     const FactoryBlock &        factory_block);
  ~Model();

private:
  Model();
  Model(const Model &);
  Model &operator=(const Model &);

public:
  virtual void forEachInstance(DeviceInstanceOp &op) const /* override */;
    
  virtual std::ostream &printOutInstances(std::ostream &os) const;

  bool processParams ();
  bool processInstanceParams ();

private:

  // parameter variables
  double cMem;     // membrane capacitance
  double gMem;     // membrane conductance
  double vRest;    // resting potential
  double eNa;      // sodium rest potential
  double gNa;      // sodium base conductance
  double eK;       // potassium rest potential
  double gK;       // potassium base conductance
  double eA;       // a-current rest potential
  double gA;       // a-current base conductance
  double eCa;      // Calcium rest potential
  double gCa;      // Calcium base conductance
  double eKCa;     // potassium-calcium rest potential
  double gKCa;     // potassium-calcium base conductance
  double CaInit;  // initial intra-cellular calcium concentration
  double CaGamma;  // calcium current to concentration multiplier
  double CaTau;    // calcium removal time constant

  // flags that parameters were given
  bool cMemGiven;
  bool gMemGiven;
  bool vRestGiven;
  bool eNaGiven;
  bool gNaGiven;
  bool eKGiven;
  bool gKGiven;
  bool eAGiven;
  bool gAGiven;
  bool eCaGiven;
  bool gCaGiven;
  bool eKCaGiven;
  bool gKCaGiven;
  bool CaInitGiven;
  bool CaGammaGiven;
  bool CaTauGiven;

public:
  void addInstance(Instance *instance) 
  {
    instanceContainer.push_back(instance);
  }

  void setupBaseInstanceContainer()
  {
    std::vector<Instance*>::iterator iter = instanceContainer.begin();
    std::vector<Instance*>::iterator end   = instanceContainer.end();
    for ( ; iter!=end; ++iter)
    {
      Xyce::Device::DeviceModel::baseInstanceContainer.push_back( static_cast<Xyce::Device::DeviceInstance *>(*iter) );
    }
  }

private:
  std::vector<Instance*> instanceContainer;
};

void registerDevice(const DeviceCountMap& deviceMap, const std::set<int>& levelSet);

} // namespace Neuron2
} // namespace Device
} // namespace Xyce

#endif

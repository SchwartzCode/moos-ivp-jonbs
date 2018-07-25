/************************************************************/
/*    NAME: Jonathan Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_TempGradient.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef TempGradient_HEADER
#define TempGradient_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_TempGradient : public IvPBehavior {
public:
  BHV_TempGradient(IvPDomain);
  ~BHV_TempGradient() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  void         handleMailReport(std::string);
  IvPFunction* onRunState();

protected: // Local Utility functions

protected: // Configuration parameters
  double m_osx;
  double m_osy;
  double m_temp_low;
  double m_temp_high;
  double m_diff;

  double m_rep_x;
  double m_rep_y;
  double m_rep_temp;

protected: // State variables
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_TempGradient(domain);}
}
#endif

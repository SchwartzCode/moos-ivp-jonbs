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
  void         handleSearchPolygon(std::string);

protected: // Local Utility functions

protected: // Configuration parameters  
  vector<double> search_region; // region that front occurs in, with a little space left for the edges

protected: // State variables
  double m_osx;
  double m_osy;
  double m_curr_wpt_index;
  int    m_init_waypoints;
  double m_temp_low;
  double m_temp_high;
  double m_diff;

  double m_rep_x;
  double m_rep_y;
  double m_rep_temp;

  class tempPoint {
    public:
      double x_coord;
      double y_coord;
      double temp;

      string get_pos() {
        string x = to_string(x_coord);
        string y = to_string(y_coord);
        string pos = x + "," + y;

        return pos;
      }

      double get_temp() {
        return temp;
      }
      double get_x() {
        return x_coord;
      }
      double get_y() {
        return y_coord;
      }
  
      tempPoint() {
        x_coord=0;
        y_coord=0;
        temp=0;
      }
      tempPoint(double x, double y, double t){
        x_coord = x;
        y_coord = y;
        temp = t;
      }
  };


  vector<tempPoint> temp_info;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_TempGradient(domain);}
}
#endif

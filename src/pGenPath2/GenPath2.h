/************************************************************/
/*    NAME: Jonathan B Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include <vector>
#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "XYSegList.h"

using namespace std;

class GenPath2 : public CMOOSApp
{
 public:
   GenPath2();
   ~GenPath2();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool postPoints();
   void organizePoints();
   void missedPoints();

 protected:
   void RegisterVariables();
 
   bool m_points_sent;
   vector<string> m_input;
   double m_points [2][50];
   bool m_point_reached[50];
   double m_osX;
   double m_osY;
   int m_capture_dist;
   bool m_done_parousing;
   XYSegList my_seglist;
   XYSegList orgPoints;

 private: // Configuration variables

 private: // State variables
};

#endif 

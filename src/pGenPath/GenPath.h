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

class GenPath : public CMOOSApp
{
 public:
   GenPath();
   ~GenPath();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool postPoints();
   void organizePoints();

 protected:
   void RegisterVariables();
 
   bool m_points_sent;
   vector<string> m_input;
   double m_points [2][50];
 private: // Configuration variables

 private: // State variables
};

#endif 

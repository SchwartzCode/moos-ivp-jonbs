/************************************************************/
/*    NAME: THE Jonathan Schwartz                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE:                                                 */
/************************************************************/
#include <string>
#include <vector>
#include <iterator>
#include <MBUtils.h>


#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPoint.h"

using namespace std;

class PointAssign : public AppCastingMOOSApp
{
 public:
   PointAssign();
   ~PointAssign();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport();
 
 protected:
   void RegisterVariables();
   vector<string> m_input;
   bool m_points_given;
   int m_vector_size;
   bool m_assign_by_region;
   vector<string> m_output_v1;
   vector<string> m_output_v2;
   bool splitDistribution();
   bool regionalDistribution();
   void postViewPoint(string input, string color);
   bool m_post_points;

  int m_good_message_count;
  int m_bad_message_count;
  int a; 

 private: // Configuration variables

 private: // State variables
};

#endif 

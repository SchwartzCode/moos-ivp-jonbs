/************************************************************/
/*    NAME: Jonathan Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef CommunicationAngle_HEADER
#define CommunicationAngle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <string>

using namespace std;

class CommunicationAngle : public CMOOSApp
{
 public:
   CommunicationAngle();
   ~CommunicationAngle();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

   string m_os_name;
   string m_collab_name;
   double m_os_x;
   double m_os_y;
   double m_os_depth;
   double m_os_heading;
   double m_os_speed;
   double m_collab_x;
   double m_collab_y;
   double m_collab_depth;
   double m_collab_heading;
   double m_collab_speed;
   bool m_collab_registered;

  //returns
  string m_acoustic_path;
  string m_connectivity_loc_ref;


 private: // Configuration variables

 private: // State variables
};

#endif 

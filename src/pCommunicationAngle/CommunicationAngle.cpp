/************************************************************/
/*    NAME: Jonathan Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle::CommunicationAngle()
{
  m_os_name = "batmobile";
  m_collab_name = "robinmobile";
  m_os_x = 0;
  m_os_y = 0;
  m_os_depth = 0;
  m_os_heading = 0;
  m_os_speed = 0;
  m_collab_x = 0;
  m_collab_y = 0;
  m_collab_depth = 0;
  m_collab_heading = 0;
  m_collab_speed = 0;
  m_collab_registered = false; 

  m_acoustic_path = "";
  m_connectivity_loc_ref = "";

// These constants are defined in main.cpp
// ---------------------------------------------
//  surface_sound_speed = 1480;   meters per second
//  sound_speed_gradient = 0.016;   m/s per meter i think
//  water_depth = 6000;   meters
}

//---------------------------------------------------------
// Destructor

CommunicationAngle::~CommunicationAngle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    string sval = msg.GetString();
    double dval = msg.GetDouble();

    if (key == "COLLABORATOR_NAME") {
      m_collab_registered = true;
      m_collab_name = sval;
    } else if (key == "VEHICLE_NAME") {
      m_os_name = sval;
    } else if (key == "NAV_X") {
      m_os_x = dval;
    } else if (key == "NAV_Y") {
      m_os_y = dval;
    } else if (key == "NAV_DEPTH") {
      m_os_depth = dval;
    } else if (key == "NAV_HEADING") {
      m_os_heading = dval;
    } else if (key == "NAV_SPEED") {
      m_os_speed = dval;
    }

    if (m_collab_registered) {
      if (key == m_collab_name + "_NAV_X") {
        m_collab_x = dval;
      } else if (key == m_collab_name + "_NAV_Y") {
        m_collab_y = dval;
      } else if (key == m_collab_name + "_NAV_DEPTH") {
        m_collab_depth = dval;
      } else if (key == m_collab_name + "_NAV_HEADING") {
        m_collab_speed = dval;
      }

    }


#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle::Iterate()
{
  
  
  
  Notify("ACOUSTIC_PATH", m_acoustic_path);
  //elev_angle=xxx.x,trans_loss=yyy.y,depth=ddd.d,id=user@
  Notify("CONNECTIVITY_LOCATION", m_connectivity_loc_ref);
  //x=xxx.xxx,y=yyy.yyy,depth=ddd.d,id=user@

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle::RegisterVariables()
{
  Register("VEHICLE_NAME", 0);
  Register("COLLABORATOR_NAME", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_HEADING", 0);
  Register("NAV_SPEED", 0);
  
  if (m_collab_registered) {
    Register(m_collab_name + "_NAV_X", 0);
    Register(m_collab_name + "_NAV_Y", 0);
    Register(m_collab_name + "_NAV_DEPTH", 0);
    Register(m_collab_name + "_NAV_HEADING", 0);
    Register(m_collab_name + "_NAV_SPEED", 0);
  }

  // Register("FOOBAR", 0);
}


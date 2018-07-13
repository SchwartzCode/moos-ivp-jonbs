/************************************************************/
/*    NAME: Jonathan B Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath2.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GenPath2.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath2::GenPath2()
{
  m_points_sent = false;
  m_capture_dist = 5;
  m_done_parousing = false;

  for (int i=0; i<50; i++)
    m_point_reached[i] = false;

} 

//---------------------------------------------------------
// Destructor

GenPath2::~GenPath2()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath2::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    string sval = msg.GetString();
    double dval = msg.GetDouble();

    if (key=="VISIT_POINT")
      m_input.push_back(sval);
    else if (key=="NAV_X")
      m_osX = dval;
    else if (key == "NAV_Y")
      m_osY = dval;
    else if (key=="RETURN" && sval=="true")
      m_done_parousing = true;

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

bool GenPath2::OnConnectToServer()
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

bool GenPath2::Iterate()
{

    if (m_input.size() == 52 && !m_points_sent)// && m_points_sent == false)
    {
      organizePoints();
      m_points_sent = postPoints(); 
    }

    if (m_points_sent)
    {
      for (int i=0; i<50; i++)
      {
        if (!m_point_reached[i] && hypot(m_osX-orgPoints.get_vx(i), m_osY-orgPoints.get_vy(i)) < m_capture_dist) {
          m_point_reached[i] = true;
          Notify("POINT_REACHED", i);
        }
      }
    }

    if (m_done_parousing)
    {
      missedPoints();
      for (int a=0; a<50; a++)
      { 
        if (!m_point_reached[a])
            Notify("POINT_MISSED", a);
      }
    }

  return(true);
}

//-------------------------------------------------------
//Procedure: missedPoints()
//           makes a new seglist with the points the original parousing missed
void GenPath2::missedPoints()
{
  XYSegList missed_pts;

  for (int i=0; i<50; i++)
  {
    if (!m_point_reached[i])
      missed_pts.add_vertex(orgPoints.get_vx(i), orgPoints.get_vy(i));
  }


  string updates_str = "points = ";
  updates_str += missed_pts.get_spec();

  if (missed_pts.size() > 0)
  {
    m_done_parousing = false;

    Notify("WAYPOINT_UPDATES", updates_str);
    Notify("POINTS", "true");
    Notify("RETURN", "false");
  }
}
 
//---------------------------------------------------------
// Procedure: organizePoints()
//            supposed to make the vehicle path a little better
void GenPath2::organizePoints()
{
  vector<string> input(m_input.begin()+1, m_input.end()-1);
  
  for (int i=0; i<50; i++)
  {  
  //  Notify("werkin", input[i]);
    string x = input[i].substr(input[i].find("x=")+2, input[i].find(",y=")-(input[i].find("x=")+2));
    string y = input[i].substr(input[i].find("y=")+2, input[i].find(",id=")-(input[i].find("y=")+2));
    my_seglist.add_vertex(stod(x), stod(y));
  }

  int pSize = my_seglist.size();

  double x_prev = my_seglist.get_vx(0);
  double y_prev = my_seglist.get_vy(0);

  orgPoints.add_vertex(x_prev, y_prev);
  my_seglist.delete_vertex(0);
  

  for (int c=1; c<pSize; c++) //hehe c++ get it?
  {
    unsigned int close_dex = my_seglist.closest_vertex(x_prev, y_prev);
    x_prev = my_seglist.get_vx(close_dex);
    y_prev = my_seglist.get_vy(close_dex);
    orgPoints.add_vertex(x_prev, y_prev);
    my_seglist.delete_vertex(close_dex);

  }


}

//---------------------------------------------------------
// Procedure: postPoints()
//            sends points to the waypoint behavior

bool GenPath2::postPoints()
{
  string updates_str = "points = ";
  

  updates_str += orgPoints.get_spec();

  Notify("WAYPOINT_UPDATES", updates_str);
  
  return true;
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath2::OnStartUp()
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

      if (param == "visit_radius")
        m_capture_dist = stoi(value);
    }
  }
  
  Notify("PA_BEGIN", "true");
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void GenPath2::RegisterVariables()
{
  Register("VISIT_POINT", 0);
  // Register("FOOBAR", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0); 

  //Debugging
  Register("POINT_REACHED", 0);
  Register("RETURN", 0);
}

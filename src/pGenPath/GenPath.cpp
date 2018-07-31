/************************************************************/
/*    NAME: Jonathan B Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GenPath.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  m_points_sent = false;
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    string sval = msg.GetString();

    if (key=="VISIT_POINT")
    {
      m_input.push_back(sval);
      Notify("POINT_RECIEVED", sval);
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

bool GenPath::OnConnectToServer()
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

bool GenPath::Iterate()
{
  Notify("PG_RUNNING", "true");

    if (m_input.size() == 52 && !m_points_sent)// && m_points_sent == false)
    {
      Notify("POST", m_input.front());
      organizePoints();
      m_points_sent = postPoints(); 
    }

  return(true);
}
 
//---------------------------------------------------------
// Procedure: organizePoints()
//            supposed to make the vehicle path a little better
void GenPath::organizePoints()
{
  vector<string> input(m_input.begin()+1, m_input.end()-1);
  
  for (int a=0; a<50; a++)
  {
    string x = input[a].substr(input[a].find("x=")+2, input[a].find(",y=")-(input[a].find("x=")+2));
    string y = input[a].substr(input[a].find("y=")+2, input[a].find(",id=")-(input[a].find("y=")+2));
    double x_val = stod(x);
    double y_val = stod(y);

    m_points[0][a] = x_val;
    m_points[1][a] = y_val;
  }

  int index = 1;
  while (index<49)
  {
    int close_dex = index;
    double lowest_dist = hypot(m_points[0][index]-m_points[0][index-1], m_points[1][index]-m_points[1][index-1]);

    for (int b=index+1; b<50; b++)
    {
      double hypot_holder = hypot(m_points[0][b]-m_points[0][index], m_points[1][b]-m_points[1][index]);
      if (hypot_holder < lowest_dist) {
        lowest_dist = hypot_holder;
        close_dex = b;
      }
        
    }

    double hold_x = m_points[0][close_dex];
    double hold_y = m_points[1][close_dex];

    m_points[0][close_dex] = m_points[0][index];
    m_points[1][close_dex] = m_points[1][index];
    
    m_points[0][index] = hold_x;
    m_points[1][index] = hold_y;
    
    swap(m_input[index+1], m_input[close_dex+1]);

    index++;
  }
}

//---------------------------------------------------------
// Procedure: postPoints()
//            sends points to the waypoint behavior

bool GenPath::postPoints()
{
  string updates_str = "points = ";

  for (int i=0; i<50; i++)
  {  
    string x = to_string(m_points[0][i]);
    string y = to_string(m_points[1][i]);

    updates_str += x + "," + y;
    if (i<49)
      updates_str += ":";
  }

  Notify("WAYPOINT_UPDATES", updates_str);
  
  return true;
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
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
  
  Notify("PA_BEGIN", "true");
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void GenPath::RegisterVariables()
{
  Register("VISIT_POINT", 0);
  // Register("FOOBAR", 0);
  
  //Debugging variables
  Register("POINT_RECIEVED", 0);
  Register("MSIZE", 0);
  Register("POST", 0);
  Register("X_VAL", 0);
  Register("Y_VAL", 0);
}


/************************************************************/
/*    NAME: THE Jonathan Schwartz                                 */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include "PointAssign.h"

using namespace std;


//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  m_points_given = false;
  m_vector_size = 0;
  m_assign_by_region = false;  //default, other option is splitting points between east and west
  m_good_message_count=0;
  m_bad_message_count=0;
  m_post_points = false;

  a=0;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
  Notify("MAIL_IN", "woop");

  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
  
    string key = msg.GetKey();
    string sval = msg.GetString();

    if (key == "VISIT_POINT"){
      m_input.push_back(sval);
      m_good_message_count++;
      Notify("IN_MAIL", m_input.back()); 
    }

    if (key == "PA_BEGIN")
      if (sval == "true")
        m_post_points = true;


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

bool PointAssign::OnConnectToServer()
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

bool PointAssign::Iterate()
{
  AppCastingMOOSApp::Iterate();


  m_vector_size = m_input.size();
  a++;

  //messed up around here or in PointDistribution()
 if (m_input.size() == 102 && !m_points_given && m_post_points) {
         

      Notify("VISIT_POINT_HENRY", "firstpoint");
      Notify("VISIT_POINT_GILDA", "firstpoint");
      
      if (m_assign_by_region) {
        m_points_given = regionalDistribution();
      } else {m_points_given = splitDistribution();}

      Notify("POSTED", "yeet" + to_string(a));
      
 
      Notify("VISIT_POINT_HENRY", "lastpoint");
      Notify("VISIT_POINT_GILDA", "lastpoint");
 }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

void PointAssign::postViewPoint(string input, string color)
 {
    string x = input.substr(input.find("x=")+2, input.find(",y=")-(input.find("x=")+2));
    double x_pos = stod(x);

    string y = input.substr(input.find("y=")+2, input.find(",id=")-(input.find("y=")+2));
    double y_pos = stod(y);

    string id = input.substr(input.find("id=")+3);

   XYPoint point(x_pos, y_pos);
   point.set_label(id);
   point.set_color("vertex", color);
   point.set_param("vertex_size", "2");

   string spec = point.get_spec();
   Notify("VIEW_POINT", spec);
 }

//---------------------------------------------------------
// Procedure: splitDistribution()
//            alternating which vehicle recieves the points

bool PointAssign::splitDistribution()
{
  string vec_len = to_string(m_vector_size);

  /*
  if (m_input.front() != "firstpoint") {
    reportRunWarning("ERROR: Missing first point, only recieved" + vec_len + " of 102 points.");
    return(false);
  }
  
  if (m_input.back() != "lastpoint") {
    reportRunWarning("ERROR: Missing last point, only recieved " + vec_len + " of 102 points.");
    return false;
  }
*/


    for (int i=1; i<m_input.size()-1; i++) {
      if(i%2 == 0)
        m_output_v1.push_back(m_input[i]);
      else {m_output_v2.push_back(m_input[i]);}
    }


  //Posting points
  for (int a=0; a<m_output_v1.size(); a++) {
    
    postViewPoint(m_output_v1[a], "red");
    postViewPoint(m_output_v2[a], "yellow");

    Notify("VISIT_POINT_HENRY", m_output_v1[a]);
    Notify("VISIT_POINT_GILDA", m_output_v2[a]);
  
    
  
  }

  

  return true;
}

//---------------------------------------------------------
// Procedure: regionalDistribution()
//            splitting points up based on their x coordinates
bool PointAssign::regionalDistribution()
{
  vector<string> storage(m_input.begin()+1, m_input.end()-1);
  Notify("STORAGE", storage[0]);

  vector<double> x_vals;
  for (int i=1; i<m_input.size()-1; i++)
  {
    string x_in = m_input[i].substr(m_input[i].find("x=")+2, m_input[i].find(",y=")-(m_input[i].find("x=")+2));
    x_vals.push_back(stod(x_in));
  }

  int unsorted = x_vals.size();


  while (unsorted>1) {
      int maxdex = 0;
      double max = x_vals[0];
    for (int j=0; j<unsorted; j++) {
          if (x_vals[j] > max) {
            max = x_vals[j];
            maxdex = j;
          }
      }
      //literally just copy these two lines for the vector of strings and it'll
      //be one tasty situation
      x_vals[maxdex] = x_vals[unsorted-1];
      x_vals[unsorted-1] = max;

      string holder = storage[maxdex];
      storage[maxdex] = storage[unsorted-1];
      storage[unsorted-1] = holder;
      
      unsorted--;
  }
  
  for (int a=0; a<50; a++)
    m_output_v1.push_back(storage[a]);

  for (int b=50; b<100; b++)
    m_output_v2.push_back(storage[b]);


  //Posting points
  for (int a=0; a<m_output_v1.size(); a++) {
    
    postViewPoint(m_output_v1[a], "red");
    postViewPoint(m_output_v2[a], "yellow");

    Notify("VISIT_POINT_GILDA", m_output_v1[a]);
    Notify("VISIT_POINT_HENRY", m_output_v2[a]);

  }

  return true;
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      Notify("param", value);

      if (param == "arrange_by_region") {
        if (value == "true")
          m_assign_by_region = true;
        else if (value == "false")
          m_assign_by_region = false;
      }


      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
 
  Notify("UTS_PAUSE", "false"); 
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PointAssign::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();

   Register("VISIT_POINT", 0);
  // Register("FOOBAR", 0);

  Register("PA_BEGIN", 0);
  Register("param", 0);
}

bool PointAssign::buildReport()
{
  m_msgs << "Number of good messages: " << m_good_message_count << endl;
  m_msgs << "Number of bad messages: " << m_bad_message_count << endl;

  return true;
}

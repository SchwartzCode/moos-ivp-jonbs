/************************************************************/
/*    NAME: Jonathan Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_TempGradient.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_TempGradient.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_TempGradient::BHV_TempGradient(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "Sine_Wave_Solver_B-)");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");
  m_temp_low = 110; //water wont be above boiling temp
  m_temp_high = -10; //water wont be below freezing temp
  m_diff = 0;


  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, NAV_HEADING, UCTD_MSMNT_REPORT");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_TempGradient::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "foo") && isNumber(val)) {
    // Set local member variables here
    return(true);
  }
  else if (param == "bar") {
    // return(setBooleanOnString(m_my_bool, val));
  }
  else if (param == "polygon"){
    postMessage("POINTS", val);
    handleSearchPolygon(val);
    
    return(true);
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_TempGradient::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_TempGradient::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_TempGradient::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_TempGradient::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_TempGradient::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_TempGradient::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_TempGradient::onRunToIdleState()
{
}

void BHV_TempGradient::handleSearchPolygon(string input)
{
    biteString(input, '{');
    string points = biteString(input, '}');
    postMessage("B", points);

    vector<string> split_points= parseString(points, ':');
    string updates_str = "points = ";

    postMessage("A", split_points[1]);
/*
    for (int i=0; i<split_points.size(); i++)
    {  
      string x = to_string([i]);
      string y = to_string(m_points[1][i]);

      updates_str += x + "," + y;
      if (i<49)
        updates_str += ":";
    }

  Notify("WAYPOINT_UPDATES", updates_str);
*/
    //vector<string> parsed_input = parsestring(points, ',');
    
    /*
    search_region.push_back(x-min);
    search_region.push_back(x-max);
    search_region.push_back(y-min);
    search_region.push_back(y-max);
    */

}

void BHV_TempGradient::handleMailReport(string input)
{

  vector<string> vals = parseString(input, ',');

  for (int i=0; i<vals.size(); i++)
  {
    string param = biteString(vals[i], '=');
  
    if (param == "x")
      m_rep_x = stod(vals[i]);
    else if (param == "y")
      m_rep_y = stod(vals[i]);
    else if (param == "temp")
      m_rep_temp = stod(vals[i]);
  
    postMessage("TEMP", m_rep_temp);
    
    if (m_rep_temp > m_temp_high)
    {
      m_temp_high = m_rep_temp;
      postMessage("NEW_HIGH_TEMP", to_string(m_temp_high));
    }
    
    //have m_rep_temp as a condition because it is initially passed in as
    //0 which supercedes all other low temperatures (at least some of the time)
    if (m_rep_temp < m_temp_low && m_rep_temp)
    {
      m_temp_low = m_rep_temp;
      postMessage("NEW_LOW_TEMP", to_string(m_temp_low));
    }
  }

  m_diff = m_temp_high - m_temp_low;
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_TempGradient::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;
  
  bool recieved = true;
  m_osx = getBufferDoubleVal("NAV_X", recieved);
  if (!recieved)
    postWMessage("WARNING: NAV_X variable is missing!");

  m_osy = getBufferDoubleVal("NAV_Y", recieved);
  if (!recieved)
    postWMessage("WARNING: NAV_Y variable missing!");

  string report = getBufferStringVal("UCTD_MSMNT_REPORT", recieved);
  if (recieved)
    handleMailReport(report);



  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}


/************************************************************/
/*    NAME: Jonathan Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_Pulse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Pulse::BHV_Pulse(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("WPT_INDEX");
  addInfoVars("NAV_X, NAV_Y");

  //defaults in case user doesn't enter any values in the config
  m_p_range = 20;
  m_p_duration = 4;

  m_osx = 0;
  m_osy = 0;
  m_curr_time = 0;
  m_wpt_index = 0;
  m_pulse_time = 10;

  m_pulse_ready = false;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Pulse::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "pulse_range") && isNumber(val)) {
    m_p_range = double_val;
    return(true);
  }
  else if((param == "pulse_duration") && isNumber(val)) {
    m_p_duration = double_val;
    return(true);
  }
  else if (param == "bar") {
    // return(setBooleanOnString(m_my_bool, val));
    
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_Pulse::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Pulse::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Pulse::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Pulse::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Pulse::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Pulse::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Pulse::onRunToIdleState()
{
}

//-------------------------------------------------------------------
// Procedure: pulseIt()
//   Purpose: to post a range pulse to the pMarineViewer simulation

void BHV_Pulse::pulseIt()
{
  XYRangePulse pulse;
  pulse.set_x(m_osx);
  pulse.set_y(m_osy);
  pulse.set_label("bhv_pulse");
  pulse.set_rad(m_p_range);
  pulse.set_duration(m_p_duration);
  pulse.set_time(m_pulse_time);
  pulse.set_color("edge", "yellow");
  pulse.set_color("fill", "yellow");

  string spec = pulse.get_spec();
  
  postMessage("PULSE_ATTEMPT", spec);
  postMessage("VIEW_RANGE_PULSE", spec);

  m_pulse_ready = false;
}


//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Pulse::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  bool recieved = true;
  m_osx = getBufferDoubleVal("NAV_X", recieved);
  if (!recieved)
    postWMessage("WARNING: NAV_X variable is missing");
 
  m_osy = getBufferDoubleVal("NAV_Y", recieved);
  if (!recieved)
    postWMessage("WARNING: NAV_Y variable missing!");

  m_curr_time = getBufferCurrTime();

  double new_wpt_index =  getBufferDoubleVal("WPT_INDEX", recieved);

  //no idea why moos thinks I'm not recieving the WPT_INDEX
 // if (!recieved)
 //   postWMessage("WARNING: WPT_INDEX variable missing!");

  if (new_wpt_index != m_wpt_index)
  {
    m_pulse_time = m_curr_time + 5;
    m_pulse_ready = true;
    m_wpt_index = new_wpt_index;
  }


  if (m_pulse_ready &&  m_curr_time > m_pulse_time)
    pulseIt();

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.


  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}


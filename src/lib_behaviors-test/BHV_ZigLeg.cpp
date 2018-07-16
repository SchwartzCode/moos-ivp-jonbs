/************************************************************/
/*    NAME: Jonathan Schwartz                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_ZigLeg.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_ZigLeg::BHV_ZigLeg(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("WPT_INDEX, NAV_HEADING");
  addInfoVars("NAV_X, NAV_Y");

  //defaults in case user doesn't enter any values in the config
  m_zig_angle = 45;
  m_zig_duration = 10;

  m_osx = 0;
  m_osy = 0;
  m_curr_time = 0;
  m_wpt_index = 0;
  m_heading = 0;

  m_zig_time = false;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_ZigLeg::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "zig_angle") && isNumber(val)) {
    m_zig_angle = double_val;
    return(true);
  }
  else if((param == "zig_duration") && isNumber(val)) {
    m_zig_duration = double_val;
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

void BHV_ZigLeg::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_ZigLeg::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_ZigLeg::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_ZigLeg::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_ZigLeg::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_ZigLeg::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_ZigLeg::onRunToIdleState()
{
}


//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_ZigLeg::onRunState()
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

  m_heading = getBufferDoubleVal("NAV_HEADING", recieved);
  if (!recieved)
    postWMessage("WARNING: NAV_HEADING variable is missing!");

  m_curr_time = getBufferCurrTime();

  double new_wpt_index =  getBufferDoubleVal("WPT_INDEX", recieved);

  //no idea why moos thinks I'm not recieving the WPT_INDEX
 // if (!recieved)
 //   postWMessage("WARNING: WPT_INDEX variable missing!");

  if (new_wpt_index != m_wpt_index)
  {
    m_zig_time = getBufferCurrTime() + 5;
  
    m_wpt_index = new_wpt_index;
  }

  if (!m_zig_it && m_zig_time < m_curr_time)
  {
    m_zig_it = true;
    m_zigging_heading = m_heading + m_zig_angle;
  }


  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.

  if (m_zig_it && m_zig_duration>(m_curr_time - m_zig_time))
  {
    ipf = buildFunctionWithZAIC();
  }
  else if (m_zig_it && m_zig_duration<(m_curr_time - m_zig_time))
    m_zig_it = false;

  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}


//---------------------------------------------------------------
// Procedure: buildFunctionWithZAIC()
//     Purpose: Builds and IvP function for the behavior

IvPFunction* BHV_ZigLeg::buildFunctionWithZAIC()
{
  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(5);
  spd_zaic.setBaseWidth(0.3);
  spd_zaic.setPeakWidth(0.0);
  spd_zaic.setSummitDelta(0.0);
  IvPFunction *spd_of = spd_zaic.extractIvPFunction();

  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_zigging_heading);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setValueWrap(true);
  IvPFunction *crs_of = crs_zaic.extractIvPFunction();

  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(crs_of, spd_of);

  postMessage("ZIG", m_wpt_index);

  return(ipf);
}

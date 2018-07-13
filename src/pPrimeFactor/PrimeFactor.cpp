/************************************************************/
/*    NAME: Jonathan Schwartz
                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstdint>
#include <sstream>

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  input = "15";
  output = "PRIMES_OF:" + input + "=";
  storage = stoll(input);
}

//---------------------------------------------------------
// Destructor

PrimeFactor::~PrimeFactor()
{
}

//----------------------------------------------------
//Function to deal with converting large integers to strigns
string PrimeFactor::big_int_to_string(int64_t value)
{
  ostringstream os;
  os << value;
  return os.str();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();
    string sval = msg.GetString();

    

    if (key == "NUM_VALUE") {
      input = sval;    
      storage = stoll(input);
      output = "Primes_OF:" + input + "=";
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

bool PrimeFactor::OnConnectToServer()
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

bool PrimeFactor::Iterate()
{
  bool print;

  while (storage > 1) {
    print = true;
    for (int64_t i=2; i<=storage; i++)
    {
      if (sqrt(storage) < i) {
        output += big_int_to_string(storage);
        storage = 1;
        break;
      } else if (storage%i==0) {
        output += big_int_to_string(i) + ":";
        storage = storage/i;
        break;
      }
    }
  }

  if (print)
    Notify("PRIME_RESULT", output);

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
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

void PrimeFactor::RegisterVariables()
{
  Register("NUM_VALUE", 0);
  // Register("FOOBAR", 0);
}


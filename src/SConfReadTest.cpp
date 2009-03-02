/*----------------------------------------------------------------------------
 * Copyright (c) 2007      Jefferson Science Associates, LLC               
 *                         Under U.S. DOE Contract No. DE-AC05-06OR23177  
 *                                                                      
 *                         Thomas Jefferson National Accelerator Facility
 *
 *                         Jefferson Lab 
 *                         Scientific Computing Group,
 *                         12000 Jefferson Ave.,      
 *                         Newport News, VA 23606 
 *----------------------------------------------------------------------------
 *  
 * Description:
 *     Simple Test Program to Test ConfDataStoreDB Class
 *     
 *
 * Author:
 *     Jie Chen
 *     Scientific Computing Group
 *     Jefferson Lab
 *      
 * Revision History:
 *   $Log: SConfReadTest.cpp,v $
 *   Revision 1.1  2009-03-02 23:27:26  chen
 *   Test DBMerge Code
 *
 *
 *
 *
 */
#include <fstream>
#include <iostream>
#include "DBString.h"
#include "VFloatData.h"
#include "ConfDataStoreDB.h"

using namespace std;
using namespace FFDB;

int
main (int argc, char** argv)
{
  if (argc < 7) {
    cerr << "Usage: " << argv[0] << " cachesize rearrange(0|1) dbasename stringfile vectorlength confnum" << endl;
    return -1;
  }
  
  unsigned int cachesize = atoi (argv[1]);
  int rearrange = atoi (argv[2]);
  std::string dbase (argv[3]);
  std::string strfile(argv[4]);
  unsigned int vectorlen = atoi(argv[5]);
  unsigned int confnum = atoi(argv[6]);

  // open string file stream
  ifstream sf(argv[4]);
  if (sf.bad()) {
    cerr << "Cannot open file " << strfile << endl;
    return -1;
  }
  
  // Open database
  ConfDataStoreDB<StringKey, VFloatData> dbtest;

  dbtest.setCacheSize (cachesize);
  if  (rearrange)
    dbtest.enablePageMove ();

  if (dbtest.open (dbase, O_RDONLY, 0664) != 0) {
    cerr << "cannot open database " << dbase << endl;
    sf.close ();
    return -1;
  }

  // get user data
  std::string userdata;
  if (dbtest.getUserdata (userdata) != 0) {
    cerr << " Cannot get user data" << endl;
    sf.close ();
    dbtest.close ();
    return -1;
  }
  cerr << userdata << endl;

  VFloatData expected(vectorlen, (float)confnum);
  while (sf.good()) {
    string t1;

    sf >> t1;

    if (t1.length() == 0)
      break;

    StringKey key(t1);
    VFloatData recv;

    if (dbtest.get(key, recv) != 0) {
      cerr << "Cannot find data  " << endl;
      sf.close ();
      dbtest.close ();
      return -1;
    }
    else {
      for (int i = 0; i < recv.numberOfElements(); i++)
	cerr << ((vector<float>)recv)[i];
      cerr << endl;

      for (int i = 0; i < expected.numberOfElements(); i++)
	cerr << ((vector<float>)expected)[i];
      cerr << endl;

      if (recv != expected) {
	cerr << "Retreive data error" << endl;
	sf.close ();
	dbtest.close ();
	return -1;
      }
    }
  }
  sf.close ();


  // dump all data
  vector<StringKey> keys;
  vector<VFloatData> data;
  dbtest.keysAndData (keys, data);

  for (int i = 0; i < keys.size(); i++) {
    cerr << "Key " << i << endl;
    cerr << (string)keys[i] << endl;
    for (int k = 0; k < data[i].numberOfElements(); k++) {
      cerr << ((vector<float>)data[i])[k];
    }
    cerr << endl;
    cerr << endl;
  }
  dbtest.close ();

  return 0;
}
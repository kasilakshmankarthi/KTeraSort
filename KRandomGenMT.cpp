#include <iostream>
#include <limits>
#include <random>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

using namespace std;

int main(int argc, char *argv[])
{
  uint32_t randomNumber;
  uint64_t nOutputs;
  uint32_t lLimit = 0;
  uint32_t uLimit = numeric_limits<uint32_t>::max(); //pow(2,32) -1; //(2^32) - 1
  uint32_t lfsr;

  FILE *pFile;

  if(argc < 2)
  {
     cout << "Usage: "<<"Enter the following 3 args separated by spaces"<<"\n"
         <<"Number of outputs to be generated"<<"\n"
         <<"Lower Limit for RNG"<<"\n"
         <<"Upper Limit for RNG"<<endl;
     exit(1);
  }

  nOutputs = atol(argv[1]);
  if(argc > 2)
  {
    lLimit = atoi(argv[2]);
    uLimit = atoi(argv[3]);
  }

  char fName[200], fName1[200], fName2[200], fName3[200];
  char buff[100];
  strcpy (fName,"input-");
  sprintf(buff, "%ld", nOutputs);
  strcat (fName, buff);
  if(lLimit != 0 && uLimit != 0)
  {
      strcat (fName,"-");
      sprintf(buff, "%u", lLimit);
      strcat (fName, buff);
      strcat (fName, "-");
      sprintf(buff, "%u", uLimit);
      strcat (fName, buff);
  }

  strcpy (fName1, fName);
  strcat(fName1,".1bin");
  strcpy (fName2, fName);
  strcat(fName2,".2bin");
  strcpy (fName3, fName);
  strcat(fName3,".3bin");

  cout<<"Number of outputs to be generated: "<<nOutputs<<"\n";
  if(lLimit != 0 && uLimit != 0)
  {
      cout<<"Lower Limit for RNG: "<<lLimit<<"\n";
      cout<<"Upper Limit for RNG: "<<uLimit<<"\n";
  }

  pFile = fopen(fName1, "wb");
  cout<<"Randon numbers will be written into: "<<fName1<<"\n";
  mt19937 mt1(3451432161);
  uniform_int_distribution<uint32_t> dist;//(lLimit, uLimit);
  for(uint64_t i = 0; i < nOutputs; i++)
  {
    randomNumber = dist(mt1);
    //cout<<randomNumber<<"\n";
    fwrite(&randomNumber, sizeof(randomNumber), 1, pFile);
  }
  fclose (pFile);
  cout<<"Random number file generation (Set 1) completed"<<"\n";

  pFile = fopen(fName2, "wb");
  cout<<"Randon numbers will be written into: "<<fName2<<"\n";
  mt19937 mt2(3415020193);
  for(uint64_t i = 0; i < nOutputs; i++)
  {
    randomNumber = dist(mt2);
    //cout<<randomNumber<<"\n";
    fwrite(&randomNumber, sizeof(randomNumber), 1, pFile);
  }

  fclose (pFile);
  cout<<"Random number file generation (Set 2) completed"<<"\n";

  pFile = fopen(fName3, "wb");
  cout<<"Randon numbers will be written into: "<<fName3<<"\n";
  mt19937 mt3(3485125855);
  for(uint64_t i = 0; i < nOutputs; i++)
  {
    randomNumber = dist(mt3);
    //cout<<randomNumber<<"\n";
    fwrite(&randomNumber, sizeof(randomNumber), 1, pFile);
  }
  fclose (pFile);
  cout<<"Random number file generation (Set 3) completed"<<"\n";

  return (0);
}

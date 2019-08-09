#include "QRandomGenLFSR.h"

using namespace std;

int main(int argc, char *argv[])
{
  uint32_t randomNumber;
  uint64_t nOutputs;
  uint32_t lLimit = 0;
  uint32_t uLimit = 0; //pow(2,32) -1; //(2^32) - 1
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
  lfsr = 0xCDB8ACE1u;
  for(uint64_t i = 0; i < nOutputs; i++)
  {
    if(lLimit != 0 && uLimit != 0)
    {
      randomNumber = lLimit + (randLFSR(lfsr) % (uLimit - lLimit + 1));
    }
    else
    {
      randomNumber = randLFSR(lfsr);
    }
    //cout<<randomNumber<<"\n";
    fwrite(&randomNumber, sizeof(randomNumber), 1, pFile);
  }
  fclose (pFile);
  cout<<"Random number file generation (Set 1) completed"<<"\n";

  pFile = fopen(fName2, "wb");
  cout<<"Randon numbers will be written into: "<<fName2<<"\n";
  lfsr = 0xCB8D12A1u;
  for(uint64_t i = 0; i < nOutputs; i++)
  {
    if(lLimit != 0 && uLimit != 0)
    {
      randomNumber = lLimit + (randLFSR(lfsr) % (uLimit - lLimit + 1));
    }
    else
    {
      randomNumber = randLFSR(lfsr);
    }
    //cout<<randomNumber<<"\n";
    fwrite(&randomNumber, sizeof(randomNumber), 1, pFile);
  }

  fclose (pFile);
  cout<<"Random number file generation (Set 2) completed"<<"\n";

  pFile = fopen(fName3, "wb");
  cout<<"Randon numbers will be written into: "<<fName3<<"\n";
  lfsr = 0xCFBACCDFu;
  for(uint64_t i = 0; i < nOutputs; i++)
  {
    if(lLimit != 0 && uLimit != 0)
    {
      randomNumber = lLimit + (randLFSR(lfsr) % (uLimit - lLimit + 1));
    }
    else
    {
      randomNumber = randLFSR(lfsr);
    }

    //cout<<randomNumber<<"\n";
    fwrite(&randomNumber, sizeof(randomNumber), 1, pFile);
  }
  fclose (pFile);
  cout<<"Random number file generation (Set 3) completed"<<"\n";

  return (0);
}

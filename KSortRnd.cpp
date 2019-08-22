#include "QSortRnd.h"

// Test Configuration
bool g_testAll    = false;
bool g_testSanity = false;
bool g_testSort   = false;

struct TestOpts {
  bool         &var;
  const char*  name;
};

TestOpts g_testopts[] =
{
  { g_testAll, 		  "all" },
  { g_testSanity, 	"sanity" },
  { g_testSort, 	  "sort" }
};

using namespace std;

/**
 * Driver to run and test
 */
int main(int argc, char *argv[])
{
#ifdef STD_INPUT_TESTING
  std::vector<uint32_t> input = {1,10,15,20,25,50,40,30,20,10,9524,878,17,1,99,18785,3649,164,94,
      123,432,654,3123,654,2123,543,131,653,123,533,1141,532,213,2241,824,1124,42,134,411,
      491,341,1234,527,388,245,1992,654,243,987};

  quickSortRn(input.begin(), input.end());
  //sort(input.begin(), input.end());

  for (uint32_t it : input)
  {
     cout<< "Key:"<<it<<endl;
  }
#else
  printf("QSortRnd Version: %d.%d\n", MAJOR_VERSION, MINOR_VERSION);

  const char *test = "all";
  test = argv[1];

  if (strncmp(test,"--test=", 6) == 0)
  {
    char *opt = (char *)&test[7];
    bool found = false;
    g_testAll = false;

    for(size_t i = 0; i < sizeof(g_testopts) / sizeof(TestOpts); i++)
    {
      if (strcmp(opt, g_testopts[i].name) == 0)
      {
        g_testopts[i].var = true;
        printf("Testing %s \n", g_testopts[i].name);
        found = true;
        break;
      }
    }

    if (!found)
    {
      printf("Invalid option: --test=%s\n", opt);
      printf("Valid tests: --test=%s", g_testopts[0].name);
      for(size_t i = 1; i < sizeof(g_testopts) / sizeof(TestOpts); i++)
      {
        printf(",%s", g_testopts[i].name);
      }
      printf("\n");
      exit(0);
    }
  }

  uint64_t nInputs;
  uint64_t lSize;
  uint64_t lAlloc;
  vector<uint32_t> inArr;
  vector<uint32_t> noVec;
  bool isEnableAngelSignals = false;

  char *fIn;
  char fOut[200];

  FILE *pFile;

  if(argc < 3)
  {
     cout << "Usage: "<<"Enter the following 4 args separated by spaces"<<"\n"
         <<"--test=<all/sanity/sort"<<"\n"
         <<"Input file to be sorted"<<"\n"
         <<"Number of inputs to be sorted"<<"\n"
         <<"Enable(1)/Disable(0) angel signals"<<"\n";
     exit(1);
  }

  fIn = argv[2];
  nInputs = atol(argv[3]);
  isEnableAngelSignals = atoi(argv[4]);

  cout<<"Reading unsorted numbers from: "<<fIn<<"\n";
  cout<<"Number of Inputs to be sorted: "<<nInputs<<"\n";
  cout<<"Enable(1)/Disable(0) angel signals: "<<isEnableAngelSignals<<"\n";

#ifdef PERF_INSTRUMENT
  perf_event_init(  (enable_perf_events) (ENABLE_HW_CYCLES_PER | ENABLE_HW_INSTRS_PER) );
#endif

  // obtain file size (in bytes)
  pFile = fopen(fIn, "rb");
#if 1
  lAlloc = nInputs;
#else
   fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  lAlloc = lSize/sizeof(inArr[0]);
  cout<<"Binary file size read and # of inputs to sort: "<<lSize<<","<<lAlloc<<"\n";
  rewind (pFile);
#endif

  //Allocate the array
  inArr.resize(lAlloc);

  if( pFile )
  {
    //Read the unsorted numbers
    size_t result;
    for(uint64_t i = 0; i < nInputs; i++)
    {
      result = fread(&inArr[i], sizeof(inArr[0]), 1, pFile);
    }
    fclose (pFile);
  }
  else
  {
    cout<<"Wrong Input File specified"<<endl;
    exit(0);
  }

#if defined (ANGEL) && defined (HAVE_LIBANGEL)
  if(isEnableAngelSignals)
  {
    make_angel_control_call("save-checkpoint");
    make_angel_control_call("begin-benchmark");
  }
#endif /* ANGEL && HAVE_LIBANGEL */

#ifdef PERF_INSTRUMENT
  perf_event_enable ( (enable_perf_events) (ENABLE_HW_CYCLES_PER | ENABLE_HW_INSTRS_PER) );
  uint64_t beginI = perf_per_instr_event_read();
  uint64_t beginC = perf_per_cycle_event_read();
#endif

  //make_angel_control_call("print-cycles");
  auto start = get_time::now();
  //Sort the array
  sort(inArr.begin(), inArr.end());
  //quickSortRn(inArr.begin(), inArr.end());
  auto end = get_time::now();

  duration<float> diff = end - start;
  cout<<"Successfully sorted nKeys= "<< lAlloc<<" Time(usec)= "<<((diff).count()*1000000)<<endl;
  cout<<"[nKeysSorted/Sort Time] (nKeys/sec)= "<<(double)(lAlloc/(diff).count())<<endl;
  //make_angel_control_call("print-cycles");

#ifdef PERF_INSTRUMENT
  printf("For sorting keys: %lld instructions,  %lld cycles\n", (perf_per_instr_event_read() - beginI), (perf_per_cycle_event_read() - beginC));
#endif

#if defined (ANGEL) && defined (HAVE_LIBANGEL)
  if (isEnableAngelSignals)
  {
    make_angel_control_call("end-benchmark");
    //Avoid cleaning up data structure and save model run time
    //angel_hypercall(SYS_EXIT, NULL);
    //exit(1);
  }
#endif /* ANGEL && HAVE_LIBANGEL */

  if(g_testSanity || g_testAll)
  {
    if( is_sorted(inArr.begin(), inArr.end()) )
    {
      printf("Sorting validation passed!\n");
    }
    else
    {
      printf("Sorting validation failed!\n");
    }

    /*strcpy (fOut, fIn);
    strcat(fOut,".sort");

    //Store the output for validation
    pFile = fopen(fOut, "wb");
    cout<<"Sorted numbers will be written uint32_to: "<<fOut<<"\n";
    for(uint64_t i = 0; i < nInputs; i++)
    {
      fwrite(&inArr[i], sizeof(inArr[0]), 1, pFile);
      cout<<inArr[i]<<"\n";
    }

    fclose (pFile);
    cout<<"Sorted file write is complete"<<"\n";*/
  }

#ifdef DATA_CLEANUP
  //Avoid cleaning up data structure and save model run time
  inArr.swap(noVec);
#endif

#endif//STD_INPUT_TESTING

  return (0);

}

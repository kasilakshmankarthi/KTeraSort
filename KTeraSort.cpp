#include "QTeraSort.h"

// Test Configuration
bool g_testAll      = false;
bool g_testSanity   = false;
bool g_testPartition = false;

struct TestOpts {
  bool         &var;
  const char*  name;
};

TestOpts g_testopts[] =
{
  { g_testAll, 		"all" },
  { g_testSanity, 	"sanity" },
  { g_testPartition, 	"partition" }
};


struct System
{
  //Based on https://muehe.org/posts/profiling-only-parts-of-your-code-with-perf/
   static void
   profile(const std::string& name,std::function<void()> body)
   {
      std::string filename = name.find(".data") == std::string::npos ? (name + ".data") : name;

      // Launch profiler
      pid_t pid;
      std::stringstream s;
      s << getpid();
      pid = fork();
      printf("Value of pid=%d\n", pid);
      if (pid == -1) {
       /*
        * When fork() returns -1, an error happened.
        */
        perror("fork failed");
        exit(EXIT_FAILURE);
      }
      else if (pid == 0)
      {
          auto fd=open("/dev/null",O_RDWR);
          dup2(fd,1);
          dup2(fd,2);
          printf("Running perf record\n");
          exit(execl("/usr/bin/perf","perf","record","-o",filename.c_str(),"-p",s.str().c_str(),nullptr));
      }

      // Run body
      body();

      // Kill profiler
      kill(pid,SIGINT);
      waitpid(pid,nullptr,0);
  }

  static void profile(std::function<void()> body)
  {
      profile("perf.data",body);
  }
};

int compareTo(vector<byte> buffer1, int length1,
           vector<byte> buffer2, int length2)
{
  // Short circuit equal case
  if ( (buffer1 == buffer2) &&
   (length1 == length2) )
  {
    return 0;
  }

  // Bringing WritableComparator java code local
  int end1 = length1;
  int end2 = length2;
  for (int i = 0, j = 0; i < end1 && j < end2; i++, j++)
  {
    int a = buffer1[i];
    int b = buffer2[j];
    if (a != b)
    {
      return a - b;
    }
  }

  return length1 - length2;
}

/**
 * A generic trie node
 */
class TrieNode;
class InnerTrieNode;
class LeafTrieNode;

/**
 * Abstract base class
 * Create pointers and work with inhertied class
 */
class TrieNode
{
  private:
   int level;

  public:
   TrieNode()
   {
     //Default constr
   }

   TrieNode(int &level_):level(level_)
   {
     DEBUG("Setting level in base class: "<<level<<endl);
   }

   //Virtual base class destructor
   virtual ~TrieNode()
   {
   }

   virtual int findPartition(const vector<byte>& key) = 0;

   int getLevel()
   {
     DEBUG("Returning this constructor: "<<this<<", "<<level<<endl);
     return level;
   }
};

/**
 * An inner trie node that contains 256 children based on the next
 * character.
 */
typedef TrieNode* childPtr;
class InnerTrieNode: public TrieNode
{
  public:
   TrieNode* child[256];

  public:
   InnerTrieNode(int &level_):TrieNode(level_)
   {
     DEBUG("InnerTrieNode Constr Level: "<<level_<<endl);
   }

   ~InnerTrieNode()
   {
      for(int ch=0; ch <=255; ch++)
      {
        delete child[ch];
        child[ch] = NULL;
      }
   }

   int findPartition(const vector<byte>& key)
   {
     int lvl = getLevel();
     if (key.size() <= lvl)
     {
       DEBUG("inner findPartition with size, level "<<key.size()<<","<<lvl<<endl);
       return (child[0]->findPartition(key));
     }
     DEBUG("outer findPartition with level: "<<lvl<<","<<int(key[lvl])<<endl);
     DEBUGC("child[%d] is pointing to=%p\n", int(key[lvl]), child[int(key[lvl])]);
     return ( (child[int(key[lvl])])->findPartition(key) );
   }
};

/**
 * A leaf trie node that does string compares to figure out where the given
 * key belongs between lower..upper.
 */
class LeafTrieNode: public TrieNode
{
 private:
  vector<byte> splitPoint;
  int lower;
  int upper;

 public:
  LeafTrieNode(int &level_, vector<byte> &splitPoint_,
          int &lower_, int &upper_):TrieNode(level_)
  {
    splitPoint = splitPoint_;
    lower = lower_;
    upper = upper_;
    DEBUG("LeafTrieNode Constr Lower: "<<lower<<", "<<"Upper: "<<upper<<", "<<"Level: "<<level_<<endl);
  }

  ~LeafTrieNode()
  {
  }

  int findPartition(const vector<byte>& key)
  {
    DEBUG("LeafTrie Node Lower: "<<lower<<", "<<"Upper: "<<upper<<", "<<"Level: "<<getLevel()<<endl);
#ifdef QTERASORT_DEBUG
    for(byte b : key)
    {
      DEBUG("Key Searched: "<< setw(2) << int(b) << ' ');
    }
#endif
    for(int i=lower; i<upper; ++i)
    {
      //splitPoint.compareTo(key) > 0 (splitPoint > key)
      if ( compareTo(splitPoint, splitPoint.size(), key, key.size()) > 0 )
      {
        DEBUG("Returning i: "<<i<<endl);
        return i;
      }
    }
    DEBUG("Returning upper: "<<upper<<endl);
    return upper;
  }
};


/**
 * Given a sorted set of cut points, build a trie that will find the correct
 * partition quickly.
 * @param splits the list of cut points
 * @param lower the lower bound of partitions 0..numPartitions-1
 * @param upper the upper bound of partitions 0..numPartitions-1
 * @param prefix the prefix that we have already checked against
 * @param maxDepth the maximum depth we will build a trie for
 * @return the trie node that will divide the splits correctly
 */
TrieNode* buildTrie(vector < vector<byte> >& splits, int lower, int upper,
                   vector<byte> prefix, int maxDepth)
{
  int depth = prefix.size();
  DEBUG("Depth: "<<depth<<endl);

  if (depth >= maxDepth || lower == upper)
  {
    //Create LeafTrieNode
    TrieNode *tmp = new LeafTrieNode(depth, splits[lower], lower, upper);
    DEBUGC("Constructing and returning a LeafTrieNode=%p for splits[%d]\n", tmp, lower);
    return (tmp);
  }

  InnerTrieNode *result = new InnerTrieNode(depth);
  DEBUGC("Value of InnerTrieNode result=%p\n", result);

  vector<byte> trial;
  trial = prefix;
  // append an extra byte on to the prefix
  trial.emplace_back(1);
  int currentBound = lower;

  for(int ch = 0; ch < 255; ++ch)
  {
    trial[depth] = (byte) (ch + 1);
#ifdef QTERASORT_DEBUG
    for(byte b : trial)
    {
      DEBUG("Trial:"<< setw(2) << int(b) << ' ');
    }
    DEBUG(endl);
#endif

    lower = currentBound;
    while (currentBound < upper)
    {
      for(byte b : splits[currentBound])
      {
        DEBUG("splits:"<< setw(2) << int(b) << ' ');
      }
      DEBUG(endl);

     //splits[currentBound].compareTo(trial) >= 0  (splits >= trial)
     if( compareTo(splits[currentBound], splits[currentBound].size(),
                  trial, trial.size()) >= 0 )
      {
        DEBUG("Breaking at currentBound: "<<currentBound<<endl);
        break;
      }
      currentBound += 1;
    }
    trial[depth] = (byte) ch;
    DEBUG("Inner call bounds: "<<lower<<","<<currentBound<<endl);
    result->child[ch] = buildTrie(splits, lower, currentBound, trial, maxDepth);
    DEBUGC("Value of result->child[%d]=%p and level=%d\n\n", ch, result->child[ch], result->child[ch]->getLevel());
  }

  // pick up the rest
  trial[depth] = (byte) 255;
  DEBUG("Outer call bounds: "<<currentBound<<","<<upper<<endl);
#ifdef QTERASORT_DEBUG
  TrieNode* tmp =  buildTrie(splits, currentBound, upper, trial,
                                maxDepth);

  DEBUGC("Value of result tmp=%p\n", tmp);
  result->child[255] = tmp;
#else
  result->child[255] = buildTrie(splits, currentBound, upper, trial,
                                maxDepth);
#endif
  DEBUGC("Value of result returned by buildTrie=%p\n", result);
  return result;
}

/**
 * To extract the bytes in a given Key (uint32_t)
 */
template< typename T >
vector<byte>  getKeyBytes(const T& object)
{
  //Key is 4 bytes long. Initialize to all 0s
  vector <byte> bytes (sizeof(uint32_t), 0);

  const byte* begin = reinterpret_cast< const byte* >( std::addressof(object) );
  const byte* end = begin + sizeof(T);

  //Preserve the natural order of numbers from MSB to LSB
  //Example for 4 bytes key length:
  //bytes[0]=MSB, bytes[1]=MSB-1, bytes[2]=MSB-2, bytes[3]=LSB
  std::reverse_copy( begin, end, std::begin(bytes) );

  return bytes;
}

/**
 * To find the length of the key in bytes
 */
// (pow(2,24)-1)
#define U3BYTE_MAX (16777215)
uint8_t getKeyLengthInBytes(uint32_t key)
{
  uint8_t byteSz = 0;
  if(key > U3BYTE_MAX)
  {
    byteSz = 4;
  }
  else if(key > USHRT_MAX)
  {
    byteSz = 3;
  }
  else if(key > UCHAR_MAX)
  {
    byteSz = 2;
  }
  else
  {
    byteSz = 1;
  }

  return(byteSz);
}

/**
 * Create the keyPartVec2D containing the partition keys
 */
template <typename Iterator>
void createPartitions(Iterator startSort,
        vector< vector<byte> >& keyPartVec2D,
        uint32_t& nSplits,
        uint32_t& stride)
{
  uint32_t nPartitions = 0;
  uint32_t keyConv;
  uint32_t keySz;

  while(nPartitions < nSplits)
  {
    //Make sure stride doesn't go past the startSort
    keyConv = *(startSort + (stride * (nPartitions + 1)) );
    DEBUG("Key to be converted:"<<keyConv<<endl);
    keyPartVec2D[nPartitions] = getKeyBytes(keyConv);
#ifdef QTERASORT_DEBUG
    for(byte b : keyPartVec2D[nPartitions])
    {
      DEBUG("KeyPart:"<< setw(2) << int(b) << ' ');
    }
    DEBUG(endl);
#endif
    nPartitions += 1;
  }
}

/**
 * Driver to run and test
 */
int main(int argc, char *argv[])
{
  printf("QTeraSort Version: %d.%d\n", MAJOR_VERSION, MINOR_VERSION);

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
  uint32_t stride;
  uint32_t nBuckets;
  uint32_t nSplits;
  uint32_t bucketId;
  size_t result;
  bool isEnableAngelSignals = false;

  vector<uint32_t> inArr;
#ifndef LARGE_ALLOC
  vector<uint32_t> inSortArr;
  vector<uint32_t> noVec;
#endif
  vector< vector<byte> > keyPartVec2D;

  //To store the partitions
  vector< vector<uint32_t> > outVec2D;

  char *fIn;
  char fOut[200];

  FILE *pFile;

  if(argc < 5)
  {
     cout<< "Usage: "<<"Enter the following 5 args separated by spaces"<<"\n"
         <<"--test=<all/sanitypartition>"<<"\n"
         <<"Input file to be sorted"<<"\n"
         <<"Number of Keys to be sorted"<<"\n"
         <<"Number of buckets (or) partitions"<<"\n"
         <<"Enable(1)/Disable(0) angel signals"<<"\n";

     exit(1);
  }

  fIn = argv[2];
  nInputs = atol(argv[3]);
  nBuckets = atol(argv[4]);
  isEnableAngelSignals = atoi(argv[5]);

  cout<<"Reading unsorted keys from: "<<fIn<<"\n";
  cout<<"Number of keys: "<<nInputs<<"\n";
  cout<<"Number of partitions: "<<nBuckets<<"\n";
  cout<<"Enable(1)/Disable(0) angel signals: "<<isEnableAngelSignals<<"\n";

  //Number of key partitions should be the same as nBuckets since
  //in buildTrie the range of lower, upper is (0,0) to (nBuckets-1, nBuckets-1)
  nSplits = nBuckets;

  //Open the RNG Keys file
  pFile = fopen(fIn, "rb");
#if 1
  lAlloc = nInputs;
#else
  //Obtain number of inputs based on file size (in bytes)
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  lAlloc = lSize/sizeof(inArr[0]);
  DEBUG(<<"Binary file size read and # of inputs to sort: "<<lSize<<","<<lAlloc<<"\n";
  rewind (pFile);
#endif

#ifdef PERF_INSTRUMENT
  perf_event_init(  (enable_perf_events) (ENABLE_HW_CYCLES_PER | ENABLE_HW_INSTRS_PER) );
#endif

  //Allocate the array
  inArr.resize(lAlloc);
#ifndef LARGE_ALLOC
  inSortArr.resize(lAlloc);
#endif
  //1. Read the unsorted numbers (keys)
  if( pFile )
  {
    //Read the unsorted numbers
    for(uint64_t i = 0; i < nInputs; i++)
    {
      result = fread(&inArr[i], sizeof(inArr[0]), 1, pFile);
    }
    fclose (pFile);
    cout<<"Successfully read the keys"<<endl;
  }
  else
  {
    cout<<"Wrong Input File specified"<<endl;
    exit(0);
  }

  //2. Sort the key
#ifndef LARGE_ALLOC
  copy(inArr.begin(), inArr.end(), inSortArr.begin());
#endif

  auto start = get_time::now();
#ifndef LARGE_ALLOC
  sort(inSortArr.begin(), inSortArr.end());
#else
  sort(inArr.begin(), inArr.end());
#endif
  auto end = get_time::now();
  duration<float> diff = end - start;
  DEBUG(chrono::duration_cast<ns>(diff).count()<<endl);
  cout<<"Successfully sorted nKeys= "<<lAlloc<<" Time(usec)= "<<((diff).count()*1000000)<<endl;
  cout<<"[nKeysSorted/Sort Time] (nKeys/sec)= "<<(double)(lAlloc/(diff).count())<<endl;

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
#endif

  //start = get_time::now();
  //3. Create the key partitions
  stride = lAlloc/nSplits;
  keyPartVec2D.resize(nSplits); //Number of rows
#ifndef LARGE_ALLOC
  createPartitions(inSortArr.begin(), keyPartVec2D, nSplits, stride);
#else
  createPartitions(inArr.begin(), keyPartVec2D, nSplits, stride);
#endif
#ifdef PERF_INSTRUMENT
  printf("For createPartitions: %lld instructions\n", (perf_per_instr_event_read() - beginI) );
  beginI = perf_per_instr_event_read();
#endif

  /*end = get_time::now();
  diff = end - start;
  cout<<"Successfully created key partitions using stride: "<<stride<<" Time taken: "<<(diff).count()<<endl;*/

  //4. Construct Trie for key lookup
  //start = get_time::now();
  vector<byte> zeroVec;
  //Indexing starts from 0 and hence (nBuckets-1)
  TrieNode *trie = buildTrie(keyPartVec2D, 0, (nBuckets - 1), zeroVec, PREFIX_LENGTH);
  /*end = get_time::now();
  diff = end - start;
  cout<<"Successfully created Trie lookup. Time taken: "<<(diff).count()<<endl<<endl;*/
#ifdef PERF_INSTRUMENT
  printf("For buildTrie: %lld instructions\n", (perf_per_instr_event_read() - beginI) );
  beginI = perf_per_instr_event_read();
  uint64_t beginC = perf_per_cycle_event_read();
#endif

#ifdef LARGE_ALLOC
  for(uint64_t i = 0; i < nInputs; i++)
  {
    result = fread(&inArr[i], sizeof(inArr[0]), 1, pFile);
  }
  fclose (pFile);
  cout<<"Successfully re-read the keys"<<endl;
#endif

  //5. Create buckets (or) partitioned keys
  outVec2D.resize(nBuckets); //Number of rows for 2D array
  start = get_time::now();
  System::profile("qpartition", [&]()
  {
    for(uint64_t i = 0; i < nInputs; i++)
    {
      bucketId = trie->findPartition(getKeyBytes(inArr[i]));

      //6. Fill the keys in their respective buckets
      outVec2D[bucketId].push_back(inArr[i]);
#ifdef QTERASORT_DEBUG
      if(bucketId == 0)
      {
        DEBUG("Inserting in Bucket"<<bucketId<<" "<<inArr[i]<<endl<<endl);
      }
#endif
    }
  });
  end = get_time::now();
  diff = end - start;
  DEBUG(duration_cast<ns>(diff).count()<<endl);
  cout<<"Successfully partitioned nKeys= "<<lAlloc<<" nBuckets= "<<nBuckets<<" Time(usec)= "<<((diff).count()*1000000)<<endl;
  cout<<"[nKeyPartitioned/Partition Time] (nKeys/sec)= "<<(double)(lAlloc/(diff).count())<<endl;
#ifdef PERF_INSTRUMENT
  printf("For partitioning keys: %lld instructions,  %lld cycles\n", (perf_per_instr_event_read() - beginI), (perf_per_cycle_event_read() - beginC));
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
    vector< pair<uint32_t, uint32_t> > minMax;

    for(uint64_t i = 0; i < nBuckets; i++)
    {
      auto res = minmax_element(outVec2D[i].begin(), outVec2D[i].end());
      cout<<"Bucket: "<<i<<" Size: "<<outVec2D[i].size()<<" Min: "<<*res.first<<" Max: "<<*res.second<<endl;
      if( *res.first > *res.second)
      {
        cout<<"Partitions are not unique. Validation failed!"<<endl;
        exit (0);
      }
      else
      {
        minMax.push_back( make_pair(*res.first, *res.second) );
      }
    }

    for(uint64_t i = 0; i < (nBuckets - 1); i++)
    {
      if( minMax[i].second < minMax[i+1]. first )
      {
        continue;
      }
      else
      {
        cout<<"Partitions are not unique. Validation failed!"<<endl;
        exit (0);
      }
    }

    cout<<"Partitions are unique. Validation Passed!"<<endl;
#if 0
    //8. Store the keys in bucket 0 in a file for validation
    strcpy (fOut, fIn);
    strcat(fOut,".sort");
    pFile = fopen(fOut, "wb");
    bucketId = 0;
    DEBUG("Sorted numbers in bucket: "<<bucketId<<"will be written to: "<<fOut<<"\n");
    sort(outVec2D[bucketId].begin(), outVec2D[bucketId].end());
    for(uint64_t i = 0; i < outVec2D[bucketId].size(); i++)
    {
      fwrite(&outVec2D[bucketId][i], sizeof(outVec2D[bucketId][i]), 1, pFile);
      DEBUG("Terasort: "<<outVec2D[bucketId][i]<<endl);
    }

    fclose (pFile);
    DEBUG("Sorted file write is complete"<<endl);
#endif
  }

//Avoid cleaning up data structure and save model run time
#ifdef DATA_CLEANUP
  //Cleanup allocated data structures
  inArr.swap(noVec);
  inSortArr.swap(noVec);

  for(uint64_t i = 0; i < nSplits; i++)
  {
    keyPartVec2D[i].clear();
  }
  keyPartVec2D.clear();

  for(uint64_t i = 0; i < nBuckets; i++)
  {
    outVec2D[i].clear();
  }
  outVec2D.clear();

  delete trie;
#endif

  return (0);
}

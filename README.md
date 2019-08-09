Benchmark is developed from scatch in C++. Please refer to this JIRA: https://jira.qualcomm.com/jira/browse/ST-792 for more insight. The benchmark is heavily inspired from
the Yahoo TeraSort and Hadoop MapReduce algorithm. Please refer links (1) to (3) below references.

###Example Usage

####1. QRandomGen
Generates random numbers using LFSR. Creates 3 binary files each using different seed value.
The format of the file created is input\-\<No of keys generated\>.1bin

./QRandomGen\.\<platform\>\.\<compiler\>.elf \<Num of outputs (keys) to be generated\>

Example: <br/>
cd binaries <br/>
./QRandomGen.x86_64.ivybridge.g++.6.2.0.linux.elf 16384 <br/>

####2. QSortRnd
Sorts the input file. If --test=sanity, then the sorted output is validated using std::is_sorted()

Following performance metrics are printed out:
- nKeys/Sort Time (sorts/sec)

./QSortRnd\.\<platform\>\.\<compiler\>.elf --test=\<sort/sanity/all\> \<input file\> \<Num of keys in the file\> \<Enable\(1\)/Disable \(0\) angel signals\>

Example:
cd binaries <br/>
./QSortRnd.x86_64.ivybridge.g++.6.2.0.linux.elf --test=sort ../inputs/input-16384.1bin 16384 0 (To run test) <br/>
./QSortRnd.x86_64.ivybridge.g++.6.2.0.linux.elf --test=sanity ../inputsinput-16384.1bin 16384 0 (To run test and perform validation) <br/>

####3. QTeraSort
Reads the generated input file created by Step 1 above. Sorts the input file and uses the stride to create
(nBuckets) split points. Builds a Trie using the split points. Partitions the unsorted input file using Trie lookup. The leaf node of Trie provides the bucket id in which the given
key has to be placed. If --test=sanity, then std::minmax_element() is run on each bucket and then the minima and maxima of each bucket is tested for uniqueness and less than
the next bucket id.

In a real MapReduce algorithm the individual partitioned buckets are sorted and then concatented to form a single output.

Following performance metrics are printed out:
- nKeys/Sort Time (sorts/sec)
- nKeys/Partition Time (nKeys/sec)

./QTeraSort\.\<platform\>\.\<compiler\>.elf --test=\<partition/sanity/all\> \<input file\> \<Num of keys in the file\> \<Num of buckets or partitions\> \<Enable\(1\)/Disable \(0\) angel signals\>

Example: <br/>
cd binaries <br/>
./QTeraSort.x86_64.ivybridge.g++.6.2.0.linux.elf --test=partition ../inputs/input-16384.1bin 16384 100 0 (To run test) <br/>
./QTeraSort.x86_64.ivybridge.g++.6.2.0.linux.elf --test=sanity ../inputs/input-16384.1bin 16384 100 0 (To run test and perform validation) <br/>

###References
1. http://sortbenchmark.org/YahooHadoop.pdf
2. http://www.slideshare.net/tungld/terasort
3. http://static.googleusercontent.com/media/research.google.com/en//archive/mapreduce-osdi04.pdf
4. http://stackoverflow.com/questions/7602919/how-do-i-generate-random-numbers-without-rand-function
5. https://github.com/apache/hadoop/tree/trunk/hadoop-mapreduce-project/hadoop-mapreduce-examples/src/main/java/org/apache/hadoop/examples/terasort
6. https://gist.github.com/lawliet89/4106929
7. http://www.cplusplus.com/forum/beginner/155821/
8. http://grepcode.com/file/repo1.maven.org/maven2/com.facebook.presto.hadoop/hadoop-apache2/0.1/org/apache/hadoop/io/FastByteComparisons.java
9. http://www.cplusplus.com/reference/algorithm/lexicographical_compare/

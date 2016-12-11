/*
30 Nov 7:38 am

Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>


using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss

#define ADDRESSING 32 /* 32-bit addressing mode */
//#define DEBUG
#define DEBUGSIMPLE
#define DEBUGFILE
//#define DEBUGCONCEPT
#define KILOBYTE 1024


struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };

struct wayCellStructure
{
	bitset<32> address;/* the address that it was called with */
	bitset<32> tag_w;/* base Tag Bit-set */
	int used;
	int offset_w;/* offset bits */
	
};
typedef struct wayCellStructure wayCell;
	   
/* you can define the cache class here, or design your own data structure for L1 and L2 cache
class cache {
      
      }
*/   

/*
	Class name	:	cache
	Purpose		:	To create cache data structure for a sepcified input
				:	of blockSize, setSize and cSize	
	Testing		:	Not done				
 */    
class cache
{
	public:
	int offset; /* for offset bits */	
	int index; 	/* for index bits */
	int tag;	/* tag bits */
	int ways;
	bitset<32>accessAddress;/* Store a copy of the address that is being looked for */
	bitset<32>accessTag;/* tag from address to be accessed */
	bitset<32>accessIndex;/* index from address to be accessed */
	bitset<32>accessOffset;/* Offset from address to be accessed */
	vector<vector<wayCell> > wayNumber;
	

	void cacheCalc(int blockSize, int setSize, int cSize)
	{	
		if(((blockSize || setSize) > cSize*KILOBYTE) || ((cSize*KILOBYTE)%blockSize) !=0 || setSize <0) {/* blockSize and setsize cannot be greater than cache size, block size 
		 not a multiple of cache size, negative powers of 2 */
			cout<< "error" <<endl;
			blockSize=1;
			setSize=1;		//if any errors are there then block size and set size are 1 // 
		}
		offset = log2(blockSize);
		if(setSize==0) 
			ways= (cSize*KILOBYTE)/(blockSize);  
		else		
			ways = setSize;
		//index = cSize*KILOBYTE;
		if(setSize==0)
			index=0;		// since no index bits for fully associative cache
		else
			index = log2(cSize*KILOBYTE)- offset - log2(ways);
		tag = ADDRESSING - index - offset;
		
#ifdef DEBUG
		cout << "offset=" << offset << endl;
		cout << "index=" << index << endl;
		cout << "ways=" << ways << endl;
		cout << "tag=" << tag << endl;		
#endif


	}
	
	void cacheCreate()
	{	
#ifdef DEBUGCONCEPT
		int index_actual = 1 << index;/* total number of sets = 2^index */
		wayCell wayTest[index_actual][ways];/* array of way structures (index_actual X ways)*/
		/* dynamically allocating 2d array using vectors */
		//wayNumber.resize(index_actual, vector<wayCell>(ways));
		wayNumber.resize(3, vector<wayCell>(2));
		int count = 0;
		for(int i = 0; i < 3; i ++)
		{
			for(int j = 0; j < 2; j++){
				wayNumber[i][j].tag_w = count;
				count++;
			}
		}	
		/* Printing */
		for(int k = 0; k < 3; k ++)
		{
			for(int l = 0; l < 2; l++){
				cout << wayNumber[k][l].tag_w << " ";
			}
			cout << endl;
		}		
		cout<< "wayTest size = " << sizeof(wayTest) << endl;
		cout<< "wayNumber size = " << wayNumber.size() << endl;		
#else	
		/* dynamically allocating 2d array using vectors */
		int index_actual = 1 << index;/* total number of sets = 2^index */
		wayNumber.resize(index_actual, vector<wayCell>(ways));
		
		/* Initializing the 2d vector of wayCell type */
		for(int index_loop = 0; index_loop < index_actual; index_loop ++)
		{
			for(int way_loop = 0; way_loop < ways; way_loop++){
				wayNumber[index_loop][way_loop].address.set();/* address bitset assigning to 0xffffffff */
				wayNumber[index_loop][way_loop].tag_w.set();/* Tag bitset assigning to 0xffffffff */
				wayNumber[index_loop][way_loop].used = 0;/* valid-bit */
				wayNumber[index_loop][way_loop].offset_w = 0;/* offset */
			}
		}				
#endif

	}

#if 0	
	void updateAccessVariables(bitset<32> acTag, bitset<32> acIndex, bitset<32> acOffset, bitset<32> accessaddr)
	{/* updating the required access variables */
		accessTag 		= acTag;
		accessIndex 	= acIndex;
		accessOffset	= acOffset;
		accessAddress	= accessaddr;
	}
	/*+++Remove this function!! */
#endif
	
	void updateAccessVariables(bitset<32> addr)
	{
		string s1;
		accessAddress = addr; /* Assign new value of access address */
		
		s1= addr.to_string();
		accessOffset = bitset<32> (s1.substr((32-offset),offset));/* new  value of access offset */
		accessIndex = bitset<32> (s1.substr(tag,index));/* new  value of access index */
		accessTag = bitset<32> (s1.substr(0,tag));/* new  value of access Tag */

#ifdef	DEBUG
		cout << "accessAddress "<< accessAddress <<endl;
		cout << "accessOffset "<< accessOffset <<endl;
		cout << "accessIndex " << accessIndex <<endl;
		cout << "accessTag " << accessTag <<endl;
#endif		
#ifdef	DEBUGSIMPLE	
		cout << "accessAddress "<< hex << accessAddress.to_ulong() << "\t";
		cout << "accessTag " << hex << accessTag.to_ulong() << "\t";
		cout << "accessIndex "<< hex << accessIndex.to_ulong() << "\t";
		
#endif
	}

};

/*
	Class name	:	cacheReadWrite
	Purpose		:	To perform read and write operations on L1 and L2 cache	
	Testing		:	Not done				
 */    
class cacheReadWrite
{
	public:
	
	/*
		#define NA 0 // no action
		#define RH 1 // read hit
		#define RM 2 // read miss
		#define WH 3 // Write hit
		#define WM 4 // write miss

		NA - No Access
		RH - Read Hit
		RM - Read Miss
		WH - Write Hit
		WM - Write Miss
	*/
	int L1_status;/**/
	int L2_status;/**/
	int L1wayCounter;
	int L2wayCounter;
	
	void cacheReadWrite_initialize()
	{/* initialize way Counters */
		
		/* Initializing states to No access */
		L1_status = NA;
		L2_status = NA;		
		/* Intitialize waycounters */
		L1wayCounter = 0;
		L2wayCounter = 0;

	}
	
};
 
/* Function definitons */

/* Scan through ways */
int scanThroughSets(cache &Lcompare)
{	/* Before calling function check if tag and index to be found is updated in
		cache structure */
	int isize = Lcompare.ways;
	unsigned long u_compIndex = Lcompare.accessIndex.to_ulong();
	unsigned long u_compTag = Lcompare.accessTag.to_ulong();
	for(int i = 0; i < isize; i++)
	{/* Looks through all the ways of the current requested set to check for a tag match */	
		if(Lcompare.wayNumber[u_compIndex][i].tag_w.to_ulong() == u_compTag && 
			Lcompare.wayNumber[u_compIndex][i].used == 1)
		{
			/* returns 1 if successful */
			return 1;
		}
	}/* Else returns 0 on failure */
	return 0;
}

/* Add-only or Evict-and-add into Cache*/
bitset<32> updateCache(cache &Lcache, int &Counter)
{
	int evict_check = 1;
	bitset<32> evicted;
	evicted = Lcache.accessAddress;/* return current access address if no eviction has occured */
	int waySize = Lcache.ways;
	bitset<32> temp;
	unsigned long u_compIndex = Lcache.accessIndex.to_ulong();
	unsigned long u_compTag = Lcache.accessTag.to_ulong();


	
	/* Check to see if tag can be added */
	for(int i = 0; i < waySize; i++)
	{		
		if(Lcache.wayNumber[u_compIndex][i].used == 0)
		{	/* Space available in way */
			evict_check = 0;/* eviction need not occur. Space is available in way */
			Lcache.wayNumber[u_compIndex][i].tag_w = Lcache.accessTag;
			Lcache.wayNumber[u_compIndex][i].used = 1;/* To show that it is full */
			Lcache.wayNumber[u_compIndex][i].address = Lcache.accessAddress;
#ifdef DEBUGSIMPLE
			cout << "WNumb" << int(i) << "\t";
#endif
			break;
		}
	}
	
	/* check if data needs to be evicted */
	if(evict_check == 1)
	{/* If so then store evicted tag into "evicted" and update with new tag */
		/*+++checkpoint - which address is being evicted? */
		temp = evicted;
		evicted = Lcache.wayNumber[u_compIndex][Counter].address;/* address being kicked out */
		Lcache.wayNumber[u_compIndex][Counter].tag_w = Lcache.accessTag;
		Lcache.wayNumber[u_compIndex][Counter].address = temp;
#ifdef	DEBUGSIMPLE	
		cout << "Counter "<< Counter << "\t";
#endif	
		
		/* Increment way counter */
		/*+++checkpoint - Counter Overflow check */
		Counter = Counter + 1;
		if(Counter > (waySize - 1))
		{/* if counter has reached beyond the last available way, make it 0 */
			Counter = 0;
		}
		
	}
	
#ifdef	DEBUGSIMPLE	
		cout << endl;
#endif
	
	return evicted;
}	
 
/* masterRead */
void masterRead(cache &L1cache, cache &L2cache, cacheReadWrite &cacheCounter)
{
	int resultL1 = 0;
	int resultL2 = 0;
	bitset<32> evicted;
	bitset<32> curentAddressL2;
	
	
	/*	Possible read cases:
				L1_status	L2_status
		case1:		RH			NA
		case2:		RM			RH
		case3:		RM			RM
	*/
	
	//Scan L1; check for fail 
	/* +++checkpoint - see if the access variables are updated in L1 */
	resultL1 = scanThroughSets(L1cache);
	
	//Scan L2; if failed in L1 look in L2
	if(resultL1 == 0)
	{
		cacheCounter.L1_status = RM;/* update L1 status */
		/* +++checkpoint - see if the access variables are updated in L2 */
		resultL2 = scanThroughSets(L2cache);/* check in L2 */
		if(resultL2 == 0)
		{
			cacheCounter.L2_status = RM;/* update L2 status for case3 */
			
			//update tag in L2 if scan failed in scan(L2) according to L2wayCounter
			
			/*evicted = */updateCache(L2cache, cacheCounter.L2wayCounter);
			
			//dont worry about evicted block
		}
		else
		{	/* L2 hit */
			cacheCounter.L2_status = RH;/* update L2 status for case2 */
		}
		//update tag in L1 if scan failed in Scan(L1) according to L1wayCounter
		//curentAddressL2 = L1cache.accessAddress;/* Used to see if the data has been evicted */
		/*evicted =*/ updateCache(L1cache, cacheCounter.L1wayCounter);
		
		//if evicted then write evicted bitset into L2
		/*if(evicted.to_ulong() != curentAddressL2.to_ulong())*/
			/* +++checkpoint - see if the access variables are updated in L2 */
			/* store the access info into cache structure *//* +++possible screw up for offset */
			/*L2cache.updateAccessVariables(evicted);*/
			/*evicted = *//*updateCache(L2cache, cacheCounter.L2wayCounter);*/
			
			//dont worry of L2's evicted block
			
	}
	else
	{/* block found in L1 */
		/* updating the states of L1 and L2 for case1*/
		cacheCounter.L1_status = RH;
		cacheCounter.L2_status = NA;
	}		
}

// scan for write // 
int scanThroughSets_Write(cache &Lcompare)
{	/* Before calling function check if tag and index to be found is updated in
		cache structure */
	int isize = Lcompare.ways;
	unsigned long u_compIndex = Lcompare.accessIndex.to_ulong();
	unsigned long u_compTag = Lcompare.accessTag.to_ulong();
	for(int i = 0; i < isize; i++)
	{/* Looks through all the ways of the current requested set to check for a valid-bit match */	
		if(Lcompare.wayNumber[u_compIndex][i].used == 1)
		{
			/* returns 1 if successful */
			if(Lcompare.wayNumber[u_compIndex][i].tag_w.to_ulong() == u_compTag)

			return 1;    // MANOJ- might have to update address to deal with eviction // but NOP
		}
	}/* Else returns 0 on failure */
	return 0;
#ifdef DEBUG
		cout<< "in write scan"<<endl;
#endif	
}
// MASTER WRITE // 
void masterWrite(cache &L1cache, cache &L2cache, cacheReadWrite &cacheCounter) {
	int resultL1 = 0;
	int resultL2 = 0;
	bitset<32> evicted;
	bitset<32> curentAddressL2;

	resultL1 = scanThroughSets(L1cache);
#ifdef DEBUG
		cout<< "hit/miss in L1 for write " << resultL1 << endl;
#endif
	if(resultL1==0) {
		cacheCounter.L1_status = WM;/* update L1 status */
		/* +++checkpoint - see if the access variables are updated in L2 */
		resultL2 = scanThroughSets(L2cache);/* check in L2 */
		if(resultL2==0){

				cacheCounter.L2_status = WM;/* update L2 status for case3 */
		}
		else
		{	/* L2 hit */
			cacheCounter.L2_status = WH;/* update L2 status for case2 */

		}

	}
	else
	{/* block found in L1 */
		/* updating the states of L1 and L2 for case1*/
		cacheCounter.L1_status = WH;
		cacheCounter.L2_status = NA;
	}	


}



int main(int argc, char* argv[]){
 
    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
#ifdef	DEBUGFILE
	cache_params.open("cacheconfig.txt");
#else
    cache_params.open(argv[1]);
#endif

    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;              
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;              
      cache_params>>cacheconfig.L2blocksize;           
      cache_params>>cacheconfig.L2setsize;        
      cache_params>>cacheconfig.L2size;
      }
    
	cache L1;
	cache L2;
#ifdef DEBUG
	cout<< "L1 info" << endl;
#endif
	L1.cacheCalc(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size);
	L1.cacheCreate();
#ifdef DEBUG
	cout<< "L2 info" << endl;
#endif
	L2.cacheCalc(cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);
	L2.cacheCreate();
	
	// Implement by you: 
	// initialize the hirearch cache system with those configs
	// probably you may define a Cache class for L1 and L2, or any data structure you like
   
	/* Create variables needed for keeping track of the counters for Round-Robin */
	cacheReadWrite cacheCounter;
	cacheCounter.cacheReadWrite_initialize();
	int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
	int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
      
    ifstream traces;
    ofstream tracesout;
    string outname;
 
#ifdef	DEBUGFILE
	outname = "trace.out";
	traces.open("trace.txt");
#else
    outname = string(argv[2]) + ".out";
	traces.open(argv[2]);
#endif    
	
    tracesout.open(outname.c_str());
    
    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;        
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
	
    if (traces.is_open()&&tracesout.is_open()){    
        while (getline (traces,line)){   // read mem access file and access Cache
            
            istringstream iss(line); 
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
#if 0
            s1= accessaddr.to_string();
    		bitset<32> accessaddr_offset1 = bitset<32> (s1.substr((32-L1.offset),L1.offset));
    		bitset<32> accessaddr_offset2 = bitset<32> (s1.substr((32-L2.offset),L2.offset));
    		bitset<32> accessaddr_index1 = bitset<32> (s1.substr(L1.tag,L1.index));
    		bitset<32> accessaddr_index2 = bitset<32> (s1.substr(L2.tag,L2.index));
    		bitset<32> accessaddr_tag1 = bitset<32> (s1.substr(0,L1.tag));
    		bitset<32> accessaddr_tag2 = bitset<32> (s1.substr(0,L2.tag));
#endif

			/* store the access info into cache structure */
          	L1.updateAccessVariables(accessaddr);			
			L2.updateAccessVariables(accessaddr);
			
           // access the L1 and L2 Cache according to the trace;
              if (accesstype.compare("R")==0)
              
             {    
			 
				//Implement by you:
				// read access to the L1 Cache, 
				//  and then L2 (if required), 
                
				/* masterRead */
				/* +++Start writing again from here */
				masterRead(L1, L2, cacheCounter);
				
				//  update the L1 and L2 access state variable;
				L1AcceState = cacheCounter.L1_status;
				L2AcceState = cacheCounter.L2_status;
#ifdef DEBUGSIMPLE
				cout << " "<<endl;
#endif				 
				 
                 }
             else 
             {    
                   //Implement by you:
                  // write access to the L1 Cache, 
                  //and then L2 (if required), 
                  //update the L1 and L2 access state variable;
             	masterWrite(L1, L2, cacheCounter);
             	L1AcceState = cacheCounter.L1_status;
				L2AcceState = cacheCounter.L2_status;
                                   
                  
                  }
              
              
             
            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
             
             
        }
        traces.close();
        tracesout.close(); 
    }
    else cout<< "Unable to open trace or traceout file ";


   
    
  

   
    return 0;
}

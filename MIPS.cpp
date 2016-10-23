#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.
#define DEBUGGING


class Split
{
public:
		bitset<5> rd1,rd2,wr;
		bitset<16> imm;
		bitset<26> for_con;
		std::string s1;
		std::string M1;
		std::string M2;
		std::string M3;
		std::string M4;
		std::string M5;
				
		void divide (bitset<32> W_Data) {
			s1 = W_Data.to_string();

			std::string d2_sub = s1.substr(29,3);/*---changed the order - to read from big endian input */
			bitset<3> d2_sub_set(d2_sub);				
			unsigned long decodeValue = d2_sub_set.to_ulong();
		
			M1 = s1.substr(6,5);//(25,5);
			M2 = s1.substr(11,5);//(20,5);
			M3 = s1.substr(16,5);//(25,5);
			M4 = s1.substr(16,16);//(15,16);/* +++ check again */ // should work
			M5 = s1.substr(6,26);//(25,26);/* +++ check again */ // should work
		
			rd1= bitset<5>(M1);
			rd2= bitset<5>(M2);
			wr = bitset<5>(M3);
			imm = bitset<16>(M4);	// immediate value 
			for_con = bitset<26>(M5); // concat value
/*
			cout << " rd1 ";	
			cout << rd1 << endl;			
			cout << " rd2 ";	
			cout << rd2 << endl;	
			cout << " wr ";	
			cout << wr << endl;	
			cout << " imm ";	
			cout << imm << endl;
//			cout << " for_con ";	
//			cout << for_con << endl;			
*/			
		}
};

class RF
{
    public:
        bitset<32> ReadData1, ReadData2; 
     	RF()
    	{ 
          Registers.resize(32);  
          Registers[0] = bitset<32> (0);/* Sets register 0 to 0 */  
        }

        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {   
        	//int a = 8;
			
            // implement the funciton by you.
			if(WrtEnable == 1)
			{/* Writing the data into register */
				
				Registers[WrtReg.to_ulong()] = WrtData;
				cout << "Registers[WrtReg.to_ulong()]";
				cout << Registers[WrtReg.to_ulong()] << endl;
				
			}
			else
			{/* Reading registers */
				ReadData1 = Registers[RdReg1.to_ulong()];
				ReadData2 = Registers[RdReg2.to_ulong()];

			}
			
         }
		 
	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {        
                        rfout << Registers[j]<<endl;
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               
               }     
	private:
            vector<bitset<32> >Registers;
	
};


class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {   
                 // implement the ALU operations by you. 
				 long function = ALUOP.to_ulong();
				 int op1 = (int)oprand1.to_ulong();
				 int op2 = (int)oprand2.to_ulong();
				 int result = 0;

				 switch(function)
				 {
					case ADDU:
						result = op1 + op2;
						break;
					case SUBU:
						result = op1 - op2;
						break;
					case AND:
						result = op1 & op2;
						break;
					case OR:
						result = op1 | op2;
						break;
					case NOR:
						result = ~(op1 | op2);
						break;
					default:
						break;
				 }
				 
				std::bitset<32> z(result);
				ALUresult = z;/*+++ just keeping the value in ALUresult */
                return z;
               }            
};


class INSMem
{
      public:
          bitset<32> Instruction;
          string word_array[20];
          string word;
      
          INSMem()
          {       IMem.resize(MemSize); 
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {      
                        IMem[i] = bitset<8>(line);
                        word_array[i]=line; // store all instructions into string
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
            string ReadMemory (bitset<32> ReadAddress) 
			{    
				// implement by you. (Read the byte at the ReadAddress and the following three byte).

				word= word_array[ReadAddress.to_ulong()] + word_array[ReadAddress.to_ulong() + 1] + word_array[ReadAddress.to_ulong() + 2] + word_array[ReadAddress.to_ulong() + 3];
				
				//std::bitset<8> bit(std::string(word.rbegin(), bin.rend()));
				//Instruction= bitset<32>(string(word.rbegin(),word.rend()));/* ---reversing order */
				
				//Instruction= bitset<32>(word);
				//return Instruction;     
				return word;
			}     

      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
	public:
		bitset<32> readdata;  
		DataMem()
		{
		DMem.resize(MemSize); 
		ifstream dmem;
			  string line;
			  int i=0;
			  dmem.open("dmem.txt");
			  if (dmem.is_open())
			  {
			  while (getline(dmem,line))
				   {      
					DMem[i] = bitset<8>(line);
					i++;
				   }
			  }
			  else cout<<"Unable to open file";
			  dmem.close();
	  
		}  
		bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
		{    
			cout<<"Address MemAccess = ";
			cout<<Address << endl;
			//implement by you.
			/* If writemem is enabled then store the WriteData into Address */
			if (writemem == 1)/*+++ Can it do both write into and read from an address? */
			{
				/* store WriteData into Address */
				WriteDataMemory (Address, WriteData); 
			}
			/* else if readmem is enabled then send the data in Address back to readdata */
			else if(readmem == 1)
			{
				readdata = ReadDataMemory (Address);
				/* store contents of Address into readdata */
			}

			return readdata;     
		
		
		}   

		bitset<32> ReadDataMemory (bitset<32> ReadAddress) 
		{    
			// implement by you. (Read the byte at the ReadAddress and the following three byte).
			
			long base_address = ReadAddress.to_ulong();
			//bitset<8> temp = DMem[base_address];
			//std::string b_a_0 = temp.to_string();
			std::string b_a_0 = DMem[base_address].to_string();
			std::string b_a_1 = DMem[base_address + 1].to_string();
			std::string b_a_2 = DMem[base_address + 2].to_string();
			std::string b_a_3 = DMem[base_address + 3].to_string();
			readdata = bitset<32>(b_a_0 + b_a_1 + b_a_2 + b_a_3);
			

			return readdata;     
		} 

		void WriteDataMemory (bitset<32> WriteAddress, bitset<32> WriteData) 
		{    
			// Put WriteData into DMem file
			long base_address = WriteAddress.to_ulong();
			std::string WriteString = WriteData.to_string();
			std::string temp;			
			/* Write into the DMem file */
			temp = WriteString.substr(0,8);
			DMem[base_address] = bitset<8>(temp);/* +++ check again */
			temp = WriteString.substr(8,8);
			DMem[base_address + 1] = bitset<8>(temp);
			temp = WriteString.substr(16,8);
			DMem[base_address + 2] = bitset<8>(temp);
			temp = WriteString.substr(24,8);
			DMem[base_address + 3] = bitset<8>(temp);
    
		} 		
		  
		void OutputDataMem()
		{
			ofstream dmemout;
			dmemout.open("dmemresult.txt");
			if (dmemout.is_open())
			{
			for (int j = 0; j< 1000; j++)
			   {     
				dmemout << DMem[j]<<endl;
			   }
			 
			}
			else cout<<"Unable to open file";
			dmemout.close();

		}             
      
      private:
           vector<bitset<8> > DMem;
      
};  

class mux
{
	public:
		bitset<32> mux_out2_1, mux_out3_1;
		bitset<5> mux_out_5bit;
		bitset<32> multiplexer2_1(bitset<32> input1, bitset<32> input2, bitset<1> control) {
            long d_control= control.to_ulong();
			if(d_control ==0)
				mux_out2_1= input1;
			else
				mux_out2_1=input2;

//			cout << " mux_out2_1  = ";	
//			cout << mux_out2_1 << endl;				
			return mux_out2_1;/*+++ changed */
		}

		bitset<32> multiplexer3_1(bitset<32> input0, bitset<32> input1, bitset<32> input2, bitset<2> NextPC){
            long d_NextPC= NextPC.to_ulong();
			if(d_NextPC==0) { //pc=pc+4
				mux_out3_1= input0;
			}
			else if (d_NextPC==1){ //I-jump
				mux_out3_1= input1;
			}
			else if (d_NextPC==2){ // J type
				mux_out3_1= input2;
			}

			cout << " mux_out3_1  = ";	
			cout << mux_out3_1 << endl;	
		
			
			return mux_out3_1;
		}
    bitset<5> multiplexer_5bit(bitset<5> input0, bitset<5> input1, bitset<1> control){
        
        long d_control= control.to_ulong();
        if(d_control==1) // I-type=1
            mux_out_5bit= input0;
        else
            mux_out_5bit= input1;
/*
		cout << " mux_out_5bit  = ";	
		cout << mux_out_5bit << endl;			
*/
        return mux_out_5bit;
    }

};

class Decoder
{
	public:
			int d_IsLoad,d_IsStore,d_I_Type,d_ALUOP,d_WrtEnable,d_MemWrtEnable;
			int	d_NextPC;
            bitset<1> IsLoad,IsStore,I_Type,WrtEnable,MemWrtEnable,isEQ;
            bitset<2> NextPC;
            bitset<3> ALUOP;
			unsigned long Control_Signal(bitset<32> Instruction) {
				std::string d1 = Instruction.to_string();
				std::string d1_sub = d1.substr(0,6);/*---changed the order - to read from big endian input */
				bitset<6> d1_sub_set(d1_sub);
				
				/* for R-type */
				std::string d2_sub = d1.substr(29,3);/*---changed the order - to read from big endian input */
				bitset<3> d2_sub_set(d2_sub);				
				
				unsigned long decodeValue = d1_sub_set.to_ulong();
/*
				cout<<"d1_sub_set =";
				cout<<d1_sub_set <<endl;
*/				
				

                if(decodeValue == 0x23) {
					d_IsLoad= 1;
                    IsLoad= bitset<1> (d_IsLoad);
					//cout<<IsLoad<<endl;
                }
				else 
				{
					d_IsLoad= 0;
                    IsLoad= bitset<1> (d_IsLoad);					
					cout<<"IsLoad";
					cout<<IsLoad<<endl;
				}
				
                if (decodeValue == 0x2b){
					d_IsStore= 1;
                    IsStore= bitset<1> (d_IsStore);
                }
				else{
					d_IsStore= 0;
                    IsStore= bitset<1> (d_IsStore);
                }
				
                if ((decodeValue==0x00 ) || (decodeValue == 0x02) || (decodeValue == 0x3f)){
					/* R-type or J-type */
					d_I_Type= 0;
                    I_Type= bitset<1> (d_I_Type);
					cout<<"0Itype= "<<I_Type<<decodeValue<<endl;
                }
				else
				{
					d_I_Type= 1;
                    I_Type= bitset<1> (d_I_Type);
					cout<<"1Itype= "<<I_Type<<endl;
				}
				
                if (decodeValue== 0x09 || decodeValue== 0x23 || decodeValue== 0x2b){
					/* Addiu, load, store */
					d_ALUOP= 1;
                    ALUOP= bitset<3> (d_ALUOP);
                }
                else if(decodeValue== 0x00)
				{
                    ALUOP= d2_sub_set;
                }
				
                if ((decodeValue== 0x04) || (decodeValue== 0x02) || (decodeValue== 0x3f) || (decodeValue== 0x00) || (decodeValue== 0x2B) || (decodeValue== 0x09)){
					/* branch, or j-type or R-type or Store */
					
					d_WrtEnable= 0;
                    WrtEnable= bitset<1> (d_WrtEnable);
					cout<<"WR1 WrtEnable = "<<WrtEnable<<endl;
                }
                else if((decodeValue == 0x23))
				{	/* Load */
					
					d_WrtEnable= 1;
                    WrtEnable= bitset<1> (d_WrtEnable);
					cout<<"WR2 WrtEnable = "<<WrtEnable<<endl;
                }
				
                if(decodeValue== 0x02) {//jump
					d_NextPC= 1;
                    NextPC= bitset<2> (d_NextPC);
                }
                else if(decodeValue== 0x04 && isEQ==1) {//beq
					d_NextPC= 2;
                    NextPC= bitset<2> (d_NextPC);
                }
                else{
					d_NextPC= 0;
                    NextPC= bitset<2> (d_NextPC);
                }
				cout<<"WR3 WrtEnable = "<<WrtEnable<<endl;
			return decodeValue;
			}
};

class concat  
{
	public:
		bitset<32> concat_value;
 		// concat // 
		bitset<32> Concat_Op(bitset<26> instrn, bitset<32> msb){
			std:: string s1,s2;
			s1= instrn.to_string();
			s2= msb.to_string();

			string Next_PC= s2[0,4] + s1 + "00"; // should convert next pc to string too
			concat_value = bitset<32> (Next_PC);
//			cout << "indecoder concat_value = ";	
//			cout << concat_value << endl;
			return concat_value;
		}

		//// Sign Extention //// 
		bitset<32> ExtendedValue, ExtendedValue_Adder;

		void Extension (bitset<16> input1){
			string s1= input1.to_string();
			string s2;
			if(s1.substr(15,1)=="1"){
				string ones = "1111111111111111";
				s1.insert(0,ones);/* prepending ones */
				ExtendedValue = bitset<32> (s1);
				s2= s1.substr(29,29);
				s2.append("00");
				ExtendedValue_Adder= bitset<32> (s2);
			}
			else{
				string zeroes = "0000000000000000";
				s1.insert(0,zeroes);/* prepending zeroes */
				ExtendedValue = bitset<32> (s1);
				s2= s1.substr(29,29);
				s2.append("00");
				ExtendedValue_Adder= bitset<32> (s2);				
				
			}
/*
			cout << " ExtendedValue  = ";	
			cout << ExtendedValue << endl;	
			cout << " ExtendedValue_Adder  = ";	
			cout << ExtendedValue_Adder << endl;			
*/
		}
};

class adder
{
	public:
	bitset<32> output;
	bitset<32> Adding(bitset<32> input1, bitset<32> input2){

		unsigned long d_output= input1.to_ulong()+ input2.to_ulong();
		output= bitset<32> (d_output);
//		cout << "adder op" <<endl;
//		cout << output << endl;
		return output;
	}
};

class PC_keep
{/* To keep track of PC */
	public:
		std::bitset<32> PC;
		PC_keep()
		{
			std::bitset<32> x(0);
			PC = x;
		}
};

   
int main()
{
    RF myRF;
	PC_keep myPC;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;
    Decoder myDecoder;
	mux mymux;
	adder myadder;
	concat myconcat;  
    //std::bitset<32> PC (0);
    std::bitset<32> write_data (0);
	std::bitset<32> Next_PC (0);
	std::bitset<32> N_PC(0);
	std::string temp;
	bitset<32> instrn;
	int loop = 0;
	unsigned long decodeValue = 0;
	std::bitset<32> con(0);
	std::bitset<32> I_jump_addr(0);
	std::bitset<1> isEQ(0);
	std::bitset<5> muxOUT_5bit(0);
	std::bitset<32> M_Out(0);
	std::bitset<32> ALU_Result(0);
	std::bitset<32> data(0);

	
    Split mySplit; 

    while (1)
	{
		/* +++ Before fetching the next instruction should we reset all the flags back to their defaults? */
        // Fetch
		cout << "loop = ";	
		cout << loop << endl;
		
        temp = myInsMem.ReadMemory(myPC.PC);
        bitset<32>x(temp);
		instrn = x;

#ifdef DEBUGGING
		cout << "myPC.PC = ";	
		cout << myPC.PC << endl;	
		cout << "instrn = ";	
		cout << instrn << endl;			
#endif		
			
        unsigned long d_instrn= instrn.to_ulong();
		// If current insturciton is "11111111111111111111111111111111", then break; //
		if((d_instrn == 0xffffffff) || (myPC.PC.to_ulong() > long(MemSize))) // might have to add an else to increment pc
			
		break;


		// decoder// 
        decodeValue = myDecoder.Control_Signal(instrn);
#ifdef DEBUGGING
        cout << "myDecoder.IsLoad";
		cout << myDecoder.IsLoad << endl;		
		cout << "myDecoder.IsStore";
		cout << myDecoder.IsStore << endl;		
		cout << "myDecoder.I_Type";
		cout << myDecoder.I_Type << endl;		
		cout << "myDecoder.WrtEnable";
		cout << myDecoder.WrtEnable << endl;		
        cout << "myDecoder.NextPC";
		cout << myDecoder.NextPC << endl;		
        cout << "myDecoder.ALUOP";
		cout << myDecoder.ALUOP << endl;			
#endif
		mySplit.divide(instrn);/* Splits the instruction into required fields: imm, rd1, rd2, wrt etc., */

		// Sign extend // 
		myconcat.Extension(mySplit.imm);
		
		// pc=pc+4
		N_PC= myadder.Adding(myPC.PC,4); // binary of 4(may be) check

		// concat for J-Type //
		con= myconcat.Concat_Op(mySplit.for_con, N_PC);
#ifdef DEBUGGING
        cout << "mySplit.for_con";
		cout << mySplit.for_con << endl;		
		cout << "N_PC";
		cout << N_PC << endl;			
		
#endif
		

		// I-jump adder//
		I_jump_addr= myadder.Adding(N_PC, myconcat.ExtendedValue_Adder);


		
#ifdef DEBUGGING
        cout << "myPC.PC";
		cout << myPC.PC << endl;				
		// decode(Read RF)

		cout<< "mySplit.rd2 =";
		cout<< mySplit.rd2 << endl;		
		cout<< "mySplit.wr =";
		cout<< mySplit.wr << endl;		
		cout<< "myDecoder.I_Type =";
		cout<< myDecoder.I_Type << endl;
#endif

		
		muxOUT_5bit= mymux.multiplexer_5bit(mySplit.rd2, mySplit.wr, myDecoder.I_Type);/* +++ */
#ifdef DEBUGGING
/*
		cout<< "mySplit.rd1 =";
		cout<< mySplit.rd1 << endl;
		cout<< "mySplit.rd2 =";
		cout<< mySplit.rd2 << endl;				
		cout<< "write_data =";
		cout<< write_data << endl;	
		cout<< "myDecoder.WrtEnable =";
		cout<< myDecoder.WrtEnable << endl;
*/		
#endif
												
		myRF.ReadWrite(mySplit.rd1, mySplit.rd2, muxOUT_5bit, write_data, myDecoder.WrtEnable);
#ifdef DEBUGGING
		cout<< "myRF.ReadData1 =";
		cout<< myRF.ReadData1 << endl;
		cout<< "myRF.ReadData2 =";
		cout<< myRF.ReadData2 << endl;
#endif		
		// Execute

		/* Generating the isEQ flag */
		if((myRF.ReadData1.to_ulong() - myRF.ReadData2.to_ulong())==0)
			myDecoder.isEQ= 1;	
#ifdef DEBUGGING
		cout<< "isEQ Main=";
		cout<< isEQ << endl;
	
#endif		
		/* Calling Decoder again for BEQ */
		decodeValue = myDecoder.Control_Signal(instrn);

		// mux // 
		/*+++ Decoder module has to be changed */
		myPC.PC= mymux.multiplexer3_1(N_PC, con, I_jump_addr, myDecoder.NextPC);
#ifdef DEBUGGING
        cout << "N_PC=";
		cout << N_PC << endl;
        cout << "I_jump_addr=";
		cout << I_jump_addr << endl;
        cout << "con=";
		cout << con << endl;
        cout << "myDecoder.NextPC=";
		cout << myDecoder.NextPC << endl;		
#endif		
		
		M_Out= mymux.multiplexer2_1(myRF.ReadData2, myconcat.ExtendedValue,myDecoder.I_Type);
#ifdef DEBUGGING
/*		cout<< "myRF.ReadData2 =";
		cout<< myRF.ReadData2 << endl;
		cout<< "myconcat.ExtendedValue =";
		cout<< myconcat.ExtendedValue << endl;		
		cout<< "myDecoder.I_Type =";
		cout<< myDecoder.I_Type << endl;	
*/		cout<< "M_Out =";
		cout<< M_Out << endl;
#endif		


		
		ALU_Result= myALU.ALUOperation(myDecoder.ALUOP, myRF.ReadData1, M_Out);
#ifdef DEBUGGING
/*		cout<< "myRF.ReadData1 =";
		cout<< myRF.ReadData1 << endl;
		cout<< "M_Out =";
		cout<< M_Out << endl;
*/

		cout<< "myDecoder.ALUOP=";
		cout<< myDecoder.ALUOP << endl;

		cout<< "ALU_Result=";
		cout<< ALU_Result << endl;		
#endif		

		
		
		
		
		// Read/Write Mem
		data= myDataMem.MemoryAccess(ALU_Result,myRF.ReadData2,myDecoder.IsLoad,myDecoder.IsStore);
#ifdef DEBUGGING
		cout<< "data =";
		cout<< data << endl;		
#endif
		// Write back to RF
		write_data= mymux.multiplexer2_1(ALU_Result, data, myDecoder.IsLoad);
#ifdef DEBUGGING 
 /*
		cout << "ALU_Result = ";	
		cout << ALU_Result << endl;	
        cout << "myDecoder.IsLoad = ";	
		cout << myDecoder.IsLoad << endl;			
*/
        cout << "write_data = ";	
		cout << write_data << endl;
#endif	
		if(decodeValue== 0x00 || decodeValue== 0x09)
		{/* For r-types, first read the register data then after comuting the value write back into
			registers */
			myDecoder.WrtEnable = 1;
			cout<< "decodeValue Main = "<< decodeValue <<endl;	
		}
#ifdef DEBUGGING
		cout<< "muxOUT_5bit =";
		cout<< muxOUT_5bit << endl;
		cout<< "myDecoder.WrtEnable = "<< myDecoder.WrtEnable <<endl;
		cout<< "write_data = "<< write_data <<endl;	
#endif		
		myRF.ReadWrite(mySplit.rd1, mySplit.rd2, muxOUT_5bit, write_data, myDecoder.WrtEnable);
 #ifdef DEBUGGING
		cout << "mySplit.rd1 = ";	
		cout << mySplit.rd1 << endl;			
        cout << "mySplit.rd2";	
		cout <<mySplit.rd2 << endl;
        cout << "muxOUT_5bit";			
		cout <<muxOUT_5bit << endl;
		cout << "write_data";	
		cout <<write_data << endl;
		
 		cout << "myDecoder.WrtEnable";	
		cout <<myDecoder.WrtEnable << endl;       

#endif		
		
        myRF.OutputRF(); // dump RF;
		loop = loop + 1;
		
    }
        myDataMem.OutputDataMem(); // dump data mem
      
        return 0;
        
}

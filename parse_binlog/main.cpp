#include <iostream>
#include <vector>
#include <thread>
#include <map>
#include <set>
#include <atomic>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <assert.h>
#include "Binlog.h"
#pragma pack(1)

using std::cin;
using std::cout;
using std::endl;





/* run this program using the console pauser or add your own getch, system("pause") or input loop */

std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;
void print_threadid() {
	while (lock_stream.test_and_set()) {}
	std::cout << "Hello . I am Thread " << std::this_thread::get_id() << endl;
	lock_stream.clear();
}


std::map<std::string, int> GetWordCountMap(const std::string& filename) {
	std::map<std::string, int> Map_Word_Count;
	std::fstream file(filename);
	std::string text;
	while (getline(file, text)) {
		std::istringstream line(text);
		std::string word = "";
		while (line >> word) {
			if (!Map_Word_Count[word])
				Map_Word_Count[word] = 0;
			Map_Word_Count[word]++;
		}
	}
	return  Map_Word_Count;
}

struct student {
	char name[20];
	int num;
	int age;
	char sex;
};

// left value 

int &getNum(int &a) {
	return a;
}
/*
int ia=3,ib=0;
getNum(ia)=4;
cout<<ia<<endl;*/


std::atomic<int> ai(0);

extern int thread_id;
int main(int argc, char** argv) {
	/*constexpr int assert_var=1;
	assert(assert_var);
	#ifndef NDEBUG
	std::cerr<<"function name : "<<__func__<<endl;
	#endif*/
	auto getTrailingReturnType(int lrs) -> int(*)[10];

	int a = 1;
	decltype(a) b = 1;
	std::cerr << "error \n";

	if (1 == 1) {

		/*	thread_id=10;
		std::vector<int> vi={1,2,3,4,5};
		std::cout<<vi.back()<<std::e
		ndl;
		Animal an1=Animal("sky",26);

		an1.hello();
		Animal an2=Animal("sky2",26);
		an2.hello();
		print(an2);
		*/
		/*	ai.store(2,std::memory_order_relaxed);
		cout<<ai<<" "<<ai.is_lock_free()<<endl;

		print_threadid();
		std::vector<std::thread> vecthd;
		for(int i=0;i<10;i++)
		vecthd.push_back(std::thread(print_threadid));
		for(auto &th:vecthd)
		th.join();

		std::array<int ,10> ai={1,2,3,4,5,6,7,8,9,10};
		cout<<boost::format("a1 is %d")%ai[1]<<endl;

		*/
		/*	 std::string filename;
		std::cout<<"Please input filname:"<<std::endl;
		std::cin>>filename;
		std::map<std::string ,int> WordCountResult=GetWordCountMap(filename);
		std::string word;
		std::cout<<"Please input the word to find:"<<std::endl;
		typedef std::map<std::string ,int>::value_type  mi;
		while(std::cin>>word){
		if(word=="exit" ){
		typedef std::function<bool(std::pair<std::string, int>, std::pair<std::string, int>)> Comparator;

		Comparator compFunctor =
		[](std::pair<std::string, int> elem1 ,std::pair<std::string, int> elem2)
		{
		return elem1.second > elem2.second;
		};


		std::set<std::pair<std::string, int>, Comparator> setOfWords(
		WordCountResult.begin(), WordCountResult.end(), compFunctor);

		for (auto word_count=setOfWords.cbegin();word_count!=setOfWords.cend();word_count++)
		std::cout<<"word "<<word_count->first<<" appears "<<word_count->second<<(word_count->second>1?" times":" time")<<endl;
		break;
		}
		std::cout<<"word "<<word<<" appears "<<WordCountResult[word]<<(WordCountResult[word]>1?" times":" time")<<endl;
		}
		*/
		/*  student stud[3]={"Li",1001,18,'f',"Fun",1002,19,'m',"Wang",1004,17,'f'};
		std::ofstream outfile("stud.dat",std::ofstream::binary);
		if(!outfile)
		{
		std::cerr<<"open error!"<<std::endl;
		abort( );//ÍË³ö³ÌÐò
		}
		for(int i=0;i<3;i++)
		outfile.write((char*)&stud[i],sizeof(stud[i]));
		outfile.close( );*/
	}

	/*  t;
	uint8_t  in[4]={1,252,253,255};
	for(auto &a:in){
	UnPackedLength(a);
	}
	*/


	std::ifstream  infile("mysql-bin.000490", std::ifstream::binary);
	int magn;
	// cout<<sizeof(magn)<<endl; 4
	infile.read((char *)&magn, 4);
	assert(infile.is_open());
	cout << "Magic_Number: " << magn << endl;
	for (int i = 0; i <= 10; i++) {
		BinlogEventHeader MID(infile);
		if (MID.type_code == FORMAT_DESCRIPTION_EVENT) {
			FormatDescriptionEvent FDE(MID, infile);
			FDE.PrintFormatDescriptionEventData();
		}
		else if (MID.type_code == TABLE_MAP_EVENT) {
			TableMapBinlogEvent TME(MID, infile);
			TME.PrintTableMapBinlogEventtData();
		}

		infile.seekg(MID.next_position);
	}
	infile.close();


	return 0;
}
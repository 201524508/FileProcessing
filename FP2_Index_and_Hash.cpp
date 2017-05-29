#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "LinkedList.h"
using namespace std;

/* block size = 4k bytes(4096 bytes), record size = 32 bytes */

struct Student {
	char name[20];
	unsigned studentID;	//key
	float score;	//for indexing
	unsigned advisorID;
};

class HashTable {
private:
	int key;	//studentID
	LinkedList **table;	//LinkedList를 배열의 원소로 설정
	int numItem;
	int lengthArray;
public:
	HashTable();
	~HashTable();
	void makeDBfile(ifstream& inp);
	void setHashTable();
	void insertItemHash(Item *p, int b);
	Item* getItem_Key(unsigned k);
	void printTable();
	int hashFunc(unsigned k);
	int getNumItem();
};

HashTable::HashTable() {
	key = -1;
	table = NULL;
	numItem = 0;
	lengthArray = 0;
}

HashTable::~HashTable() {
	delete[] table;
}

void HashTable::insertItemHash(Item *p, int b) {
	table[b]->insertItemList(p);
}

int HashTable::hashFunc(unsigned k) {
	int result;

	result = k % 32;

	return result;
}

Item* HashTable::getItem_Key(unsigned k) {	//하나만 나옴..?
	int t = hashFunc(k);	//리스트 배열 인덱스
	int n = table[t]->getNum();
	Item *tmp = table[t]->getItem(k);	//LinkedList의 first Item

	for (int i = 0; i < n; i++) {
		if (tmp->getKey() == k) {
			return tmp;
		}
		else {
			while (tmp->getNext() != NULL) {
				if (tmp->getKey() == k) {
					return tmp;
				}
				tmp = tmp->getNext();
			}
		}
	}
	cout << "no matched item\n";
	return NULL;
}

void HashTable::printTable() {
	for (int i = 0; i < lengthArray; i++) {
		cout << "[" << i << "번째 Hash List]\n";
		table[i]->printList();
	}
}

int HashTable::getNumItem() {
	int result = 0;
	for (int i = 0; i < lengthArray; i++) {
		result = result + table[i]->getTotNum();
	}

	return result;
}

void HashTable::setHashTable() {
	ifstream fp;
	fp.open("Students.DB", ios::binary);

	ofstream fp2;
	fp2.open("Students.hash", ios::binary);

	if (fp.is_open() != true || fp2.is_open() != true) {
		cout << "error\n";
		return;
	}

	if (numItem <= sizeof(Student)) {
		lengthArray = numItem;
	}
	else {
		lengthArray = sizeof(Student);
	}
	//cout << "lengthArray : " << lengthArray << endl;
	table = new LinkedList*[lengthArray];

	for (int i = 0; i < lengthArray; i++) {
		table[i] = new LinkedList();
	}

	for (int i = 0; i < numItem; i++) {
		Student std;
		fp.read((char*)&std, sizeof(std));
		//cout << std.name << " " << std.studentID << " " << std.score << " " << std.advisorID << endl;

		int b = hashFunc(std.studentID);

		Item *it = new Item(std.studentID, b);
		//cout << it->getKey() << endl;
		insertItemHash(it, b);
		//cout << "num of table" << b << ":" << table[b]->getNum() << endl;
	}

	printTable();

	for (int i = 0; i < numItem; i++) {
		fp2.write((char*)&table[i], sizeof(table[i]));
	}

	fp.close();
	fp2.close();
}

void HashTable::makeDBfile(ifstream& fp) {
	if (fp.is_open() != true) {
		cout << "error!!\n";
		return;
	}

	ofstream fp2;
	fp2.open("Students.DB", ios::binary);
	int num;
	string buffer;
	getline(fp, buffer);
	stringstream str(buffer);
	//str.str(buffer);
	str >> num;
	//cout << "num of datum : " << num<<endl;
	
	numItem = num;

	Student std;

	/* DB 만들기 */
	for (int i = 0; i < num; i++) {
		/* 1. Student 데이터를 input file에서 읽어와 타입 변환 */
		string buffer2;
		getline(fp, buffer2);
		//cout << buffer2 << endl;
		stringstream str2(buffer2);

		string wholename;
		string lastname;
		unsigned stdID;
		float sc;
		unsigned admID;

		str2 >> wholename >> lastname;

		char* tmp = new char[lastname.length() + 1];
		strcpy(tmp, lastname.c_str());

		char** ptr = new char*[4];
		ptr[0] = strtok(tmp, ",");
		lastname = ptr[0];

		wholename = wholename + " " + lastname;
		//cout << wholename << endl;

		int j = 0;
		while (ptr[j] != NULL) {
			//printf("%s\n", ptr[j]);
			j++;
			ptr[j] = strtok(NULL, ",");
		}

		stdID = atoi(ptr[1]);
		//cout << stdID << endl;
		sc = atof(ptr[2]);
		//cout << sc << endl;
		admID = atoi(ptr[3]);
		//cout << admID << endl;

		/* 2. 타입 변환된 데이터를 Student struct 배열에 저장 */
		strcpy(std.name, wholename.c_str());
		std.studentID = stdID;
		std.score = sc;
		std.advisorID = admID;

		/* 3. 저장된 struct를 binary output file에 저장 */
		/* 블럭에 대한 수정 필요...
		블럭 처리 어떻게 하지...하...
		블럭을 배열로 만들ㄹ..?
		Student 타입 배열로..? 어떻게..? 4000 / 32 로 블럭사이즈 만들어야 됨? 어케야됨? ㅅㅂ?
		*/
		fp2.write((char*)&std, sizeof(std));
	}

	fp2.close();
}

int main() {
	ifstream fp;
	fp.open("sampleData.csv");

	HashTable hash;

	hash.makeDBfile(fp);
	hash.setHashTable();
	fp.close();

	return 0;
}
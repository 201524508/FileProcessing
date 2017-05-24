#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Student {
	char name[20];
	unsigned studentID;	//key
	float score;	//for indexing
	unsigned advisorID;
} std;

class HashTable {

};

int main() {
	ifstream fp;
	fp.open("Student.DB");

	return 0;
}
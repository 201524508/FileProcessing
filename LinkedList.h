#include <iostream>
using namespace std;

class Item {
private:
	Item *next;
	unsigned key;
	int bucketNum;
public:
	Item() {
		next = NULL;
		key = 0;
		bucketNum = -1;
	}
	Item(unsigned k, int bN) {
		next = NULL;
		key = k;
		bucketNum = bN;
	}
	Item *getNext() {
		return next;
	}
	void setNext(Item *p) {
		next = p;
	}
	void setKey(unsigned k) {
		key = k;
	}
	unsigned getKey() {
		return key;
	}
	void setBucketNum(int bN) {
		bucketNum = bN;
	}
	int getBucketNum() {
		return bucketNum;
	}
};

class LinkedList {	//총 크기 = 4k 바이트 (4096)
private:
	Item *first;	//Item 크기 = 12 바이트 (12)
	int numItem;
	LinkedList *next;	//overflow가 될 경우를 대비한 next 버킷 포인터
public:
	LinkedList() {	//컨스트럭터
		first = NULL;
		numItem = 0;
		next = NULL;
	}
	~LinkedList() {	 //디스트럭터
		deleteList(first);
		if (next != NULL) {
			delete[] next;
		}
	}
	void insertItemList(Item *p);	//p 넣기
	void deleteList(Item *t) {	//디스트럭터 함수
		if (t == NULL) {
			delete t;
		}
		else {
			deleteList(t->getNext());
			delete t;
		}
	}
	Item* getItem(unsigned k);
	LinkedList* getNextList() {
		return next;
	}
	void printList();
	int getNum() {
		return numItem;
	}
	int getTotNum();
	bool IsFullList();
};

void LinkedList::insertItemList(Item *p) {	//q 뒤에 p 넣기 -> 무한루프
	if (first == NULL) {
		first = p;
		p->setNext(NULL);
		numItem = numItem + 1;
	}
	else {
		Item *q = first;
		//q = first->getNext();
		if (!this->IsFullList()) {
			while (q->getNext() != NULL) {
				q = q->getNext();
			}
			q->setNext(p);
			numItem = numItem + 1;
		}
		else {
			cout << "overflow list in LinkedList.h\n";
			cout << "num of item in list : " << numItem << endl;
			if (next == NULL) {
				next = new LinkedList();
			}
			next->insertItemList(p);
		}
	}
}

Item* LinkedList::getItem(unsigned k) {
	Item *tmp = first;
	if (tmp->getNext() == NULL && tmp->getKey() == k) {	//first : 블럭(버킷) 포인터, k와 같은 값을 가지는 유일한 아이템
		return tmp;
	}
	while (tmp->getNext() != NULL) {
		if (tmp != first && tmp->getKey() == k) {	//first : 그냥 블럭(버킷) 포인터, first->next가 리턴되면 제대로 된 것
			return tmp;
		}
		tmp = tmp->getNext();
	}
	cout << "no item matched key\n";
	return NULL;
}

void LinkedList::printList() {
	if (first == NULL) {
		cout << "no Item to print\n";
		return;
	}

	cout << "number of Items : " << numItem << endl;
	if (getNextList() != NULL) {
		cout << "overflowed list\n";
		cout << "number of Items in next list : " << getNextList()->getNum() << endl;
	}
}

int LinkedList::getTotNum() {
	int result = getNum();
	LinkedList *t = next;
	while (t != NULL) {
		result = result + t->getNum();
	}
	return result;
}

bool LinkedList::IsFullList() {
	if (4096 - (sizeof(Item)*getNum()) < sizeof(Item)) {
		return true;
	}
	return false;
}
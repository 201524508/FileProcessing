#include <iostream>
#include <string>
using namespace std;

class Node {
private:
	Node *next;
	int flag;
public:
	Node() {
		next = NULL;
		flag = -1;
	}
	Node *getNext() {
		return next;
	}
	void setNext(Node *p) {
		next = p;
	}
};

class LinkedList {
private:
	Node *first;
public:
	LinkedList() {
		first = NULL;
	}
	~LinkedList() {
		deleteList(first);
	}
	void insertNext(Node *p, Node *q) {
		if (p->getNext() == NULL) {
			p->setNext(q);
		}
	}
	void deleteList(Node *t) {
		if (t == NULL) {
			delete t;
		}
		else {
			deleteList(t->getNext());
			delete t;
		}
	}
};
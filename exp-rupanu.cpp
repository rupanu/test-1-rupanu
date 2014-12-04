//============================================================================
// Name        : hw7-rupanu.cpp
// Author      : Rupanu Pal
// Version     :
// Copyright   : For CSci 451 class-work only
// Description : Producer/Consumer through Doubly Circular List, Ansi-style
//============================================================================

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>

using namespace std;

const int NUM_ELEMENTS = 27;

struct Node {

	Node() : prev(this),next(this), link_num(1), data(0)  {
		pthread_mutex_init ( &mutex, NULL);
	}
	Node(const Node& that) {
		//Node *nn = new Node;
		this->link_num = that.link_num;
		this->data = that.data;
		*this->prev = *that.prev;
		*this->next = *that.next;
		this->mutex = that.mutex;
		//this = nn;
	
	}
	Node(Node* prev, Node* next, int num, int value = 0)
		: prev(prev)
		, next(next)
		, link_num(num)
		, data(value)
	{
		prev->next = this;
		next->prev = this;
	}
	~Node() {
		prev->next = next;
		next->prev = prev;
	}
	friend class DoublyCircularLinkedList;

private:
	int link_num;
	int data;
	Node* next;
	Node* prev;
	pthread_mutex_t mutex;
};
class DoublyCircularLinkedList
{
public:
	DoublyCircularLinkedList() : element(new Node), flag(false) {

	}
	bool empty() const {
		return (element == element->next);
	}

	~DoublyCircularLinkedList() {

		while(!empty()) {

			delete element->next;
		}

		delete element;
	}
	void addElement(int num) {
		 new Node(element, element->next, num);
	}
	void printAll() {
		Node* trav = element->prev;
		int link = 1;
		while(trav != element) {
			cout << "Link #" << trav->link_num << "\tData "<< trav->data << endl;
			trav = trav->prev;
			link++;
		}
	}
	void printVal(Node* node) {
		cout << "Link #" << node->link_num << "\tData "<< node->data << endl;
	}
	void assign(Node* node, int val) {
		pthread_mutex_lock(&node->mutex);
		node->data = val;
		pthread_mutex_unlock(&node->mutex);
		//cout << "Assigned Link# " << node->link_num << "\tData "<< node->data << endl;
	}

	Node* getNode() {
		return element;
	}
	Node* nextNode(Node* node)
	{
		if (!empty()) {
			return node->prev;
		}
	}
	Node* prevNode(Node* node)
	{
		if (!empty()) {
			return node->next;
		}
	}
	void setFlag() {
		flag = !flag;
	}

	bool flag;
private:
	Node* element;
};

DoublyCircularLinkedList mylist;

void* f_producer(void*) {
	int n = 1;
	Node* trav = mylist.getNode();
	while(!mylist.flag) { //
		mylist.assign(trav, n);
		trav = mylist.nextNode(trav);
		n++;
	}
}

void* f_consumer(void*) {
	Node* trav = mylist.prevNode(mylist.getNode());
	int n=1;
	while(!mylist.flag) { //
		mylist.printVal(trav);
		trav = mylist.prevNode(trav);
		n++;
	}
}

int main() {
	cout << "CSci 451 Homework 7" << endl;

	for(int i = 2;i <= NUM_ELEMENTS;++i)
	{
		mylist.addElement(i);
	}
	//f_producer(NULL);
	//f_consumer(NULL);
	//mylist.printAll();
	pthread_t t_producer, t_consumer;
	int retval = 0;

	//Create and check threads for failure.
	retval=pthread_create(&t_producer, NULL, &f_producer, NULL);
	if(retval){
		cout << "Could not create producer thread" << endl;
	}
	retval=pthread_create(&t_consumer, NULL, &f_consumer, NULL);
	if(retval){
		cout << "Could not create consumer thread" << endl;
	}
	//Join all threads
	pthread_join(t_producer, NULL);
	pthread_join(t_consumer, NULL);

	mylist.setFlag();

	char x;
	pthread_exit(&x);

	return 0;
}

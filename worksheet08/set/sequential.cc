#include <limits>
#include <cassert>
#include <iostream>

template<typename T>
class Node
{
public:
	T val;
	Node<T> *next = nullptr;

	Node(const T& v) : val(v) {}
	Node(const T& v, const Node<T> *p) : val(v), next(p) {}
};


template<typename T>
class Set
{
	Node<T> tail;
	Node<T> head;

public:
	Node<T>* find_node(const T& val);

	bool add(const T& val);
	bool remove(const T& val);
	bool contains(const T& val);

	void dump();
  
public:
	Set() : head(std::numeric_limits<T>::min()),
		tail(std::numeric_limits<T>::max())
	{
		head.next = &tail;
	}
};

template<typename T>
void Set<T>::dump()
{
	Node<T> *node = &head;
	while (node) 
	{
		std::cout << node << " " << node->val << "\n";
		node = node->next;
	}
	std::cout << "------------------------\n\n";
}

template<typename T>
Node<T>* Set<T>::find_node(const T& val)
{
	Node<T> *curr = &head;
	Node<T> *next = curr->next;
  
	while (next && next->val < val)
	{
		curr = next;
		next = curr->next;
	}
	return curr;
}

template<typename T>
bool Set<T>::add(const T& val)
{
	Node<T> *res = find_node(val);
	if (res->next->val == val)
		return false;

	Node<T> *node = new Node<T>(val);
	node->next = res->next;
	res->next = node;
	return true;
}

template<typename T>
bool Set<T>::remove(const T& val)
{
	Node<T> *res = find_node(val);
	if (res->next->val != val)
		return false;

	Node<T> *node = res->next;
	res->next = node->next;
	delete(node);
	return true;
}

template<typename T>
bool Set<T>::contains(const T& val)
{
	Node<T> *res = find_node(val);
	return res->next->val == val;
}

int main(int argc, char* artv[])
{
	Set<int> s;
 
	s.add(23);
	s.add(25);
	s.add(-34);

	s.dump();

	s.remove(25);
	s.remove(234);
	s.remove(25);
  
	s.dump();
}

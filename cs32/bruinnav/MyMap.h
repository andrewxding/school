// MyMap.h


#include <iostream>
#include <cassert>

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap();
	void clear();
	int size() const;

	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct Node {
		Node(KeyType k, ValueType v)
		{
			m_key = k;
			m_value = v;
			left = nullptr;
			right = nullptr;
		}
		Node *left, *right;
		KeyType m_key;
		ValueType m_value;
	};
	Node *m_root;
	int m_size;
	void clear(Node *cur);
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap() 
{
	m_size = 0;
	m_root = nullptr;
}
//only clear if stuff in it
template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap() 
{
	if (m_size > 0)
		clear();
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const 
{
	return m_size;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value) 
{//add if only item so far
	if (m_root == nullptr) {
		m_root = new Node(key, value);
		m_size++;
		return;
	}

	//search for where to insert it into 
	Node *cur = m_root;
	for (;;) {

		if (key == cur->m_key) 
		{//override existing value
			cur->m_value = value;
			return;
		}
		//find where to insert
		if (key < cur->m_key) 
		{
			if (cur->left != nullptr)
				cur = cur->left;
			else 
			{
				cur->left = new Node(key, value);
				m_size++;
				return;
			}
		}

		else if (key > cur->m_key) 
		{
			if (cur->right != nullptr)
				cur = cur->right;
			else 
			{
				cur->right = new Node(key, value);
				m_size++;
				return;
			}
		}
	}
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const 
{
	Node *cur = m_root;
	while (cur != nullptr) 
	{//find the node
		if (key == cur->m_key) 
		{
			ValueType* ptrToVal = &(cur->m_value);
			return ptrToVal;
		}

		//traverse the tree
		else if (key < cur->m_key)
			cur = cur->left;
		else
			cur = cur->right;
	}
	return nullptr;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear() 
{
	clear(m_root);
}

//clear helper using recursion
template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear(Node *cur) 
{
	if (cur != nullptr)
	{
		clear(cur->left);
		clear(cur->right);
		delete cur;
		m_size--;
	}
}
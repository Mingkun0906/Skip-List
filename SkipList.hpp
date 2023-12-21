#ifndef ___SKIP_LIST_HPP
#define ___SKIP_LIST_HPP

#include <cmath>
#include <string>
#include <vector>
#include "runtimeexcept.hpp"

/**
 * 
 * @brief Uses the bitwise representation of the key to simulate
 * the flipping of a deterministic coin.
 * 
 * This function looks at the bitwise representation to determine
 * how many layers it occupies in the skip list. It takes the bitwise
 * XOR of each byte in a 32-bit unsigned number and uses the index `previousFlips`
 * to determine the truth value. It's best to look at an example and
 * to start with values that fit into one byte.
 * 
 * Let's start with the function call `flipCoin(0, 0)`. This call is
 * asking whether or not we should insert the key `0` into layer `1`
 * (because `previousFlips` represents the number of previous flips).
 * 
 * This function will get the bitwise representation of 0: 
 * 
 *        00000000 00000000 00000000 00000000
 * 
 * All bytes are XOR'd together: 
 * 
 * 
 *  c = 0 ^ 0 ^ 0 ^ 0
 * 
 * Then the index `previousFlips` is used to obtain the bit in the `previousFlips % 8` position.
 * 
 * So the return value is `0 & 1 = 0`, since the value in the zeroth position 
 * (obtained by bitmasking the value (1 << 0) at the bottom of the function
 * is 0.
 * Thus, this value `0` should never get added beyond the bottom-most layer.
 * 
 * Before:
 * 
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 * 
 * And after 0 is inserted
 * 
 * S_1: -inf --------> inf
 * S_0: -inf --> 0 --> inf
 * 
 * Let's look at something more interesting, like the call `flipCoin(5, 0)`.
 * Remember the binary representation for 5 is 00000101.
 * 
 * c = 0 ^ 0 ^ 0 ^ 00000101 = 00000101
 * 
 * Now we get the bit at 0th position (from `previousFlips`).
 * 
 * 00000101 & 1 = 1. Thus, `5` DOES get inserted into the next layer,
 * layer 1.
 * 
 * So the skip list before this insertion might look like:
 * 
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 * 
 * And after it would look like:
 * 
 * S_2: -inf --------> inf
 * S_1: -inf --> 5 --> inf
 * S_0: -inf --> 5 --> inf
 * 
 * Observe that a new layer had to get created, because there should always
 * be an empty layer at the top.
 * 
 * Since 5 got inserted into the next layer, we need to flip again to see if
 * it should get propagated once more.
 * 
 * `flipCoin(5, 1)` this will produce a result of `0`, since 00000101 & (1 << 1) = 0.
 * 
 * Thus, `5` will not be pushed up to the next layer.
 * 
 * @param key The integer key which will be added to the skip list
 * @param previousFlips The number of previous flips for this key
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
inline bool flipCoin(unsigned key, unsigned previousFlips)
{
	char c;
	unsigned first8Bits = (key & 0xFF000000) / 0x01000000 ; 
	unsigned next8Bits =  (key & 0x00FF0000) / 0x00010000;
	unsigned andThen =	 (key & 0x0000FF00) / 0x00000100;
	unsigned lastBits =   (key & 0x000000FF);
	c = first8Bits ^ next8Bits ^ andThen ^ lastBits;
	previousFlips = previousFlips % 8;
	return ( c & (1 << previousFlips) ) != 0;
}

/**
 * @brief Works the same as the integer version above, except
 * it XORs chars in a string instead of the first four
 * bytes of an integer.
 * 
 * @param key key that will be inserted into the skip list
 * @param previousFlips number of previous flips for this key 
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
inline bool flipCoin(std::string key, unsigned previousFlips)
{
	char c = key[0];
	for(unsigned j = 1; j < key.length(); j++)
	{
		c = c ^ key[j];
	}
	previousFlips = previousFlips % 8;
	return ( c & (1 << previousFlips) ) != 0;	
}

template<typename Key, typename Value>
class SkipList
{


private:
	struct Node
	{
		Key key;
		Value value;
		Node * next;
		Node * down;
		Node * up;
		
		Node(const Key & k, const Value & v, Node * n, Node * d, Node * u) 
		{
			key = k;
   			value = v;
    		next = n;
    		down = d;
			up = u;
		}
	};
	Node * head;
	Node * tail;
	size_t listSize = 0;
	Node * top_left;
	Node * bot_left;
	Node * top_right;
	Node * bot_right;
	unsigned layer_num = 0;
	unsigned max_layer_num = 13;
	


public:
	// Constructor
	SkipList();

	// Destructor
	~SkipList();

	// How many distinct keys are in the skip list?
	size_t size() const noexcept;

	// Does the Skip List contain zero keys?
	bool isEmpty() const noexcept;

	// How many layers are in the skip list?
	// Note that an empty Skip List has two layers by default,
	// the "base" layer S_0 and the top layer S_1.
	//
	// [S_1: Top layer]    -inf ------> inf
	// [S_0: Bottom layer] -inf ------> inf
	//
	// This "empty" Skip List has two layers and a height of one.
	unsigned numLayers() const noexcept;

	// What is the height of this key, assuming the "base" layer S_0
	// contains keys with a height of 1?
	// For example, "0" has a height of 1 in the following skip list.
	//
	// [S_1]  -inf --------> inf
	// [S_0]  -inf --> 0 --> inf
	//
	// Throw an exception if this key is not in the Skip List.
	unsigned height(const Key & k) const;

	// If this key is in the SkipList and there is a next largest key
	// return the next largest key.
	// This function should throw a RuntimeException if either the key doesn't exist
	// or there is no subsequent key. 
	// A consequence of this is that this function will
	// throw a RuntimeException if *k* is the *largest* key in the Skip List.
	Key nextKey(const Key & k) const;

	// If this key is in the SkipList and a next smallest key exists,
	// return the next smallest key.
	// This function should throw a RuntimeException if either the key doesn't exist
	// or there is no previous key. 
	// A consequence of this is that this function will
	// throw a RuntimeException if *k* is the *smallest* key in the Skip List.
	Key previousKey(const Key & k) const;

	// These return the value associated with the given key.
	// Throw a RuntimeException if the key does not exist.
	Value & find(const Key & k);
	const Value & find(Key k) const;

	// Return true if this key/value pair is successfully inserted, false otherwise.
	// See the project write-up for conditions under which the key should be "bubbled up"
	// to the next layer.
	// If the key already exists, do not insert one -- return false.
	bool insert(const Key & k, const Value & v);

	// Return a vector containing all inserted keys in increasing order.
	std::vector<Key> allKeysInOrder() const;

	// Is this the smallest key in the SkipList? Throw a RuntimeException
	// if the key *k* does not exist in the Skip List. 
	bool isSmallestKey(const Key & k) const;

	// Is this the largest key in the SkipList? Throw a RuntimeException
	// if the key *k* does not exist in the Skip List. 
	bool isLargestKey(const Key & k) const;

	void print();
	
};

template<typename Key, typename Value>
SkipList<Key, Value>::SkipList() {
	Node * bot_leftMost = new Node(Key(), Value(), nullptr, nullptr, nullptr);
	Node * bot_rightMost = new Node(Key(), Value(), nullptr, nullptr, nullptr);
	bot_leftMost -> next = bot_rightMost;
	bot_left = bot_leftMost;
	bot_right = bot_rightMost;

	Node* top_leftMost = new Node(Key(), Value(), nullptr, bot_leftMost, nullptr);
	Node* top_rightMost = new Node(Key(), Value(), nullptr, bot_rightMost, nullptr);
	top_leftMost -> next = top_rightMost;
	top_left = top_leftMost;
	top_right = top_rightMost;

	bot_leftMost -> up = top_leftMost;
	bot_rightMost -> up = top_rightMost;

	layer_num += 2;

}

template<typename Key, typename Value>
SkipList<Key, Value>::~SkipList() {
	Node * current_layer_left = top_left;
	while(current_layer_left != nullptr)
	{
		Node * currentNode = current_layer_left;
		Node * nextLayer = current_layer_left->down;
		while(currentNode != nullptr)
		{
			Node * temp = currentNode;
			currentNode = currentNode->next;
			delete temp;
		}
		current_layer_left = nextLayer;
	}
}

template<typename Key, typename Value>
size_t SkipList<Key, Value>::size() const noexcept 
{
	return listSize;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isEmpty() const noexcept 
{
	return listSize == 0;
}

template<typename Key, typename Value>
unsigned SkipList<Key, Value>::numLayers() const noexcept 
{
	return layer_num;
}

template<typename Key, typename Value>
unsigned SkipList<Key, Value>::height(const Key & k) const 
{
	Node * currentNode = top_left;
	unsigned height = 0;
	for(int i = layer_num - 1; i >= 0; i--)
	{
		while(currentNode->next->next != nullptr && currentNode->next->key <= k)
		{
			if(currentNode->next->key == k)
			{
				height = i + 1;
				break;
			}
			currentNode = currentNode->next;
		}
		if(height)
		{
			break;
		}
		if(i != 0) 
		{
            currentNode = currentNode->down;
        }
	}

	if(!height)
	{
		throw RuntimeException("The key does not exist in the skip list.");
	}
	return height;    
}

template<typename Key, typename Value>
Key SkipList<Key, Value>::nextKey(const Key & k) const 
{
	Node * currentNode = top_left;
	Node * currentLayer_left = top_left;
	for(int i = layer_num - 1; i >= 0; i--)
	{
		while(currentNode->next->next != nullptr && currentNode->next->key <= k)
		{
			if(currentNode->next->key == k)
			{
				currentNode = currentNode->next;
				for(int j = i - 1; j >= 0; j--)
				{
					currentNode = currentNode->down;
				}
				return currentNode->next->key;
			}
			currentNode = currentNode->next;
		}
		if(i != 0)
		{
			currentNode = currentNode->down;
			currentLayer_left = currentLayer_left->down;
		}
	}
	if(currentNode->key != k)
	{
		throw RuntimeException("This key does not exist in the skip list.");
	}
	else if(currentNode->next->next == nullptr)
	{
		throw RuntimeException("This key is the largest key in the skip list.");
	}
	return currentNode->next->key;
}

template<typename Key, typename Value>
Key SkipList<Key, Value>::previousKey(const Key & k) const 
{
	Node * currentNode = top_left;
	for(int i = layer_num - 1; i >= 0; i--)
	{
		while(currentNode->next->next != nullptr && currentNode->next->key < k)
		{
			currentNode = currentNode->next;
		}
		if(i != 0)
		{
			currentNode = currentNode->down;
		}
	}
	if(currentNode->next->key != k || currentNode->next == nullptr)
	{
		throw RuntimeException("This key does not exist in the skip list.");
	}
	else if(currentNode == top_left && currentNode->down == nullptr)
	{
		throw RuntimeException("This key is the smallest key in the skip list.");
	}
	return currentNode->key;
}

template<typename Key, typename Value>
const Value & SkipList<Key, Value>::find(Key k) const 
{
	Node * currentNode = top_left;
	Node * currentLayer_left = top_left;
	for(int i = layer_num - 1; i >= 0; i--)
	{
		while(currentNode->next->next != nullptr && currentNode->next->key <= k)
		{
			if(currentNode->next->key == k)
			{
				return currentNode->next->value;
			}
			else
			{
				currentNode = currentNode->next;
			}
		}
		if(i != 0) 
		{
            currentNode = currentNode->down;
			currentLayer_left = currentLayer_left->down;
        }
	}

	throw RuntimeException("The key does not exist in the skip list.");
}

template<typename Key, typename Value>
Value & SkipList<Key, Value>::find(const Key & k) 
{
	Node * currentNode = top_left;
	Node * currentLayer_left = top_left;
	for(int i = layer_num - 1; i >= 0; i--)
	{
		while(currentNode->next->next != nullptr && currentNode->next->key <= k)
		{
			if(currentNode->next->key == k)
			{
				return currentNode->next->value;
			}
			else
			{
				currentNode = currentNode->next;
			}
		}
		if(i != 0) 
		{
            currentNode = currentNode->down;
			currentLayer_left = currentLayer_left->down;
        }
	}

	throw RuntimeException("The key does not exist in the skip list.");
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::insert(const Key & k, const Value & v) 
{
	Node * currentNode = top_left;
	Node * currentLayer_left = top_left;
	for(int i = layer_num - 1; i >= 0; i--)
	{
		while(currentNode->next->next != nullptr && currentNode->next->key < k)
		{
			currentNode = currentNode->next;
		}
		if(i != 0) 
		{
            currentNode = currentNode->down;
			currentLayer_left = currentLayer_left->down;
        }
	}
	if(currentNode->next->next != nullptr and currentNode->next->key == k)
	{
		return false;
	}
	
	Node * new_element = new Node(k, v, currentNode->next, nullptr, nullptr);
	currentNode->next = new_element;
	listSize++;

	Node * current_up_layer_left = bot_left->up;
	Node * current_up_layer_right = bot_right->up;
	Node * below_element = new_element;

	if(listSize > 16)
	{
		max_layer_num = 3 * std::ceil(std::log2(listSize)) + 1;
	}
	unsigned previousFlip = 0;
	while(flipCoin(k, previousFlip) && layer_num < max_layer_num)
	{
		previousFlip++;

		Node * current_Node = current_up_layer_left;
		while(current_Node->next->next != nullptr && current_Node->next->key < k)
		{
			current_Node = current_Node->next;
		}

		Node * up_element = new Node(k, v, current_Node->next, below_element, nullptr);
		current_Node->next = up_element;
		below_element->up = up_element;

		if((layer_num - 1) == previousFlip)
		{
			Node * new_top_left = new Node(Key(), Value(), nullptr, current_up_layer_left, nullptr);
			Node * new_top_right = new Node(Key(), Value(), nullptr, current_up_layer_right, nullptr);
			new_top_left->next = new_top_right;
			top_left->up = new_top_left;
			top_right->up = new_top_right;
			top_left = new_top_left;
			top_right = new_top_right;
			layer_num++;
		}
		below_element = up_element;
		current_up_layer_left = current_up_layer_left -> up;
		current_up_layer_right = current_up_layer_right -> up;
    }
	return true;
}

template<typename Key, typename Value>
std::vector<Key> SkipList<Key, Value>::allKeysInOrder() const 
{
	std::vector<Key> keys;
	Node * currentNode = top_left;
	while(currentNode->down != nullptr) {
		currentNode = currentNode->down;
	}
	currentNode = currentNode->next;
	while(currentNode->next != nullptr) {
		keys.push_back(currentNode->key);
		currentNode = currentNode->next;
	}
    return keys;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isSmallestKey(const Key & k) const 
{
	Node * currentNode = top_left;
	bool keyExists = false;
	for(int i = layer_num - 1; i >= 0; i--) {
        while(currentNode->next->next != nullptr && currentNode->next->key <= k) 
		{
            if(currentNode->next->key == k) 
			{
                keyExists = true;
                break;
            }
            currentNode = currentNode->next;
        }

        if(keyExists)
		{
			break;
		}

        if(i != 0) 
		{
            currentNode = currentNode->down;
        }
    }
	if(!keyExists) 
	{
        throw RuntimeException("The key does not exist in the skip list.");
    }

	currentNode = bot_left->next;
    return currentNode->key == k;;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isLargestKey(const Key & k) const 
{
    Node* currentNode = top_left;
    bool keyExists = false;

    for(int i = layer_num - 1; i >= 0; i--) {
        while(currentNode->next->next != nullptr && currentNode->next->key <= k) 
		{
            if(currentNode->next->key == k) {
                keyExists = true;
                break;
            }
            currentNode = currentNode->next;
        }

        if(keyExists)
		{
			break;
		}

        if(i != 0) 
		{
            currentNode = currentNode->down;
        }
    }

    if(!keyExists) 
	{
        throw RuntimeException("The key does not exist in the skip list.");
    }

    while(currentNode->down != nullptr) {
        currentNode = currentNode->down;
    }

    while(currentNode->next->next != nullptr) {
        currentNode = currentNode->next;
    }

    return currentNode->key == k;
}


template<typename Key, typename Value>
void SkipList<Key, Value>::print() const 
{
    Node* currentLayerStart = top_left;
    while(currentLayerStart != nullptr) 
	{
        Node* currentNode = currentLayerStart;
        while(currentNode != nullptr) 
		{
            if(currentNode->key != Key())
			{
                std::cout << "(" << currentNode->key << ", " << currentNode->value << ") -> ";
            } 
			else 
			{
                std::cout << "(-, -) -> ";
            }
            currentNode = currentNode->next;
        }
        std::cout << "END" << std::endl;
        currentLayerStart = currentLayerStart->down;
    }
}



#endif


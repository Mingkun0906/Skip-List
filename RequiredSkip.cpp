#include "catch_amalgamated.hpp"
#include "SkipList.hpp"
#include <vector>


namespace{

/*
NOTE:  these are not intended as exhaustive tests.
	 You should make your own additional tests
	 VERY IMPORTANT:  if your SkipList is not templated, or if 
			it is not built as a linked structure, your score for this project
			will be zero.  Be sure your SkipList compiles and runs 
	 		with non-numeric data types. 


IF YOUR CODE DOES NOT PASS EVERY TEST IN THIS FILE, YOUR SCORE WILL BE A ZERO.


*/

	TEST_CASE("CreatedBasics", "[Required]")
	{
		SkipList<unsigned, unsigned> sl;
		REQUIRE( 2 == sl.numLayers());
		REQUIRE( 0 == sl.size() );
		REQUIRE( sl.isEmpty() );
	}

	TEST_CASE("InsertTestFirst", "[Required]")
	{
		SkipList<unsigned, unsigned> sl;
		sl.insert(3, 5);
		REQUIRE( sl.find(3) == 5 );
	}

	TEST_CASE("InsertTestSecond", "[Required]")
	{
		SkipList<std::string, std::string> sl;
		sl.insert("Shindler", "ICS 46");
		REQUIRE(sl.find("Shindler") == "ICS 46");
	}

	TEST_CASE("SimpleNextAndPrev", "[Required]")
	{
		SkipList<unsigned, unsigned> sl;
		for(unsigned i=0; i < 10; i++)
		{
			sl.insert(i,i);
		}
		for(unsigned i=1; i < 9; i++)
		{
			REQUIRE(sl.previousKey(i) == (i-1));
			REQUIRE(sl.nextKey(i) == (i+1) );
		}
	}

	TEST_CASE("SimpleAllKeysInOrder", "[Required]")
	{
		SkipList<unsigned, unsigned> sl;
		std::vector<unsigned> expected;
		for(unsigned i=0; i < 10; i++)
		{
			sl.insert(i, (100 + i) );
			expected.push_back( i );
		}
		REQUIRE( expected == sl.allKeysInOrder() );
	}

}

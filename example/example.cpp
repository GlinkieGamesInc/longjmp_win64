/**
 * example.cpp - a simple example demonstrating ...
 *               a) how to jump out of a function
 *               b) the vendor supplied implementation of longjmp unwinds
 *                  the stack and destroys local objects (as exceptions would)
 *               c) the custom implementation of longjmp (longjmp_win64)
 *                  does not unwind the stack or destroy local objects
 *                  (the desired result in this case).
 *
 *       Intended with use with vs2013 or later (tested up to vs2019), x64 build target only.
 *
 *
 * Written by Paul Glinker - Glinkie Games Inc.
 *
 * Copyright 2017 Glinkie Games Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#include <iostream>
#include <setjmp.h> // the MS setjmp/longjmp implementation.

#include "../longjmp_win64.h" // our setjmp/longjmp implementation.

using namespace std;


/**
* The purpose of this class is to display
* a message when the destructor is called.
*
* If the destructor is *not* called, we will
* consider our test a success.
*/
class DestructionNotifier
{
public:

	DestructionNotifier(const std::string & strIdentifier, bool * const pbFailed)
		: m_strIdentifier(strIdentifier), m_pbFailed(pbFailed)
	{
	}

	~DestructionNotifier()
	{
		if (m_pbFailed)
		{
			*m_pbFailed = true;
		}
		cout << "Destructor triggered from " << m_strIdentifier.c_str() << ".  Test FAILED." << endl;
	}

private:

	std::string m_strIdentifier;
	bool *		m_pbFailed;
};


/**
* standard_jump_test - create a local (stack-based) instance
* of the DestructionNotifier class, and then use the vendor
* supplied implementation of longjmp to jump out.
*
* The test is considered a success if the destructor of tempObject
* is never called. (this test always fails, which is why we need longjmp_win64)
**/
jmp_buf standard_buffer;
bool    standard_jump_test_failed = false;
int standard_jump_test(const int c, const int d)
{
	DestructionNotifier tempObject(__FUNCTION__, &standard_jump_test_failed);

	cout << "c is " << c << " and d is " << d << endl;

	longjmp(standard_buffer, 1);

	cout << "c + d is " << (c + d) << endl;
	return c + d;
}


/**
 * custom_jump_test - create a local (stack-based) instance
 * of the DestructionNotifier class, and then use longjmp_win64
 * to jump out.
 *
 * The test is considered a success if the destructor of tempObject
 * is never called.
 **/
JMP_BUF_WIN64 custom_buffer;
bool          custom_jump_test_failed = false;
int custom_jump_test(const int a, const int b)
{
	DestructionNotifier tempObject(__FUNCTION__, &custom_jump_test_failed);

	cout << "a is " << a << " and b is " << b << endl;

	longjmp_win64(custom_buffer, 1);

	cout << "a + b is " << (a + b) << endl;
	return a + b;
}


int main(int argc, char * argv[])
{
	if (setjmp(standard_buffer) == 0)
	{
		standard_jump_test(5, 6); // <-- flow-wise, never returns from here.

		cout << "This text will never be displayed, because the "
			 << "standard_jump_test returns as though it were a call "
			 << "to the vendor supplied setjmp just above."
			 << endl;
	}

	if (setjmp_win64(custom_buffer) == 0)
	{
		custom_jump_test(3, 4); // <-- flow-wise, never returns from here.

		cout << "This text will never be displayed, because the "
			 << "custom_jump_test returns as though it were a call "
			 << "to setjmp_win64 just above."
			 << endl;
	}

	if (standard_jump_test_failed)
	{
		cout << "standard_jump_test failed, as expected." << endl;
	}
	else
	{
		cout << "standard_jump_test passed, which was *not* expected." << endl;
	}

	if (custom_jump_test_failed)
	{
		cout << "custom_jump_test failed, which was *not* expected." << endl;
	}
	else
	{
		cout << "custom_jump_test passed, as expected." << endl;
	}

	cout << "Done!" << endl;

	return 0;
}

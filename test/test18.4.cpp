#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

using std::cout, std::endl;

// refer and evict dirty file-backed page
// Segfault 3/20

int main()
/*
{
    char *filename = (char *)vm_map(nullptr, 0);

    strcpy(filename, "lampson83.txt");

    char *p = (char *)vm_map(filename, 0);

    p[0] = 'a';

    char *page1 = (char *)vm_map(nullptr, 0);
    char *page2 = (char *)vm_map(nullptr, 0);
    page1[0] = 'b';
    page2[0] = 'c';

    // refer p[0] again
    cout << p[0];

    cout << page1[0];
    cout << page2[0];

    // evict p
    cout << filename[0];

    cout << endl;

    return 0;
}
*/

{
	char * filename = (char *)vm_map(nullptr, 0);

	strcpy(filename, "lampson83.txt");

	char * p = (char *)vm_map(filename, 0);

	char * p2 = p;

	p[0] = 'a';
	p2[1] = 'a';

	if (fork()) {
		char * p3 = p;
		for (int i = 0; i < 10; ++i)
			cout << p[i];
		cout << endl;
		for (int i = 0; i < 10; ++i)
			cout << p3[i];
		cout << endl;

		p3[2] = 'a';

		for (int i = 0; i < 10; ++i)
			cout << p[i];
		cout << endl;
		for (int i = 0; i < 10; ++i)
			cout << p3[i];
		cout << endl;

	} else {
		vm_yield();
	}

}

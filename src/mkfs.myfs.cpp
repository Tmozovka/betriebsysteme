//
//  mk.myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#include "myfs.h"
#include "blockdevice.h"
#include "macros.h"
#include <stdio.h>
#include <fstream>
#include <math.h>

int main(int argc, char *argv[]) {

	printf("Please wait. Your filesystem is creating. ");
	LOGF("Please wait. Your filesystem is creating. ");


	char * nameCont = argv[1];
	LOGF("container: %s \n",nameCont);

	MyFS * fs = new MyFS();
	fs->blocks->create(nameCont);

	char * pufferAdd;
	char * pufferRead;

	int count = 0;
//START:
//write files
	for (int i = 2; i < argc; i++) {

		FILE *fin;
		LOGF("open File: %s \n", argv[i]);

		fin = fopen(argv[i], "rwb");
		if (fin) {
			LOGF("successful open File: %s \n", argv[i]);
			LOGF("successful open File: %s \n", argv[i]);

			struct stat st;
			st.st_mode = S_IFREG | 0444;
			stat(argv[i], &st);

			//////////////////////NAME AUSLESEN UND BEARBEITEN//////////////////////////////////////////////
			if ((*(argv[i]) == '.') && (*(argv[i] + 1) == '/')) {
				argv[i] += 2;
			}

			int countArg = 0;
			int schiebCount = 0;
			while (*(argv[i]) != char(0)) {
				if (*argv[i] == '/') {
					schiebCount++;
				}

				argv[i]++;
				countArg++;

			}
			argv[i] -= countArg;
			countArg = 0;
			if (schiebCount != 0) {
				for (int j = 0; j < schiebCount; j++) {

					while (*argv[i] != '/') {
						argv[i]++;
					}
					argv[i]++;
				}
			}

			argv[i] -= countArg;
			countArg = 0;

			/////////////////////////////////////////////////////////////////////////////////
			off_t size =st.st_size;
			pufferAdd = new char[size];
			fread(pufferAdd, size, 1, fin);
			fs->addFile(argv[i], st.st_mode, st.st_mtime, size, pufferAdd);

		} else {
			LOGF("can't open File: %s \n", argv[i]);
		}

		delete[] pufferAdd;
	}

	LOG("all files are in container.bin \n");
	fs->root->showRoot();

	string * ar;
	ar = new string[fs->root->getSize()];
	ar = fs->root->getArray();

	fs->writeBlockDevice();

	delete [] ar;

	
	return 0;
}

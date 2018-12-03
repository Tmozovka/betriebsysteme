#pragma once
#include <string>
#include <list>

#include <stdio.h>
#include <string.h>

#include "MyFile.h"
#include <sys/types.h>
#include <unistd.h>
using namespace std;

class MyRoot {
private:
	std::list<MyFile> files;
	//MyFile files[BLOCK_NUMBER];
	MyFile* addressRoot;

public:


	//TODO KONSTRUKTOR DESTRUKTOR
	//MyRoot();

	//Julia: warum hat diese klasse 3 Konstruktoren, wenn in myfs.cpp nur MyRoot() aufgerufen wird?
	MyRoot(MyFile firstfile);
	MyRoot();
	MyRoot(string name, off_t size, mode_t mode,int firstBlock);
	~MyRoot();

	int addFile(string name, off_t size, mode_t mode,int firstBlock);
	int deleteFile(string name);
	int getFile(string name, MyFile* file);

	/*int getFileTry(int number);
	MyFile getFile(int number);*/
	void getArray(string * ar);
};



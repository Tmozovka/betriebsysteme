#pragma once
#include <string>
#include <list>

#include <stdio.h>
#include <string.h>

#include "MyFile.h"
#include "blockdevice.h"
#include <sys/types.h>
#include <unistd.h>
using namespace std;

class MyRoot {
private:
	std::list<MyFile> files;
	MyFile* addressRoot;
	int sizeRoot;

public  :

	//Julia: warum hat diese klasse 3 Konstruktoren, wenn in myfs.cpp nur MyRoot() aufgerufen wird?
	MyRoot(MyFile firstfile);
	MyRoot();
	MyRoot(string name, off_t size, mode_t mode, int firstBlock);
	MyRoot(char** array);
	~MyRoot();

	int addFile(string name, off_t size, mode_t mode, time_t t, int firstBlock);
	int deleteFile(string name);
	int getFile(string name, MyFile* file);
	MyFile* getAdr(){return addressRoot;};
	int copyFile(string name, MyFile* file);
	int setSize(string name, off_t size);

	/*int getFileTry(int number);
	 MyFile getFile(int number);*/
	//void getArray(string ** ar);
	string* getArray();

	bool existName(string name);
	bool compareRoots(MyRoot* root);
	void showRoot();

	int getSize() {
		return sizeRoot;
	}
//	char** createCharArray(char** array);

//	char** writeRootChar();
//	char** writeBlocks(BlockDevice blocks);
//	char** readBlocks(BlockDevice blocks);

	//Tanja's funktionen , um Root in BlockDevice zu schreiben
	void writeBlockDevice(BlockDevice * blocks, int start);
	void resize(char * text, int oldSize, int newSize);
	MyRoot(BlockDevice * b, int start);
	void init(BlockDevice * b, int start);
	void read( int start, BlockDevice * b);
	friend bool operator ==(MyRoot const &f, MyRoot const& f2);
	friend bool operator !=(MyRoot const &f, MyRoot const& f2);


};


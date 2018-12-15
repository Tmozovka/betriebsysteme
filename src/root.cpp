// TODO LinkedList, bibliotheken anschauen

#include <iostream>
#include "root.h"
#include "myfs-structs.h"
#include <string>
#include "macros.h"

MyRoot::MyRoot(MyFile firstfile) {
	sizeRoot = 1;
	addressRoot = new MyFile(firstfile);
	files.push_front(firstfile);

	printf("Konstruktor von MyRoot ist beendet \n");
}

MyRoot::MyRoot() {
	sizeRoot = 0;
	addressRoot = NULL;

	printf("Konstruktor von MyRoot ist beendet \n");
}

//MyFile(string cname, uid_t cuser, gid_t cgroup, off_t csize, mode_t cmode, time_t clastAccess,
//time_t clastMod, time_t clastStatusChange, int cfirstBlock);

MyRoot::MyRoot(string name, off_t size, mode_t mode, int firstBlock) {
	//firstfile -> first block
	sizeRoot = 1;
	MyFile * firstfile = new MyFile(name, getuid(), getgid(), size, mode,
			time(NULL), time(NULL), time(NULL), firstBlock);
	addressRoot = firstfile;
	files.push_front(*firstfile);
	printf("Konstruktor von MyRoot ist beendet \n");
}

MyRoot::~MyRoot() {
	sizeRoot = 0;
	files.clear();
	printf("Destruktor von MyRoot ist beendet \n");
}

int MyRoot::addFile(string name, off_t size, mode_t mode, time_t st_mtime,
		int firstBlock) {
//Wird das uebergebene Size abgefragt oder das des Roots?
//Es sollte ja das des roots angesprochen werden
//Und muesste es nicht >= heissen?
	if (sizeRoot > NUM_DIR_ENTRIES) {
		printf("too many files in Root \n");
		return -1;
	}

	sizeRoot++;
	if (name.length() > NAME_LENGTH) {
		printf("File's %s name is too big \n", name.c_str());
		return -1;
	}

	if (existName(name)) {
		printf("File's %s name is already exist \n", name.c_str());
		return -1;
	}

	//Speichern von Name, Dateigroesse und Zugriffsrechte pro Datei
	const MyFile * f = new MyFile(name, getuid(), getgid(), size, mode,
			time(NULL), st_mtime, time(NULL), firstBlock);

	//Wenn erstes File
	if (sizeRoot == 1) {
		//todo: ich(julia) habe hier folgendes entfernt:
		//addressRoot = new MyFile(f); weil so ein konstruktor nicht existiert
		//Macht es noch das richtige? Bitte Überprüfen

		addressRoot = new MyFile(name, getuid(), getgid(), size, mode,
				time(NULL), st_mtime, time(NULL), firstBlock);
	}

	files.push_back(*f);
	return 0;
}

bool MyRoot::existName(string name) {
	std::list<MyFile>::iterator it = files.begin();

	while (it != files.end()) {
		if ((it++)->getName() == name) {
			if ((it++)->getName().length() == name.length())
				return true;
		}
	}

	return false;
}

void MyRoot::showRoot() {
	printf(
			"************************************************************ \nRoot: \n");
	std::list<MyFile>::iterator it = files.begin();

	while (it != files.end()) {
		(it)->showFile();
		it++;
	}
	printf("************************************************************ \n");
}

int MyRoot::getFile(string name, MyFile * f) {

	LOGF("Starting to look for file in root");
	std::list<MyFile>::iterator it = files.begin();

	while (it->getName() != name) {
		LOGF("Found this file:%s",it->getName());
		it++;
		if (it == files.end()) {
			printf("no such file in root \n");
			return -1;
		}
	}

	*f = *it;
	return 0;

}

int MyRoot::deleteFile(string name) {
	//In Root Verzeichnis Datei loeschen
	if (files.size() != 0) {
		MyFile filetodelete;

		if (getFile(name, &filetodelete) == -1) {
			// no such file
			return -1;
		}
		files.remove(filetodelete);
		sizeRoot--;
		return 0;
	}
	return -1;
}

/*void MyRoot::getArray(string ** array)
 {
 string *arr = *array;
 arr = new string[files.size()];
 int k = 0;
 for (MyFile i: files) {
 arr[k++] = i.getName();
 }


 }*/
string * MyRoot::getArray() {
	string *arr = new string[sizeRoot];
	std::list<MyFile>::iterator it = files.begin();

	int k = 0;
	while (it != files.end()) {
		arr[k++] = it->getName();
		it++;
	}

	return arr;
}


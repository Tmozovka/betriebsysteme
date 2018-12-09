// TODO LinkedList, bibliotheken anschauen


#include "root.h"
#include "myfs-structs.h"
#include <string>

MyRoot::MyRoot(MyFile firstfile) {
	 size=1;
	addressRoot = new MyFile(firstfile);
	files.push_front(firstfile);
}

MyRoot::MyRoot() {
	 size=0;
	addressRoot=NULL;
}

//MyFile(string cname, uid_t cuser, gid_t cgroup, off_t csize, mode_t cmode, time_t clastAccess,
//time_t clastMod, time_t clastStatusChange, int cfirstBlock);

MyRoot::MyRoot(string name, off_t size, mode_t mode,int firstBlock) {
	//firstfile -> first block
	 size=1;
	MyFile * firstfile = new MyFile(name, getuid(), getgid(), size, mode, time(NULL),time(NULL),time(NULL), firstBlock);
	addressRoot = firstfile;
	files.push_front(*firstfile);
}

MyRoot::~MyRoot() {
	size=0;
	files.clear();
}

int MyRoot::addFile(string name, off_t size, mode_t mode,time_t st_mtime, int firstBlock) {
if(size>NUM_DIR_ENTRIES)
{
	printf("too many files in Root \n");
				return -1;
}


	size++;
		if(name.length()>NAME_LENGTH)
		{
			printf("File's %s name is too big \n", name);
			return -1;
		}

		if(existName(name))
		{
			printf("File's %s name is already exist \n", name);
			return -1;
		}

	//Speichern von Name, Dateigroesse und Zugriffsrechte pro Datei
	const MyFile * f = new MyFile(name, getuid(), getgid(), size, mode, time(NULL),st_mtime,time(NULL),firstBlock);
	
	//Wenn erstes File
	if(size==1){
	addressRoot = new MyFile(f);}
	
	files.push_back(*f);
	return 0;
}


bool MyRoot::existName(string name)
{
	std::list<MyFile>::iterator it = files.begin();

	while (it != files.end() )
	{
			if((it++)->getName()==name)
				{
				if((it++)->getName().length()==name.length())
				return true;
				}
	}

	return false;
}

void MyRoot::showRoot()
{
printf("************************************************************ \nRoot: \n");
	std::list<MyFile>::iterator it = files.begin();

		while (it != files.end()){
			(it)->showFile();
			it++;
		}
		printf("************************************************************ \n");
}

int MyRoot::getFile(string name, MyFile * f) {

	std::list<MyFile>::iterator it = files.begin();

	while (it->getName()!=name){
		it++;
		if(it==files.end()){
				printf("no such file in root \n");
				return -1;
				}
	}



	*f=*it;
	return 0;

}

int MyRoot::deleteFile(string name) {
	//In Root Verzeichnis Datei loeschen
	if(files.size() != 0){
		MyFile filetodelete;

		if(getFile(name, &filetodelete)==-1){
		// no such file
			return-1;
		}
		files.remove(filetodelete);
size--;
		return 0;
	}
	return -1;
}

void MyRoot::getArray(string * arr)
{
	arr = new string[files.size()];
		int k = 0;
		for (MyFile i: files) {
			arr[k++] = i.getName();
		}


}

//Tanja add: i need it to have a list from all files
/*MyFile MyRoot::getFile(int n) {

	return files[n];
}

int MyRoot::getFileTry(int n) {

	if(n<0||n>files.size())
		return -1;

	return 0;

}
*/

// TODO LinkedList, bibliotheken anschauen
/*void MyRoot::addFile(string name, off_t size, mode_t mode) {
	myFile copyfile[sizeof(files) + 1];
	int z = 0;
	for (int k = 0; k < (sizeof(files) + 1); k++, z++) {
		copyfile[z] = files[k];
	}
	//Speichern von Name, Dateigroesse und Zugriffsrechte pro Datei
	myFile f = new myFile(name, size, mode);
	copyfile[i] = f;
	files = copyfile.clone();
}

int MyRoot::deleteFile(string name) {
	myFile filetodelete = getFile(name);
	//In Root Verzeichnis Datei loeschen
	int i = 0;
	while (filetodelete.name != files[i].name && filetodelete.size != files[i].size && filetodelete.mode != files[i].mode) {
		i++;
	}
	//Nachfolgende Elemente um eine Stelle nach hinten kopieren und ueberschreiben und
	//dabei in ein anderes Array kopieren um anschlie�end die Laenge von Files zu verkuerzen
	int indexfiles = i;
	myFile array[sizeof(files) - 1];
	for (int indexarray = 0; index < sizeof(files - 1) && indexarray < sizeof(array - 1); index++, indexarray++) {
		files[index] = files[index + 1];
		array[indexarray] = files[indexarray];
	}
	//Laenge von Files um eins verkuerzen --- Falls Blockanzahl nicht konstant
	//std::array<myFile,> a = { 1,2,3 };	
return 0;
}
myFile ROOT::getFile(string name) {
	int i = 0;
	while (!(strcmp(name, files[i].name))) {
		i++;
	}
	return files[i];
}
*/

#include "root.h"
#include "myfs-structs.h"

MyRoot::MyRoot(MyFile firstfile) {
	addressRoot = new MyFile(firstfile);
	files.push_front(firstfile);
}

MyRoot::MyRoot() {
	addressRoot=NULL;
}

//MyFile(string cname, uid_t cuser, gid_t cgroup, off_t csize, mode_t cmode, time_t clastAccess,
//time_t clastMod, time_t clastStatusChange, int cfirstBlock);

MyRoot::MyRoot(string name, off_t size, mode_t mode,int firstBlock) {
	//firstfile -> first block
	MyFile * firstfile = new MyFile(name, getuid(), getgid(), size, mode, time(NULL),time(NULL),time(NULL), firstBlock);
	addressRoot = firstfile;
	files.push_front(*firstfile);
}

MyRoot::~MyRoot() {
	files.clear();
}

int MyRoot::addFile(string name, off_t size, mode_t mode,time_t st_mtime, int firstBlock) {

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
	files.push_back(*f);
	return 0;
}


bool MyRoot::existName(string name)
{
	std::list<MyFile>::iterator it = files.begin();

	while (it != files.end() )
	{
			if((it++)->getName()==name)
				return true;
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

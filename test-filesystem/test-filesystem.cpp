#include "test-filesystem.h"
#define BUFFERSIZE 100

TestFilesytem::TestFilesytem() {
}

int TestFilesytem::compareLists(MyList list1, MyList list2) {

	//Tets auf verschiedene Längen
	//std::cerr << "list1.list.size()" << list1.list.size() << "list2.list.size()" << list2.list.size();

	if (list1.list.size() != list2.list.size()) {
		std::cout << "\033[1;31mERROR: files have different sizes\033[0m" << std::endl;
		return -1;
	}

	std::list<char*>::iterator it1 = list1.list.begin();
	std::list<char*>::iterator it2 = list2.list.begin();

	int noListElement = 0;
	while (it1 != list1.list.end()) {
		for (int i = 0; i < BUFFERSIZE; i++) {
			//std::cerr<<(*it1)[i]<<(*it2)[i];
			if ((*it1)[i] != (*it2)[i]) {
				std::cout << "\033[1;31m" << "ERROR: content is different" << "\033[0m" << std::endl;
				return -1;
			}
		}
		noListElement++;
		it1++;
		it2++;
	}

	std::cout << "\033[1;32m" << "SUCCESS: Compared files are identical" << "\033[0m" << std::endl;
	return 1;
}

void TestFilesytem::readFile(char* filename, MyList* list) {
	int fileDescr = myOpen(filename);

	ssize_t bytesRead;
	do {
		char* buffer = new char[BUFFERSIZE + 1];
		bytesRead = read(fileDescr, buffer, BUFFERSIZE);
		buffer[BUFFERSIZE] = '\0';
		if (bytesRead == -1) {
			std::cerr << "Error: Could not read from file, code " << errno << std::endl;
			exit(-1);
		}

		if (bytesRead != 0) {
			buffer[bytesRead] = '\0';
			list->list.push_back(buffer);
		}

		if (bytesRead != BUFFERSIZE) {
			list->validBytesLastEntry = bytesRead;
		}
		//use this to print file
		//std::cout << buffer;

	} while (bytesRead > 0);
	std::cout << std::endl;

	myClose(fileDescr);
}

int TestFilesytem::myOpen(char* filename) {
	int fileDescr;

	fileDescr = open(filename, O_RDONLY);

	if (fileDescr == -1) {
		std::cerr << "Error: Could not open file, code " << errno << std::endl;
		exit(-1);
	}
	std::cerr << "File opened successfully!" << std::endl;
	return fileDescr;
}

/*
 * bytesToWrite = Number of Bytes to write
 * offset = Offset in bytes
 */
void TestFilesytem::myWrite(char* filename, int bytesToWrite, int offset) {

	int fileDescr = open(filename, O_CREAT | O_RDWR,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fileDescr == -1) {
		std::cerr << "Error: Could not open file, code " << errno << std::endl;
		exit(-1);
	}

	std::cerr << "Offset is now set to " << offset << ". There are " << bytesToWrite << " Bytes to write" << std::endl;

//Initializing String to write
	int MAX_LENGTH = bytesToWrite;
	char stringToWrite[MAX_LENGTH];
	int i = 0;
	char c = '0';
	while (i < MAX_LENGTH) {
		stringToWrite[i] = c;
		i++;
		c++;
		c = c % 100;
	}

	ssize_t bytesWritten;
	lseek(fileDescr, offset, SEEK_SET);

	bytesWritten = write(fileDescr, stringToWrite, bytesToWrite);

	if (bytesWritten == -1) {
		std::cerr << "Error: Could not write to file, code " << errno << std::endl;
		exit(-1);
	}

	std::cerr << "Wrote " << bytesWritten << " bytes to " << filename;
	if (bytesToWrite != bytesWritten) {
		std::cerr << " Expected were " << bytesToWrite << " Bytes";
	}
	std::cerr << std::endl;
	myClose(fileDescr);

}

void TestFilesytem::myClose(int fileDescr) {
	int ret;
	ret = close(fileDescr);
	if (ret == -1) {
		std::cerr << "Error: Could not close file, code " << errno << std::endl;
		exit(-1);
	}
	std::cerr << "File closed successfully!" << std::endl;
}

void TestFilesytem::writeListToFile(MyList list, char* filename) {

	int fileDescr = open(filename, O_CREAT | O_TRUNC | O_RDWR,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fileDescr == -1) {
		std::cerr << "Error: Could not open file, code " << errno << std::endl;
		exit(-1);
	}

	std::list<char*>::iterator it = list.list.begin();
	ssize_t bytesWritten;
	int currentNoListEntry = 0;

	while (it != list.list.end()) {
		char* buffer = new char[BUFFERSIZE];
		//std::cerr << "currentNoListEntry :" << currentNoListEntry << " list.size(): " << list.list.size() << std::endl;

		/*
		 //Print list entry
		 std::cerr<<"Print list entry "<< currentNoListEntry<<" in PrintListToFile: "<<std::endl;
		 for(int i = 0 ; i<BUFFERSIZE; i++){
		 if(((*it)[i])==0){
		 std::cerr<<"'\\0'";
		 }

		 std::cerr<<(*it)[i];
		 }

		 */

		int noBytesInPuffer = 0;
		//ganzen puffer füllen
		if (currentNoListEntry != list.list.size()) {
			while (noBytesInPuffer < BUFFERSIZE) {
				buffer[noBytesInPuffer] = (*it)[noBytesInPuffer];
				noBytesInPuffer++;
			}
			//Puffer nur soweit füllen, wie gültig
		} else {
			while (noBytesInPuffer < list.validBytesLastEntry) {
				buffer[noBytesInPuffer] = (*it)[noBytesInPuffer];
				noBytesInPuffer++;
			}

		}
		//std::cerr << "Anzahl Bytes in Puffer: " << noBytesInPuffer << std::endl;
		bytesWritten = write(fileDescr, buffer, noBytesInPuffer);
		if (bytesWritten == -1) {
			std::cerr << "Error: Could not write list to file, code " << errno << std::endl;
			exit(-1);
		}

		//std::cerr << "In Buffer von WriteListToFile steht: " << buffer << std::endl;
		it++;
		currentNoListEntry++;
		delete buffer;
	}

	myClose(fileDescr);
}

void TestFilesytem::printFile(MyList list) {

	//std::cerr << "Printfile Methode aufgerufen:" << std::endl;
	int currentListEntry = 0;
	for (char* c : list.list) {
		//std::cerr << "Im " << currentListEntry++ << "ten Eintrag steht:";
		for (int i = 0; i < BUFFERSIZE; i++) {
			std::cerr << "\033[0;7m" << c[i] << "\033[1;0m";
		}
	}
	std::cerr << std::endl;

}

void TestFilesytem::printTwoFiles(char* fn1, char* fn2) {
	if (printingFiles) {
		MyList list1;
		MyList list2;

		readFile(fn1, &list1);
		readFile(fn2, &list2);

		std::cerr << "Print " << fn1 << std::endl;
		printFile(list1);
		std::cerr << "Print " << fn2 << std::endl;
		printFile(list2);

	}
}

int main(int argc, char *argv[]) {
	TestFilesytem test1;

//Zusamenfügen von path von Datei

//for mount-dir
	std::string pathToMountFile("../mount-dir/");

//for testing in test-filessytem folder
//std::string path("");


	 pathToMountFile += argv[1];
	 char FileInMount[pathToMountFile.length() + 1];
	 for (int i = 0; i <= pathToMountFile.length(); i++) {
	 FileInMount[i] = pathToMountFile[i];
	 }


	//char * FileInMount = (char*) "file2.txt";

	char* fileToCompare = (char*) "fileToCompare.txt";

	std::cerr << "Mit Datei: " << FileInMount << " werden nun Tests ausgeführt" << std::endl;

	if (argv[2]) {
		test1.printingFiles = 1;
	} else {
		test1.printingFiles = 0;
	}

//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen" << "\033[1;0m" << std::endl;

	MyList listMount;
	MyList listMount2;
	test1.readFile(FileInMount, &listMount);
	test1.readFile(FileInMount, &listMount2);

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

//***************************************************************************************************
	//Copy von Mountfile in filetocomapre erstellen
	std::cerr << "Now writing list1 to filetoComapre" << std::endl;
	test1.readFile(FileInMount, &listMount);
	test1.writeListToFile(listMount, fileToCompare);

	//Vergleichsfile auslesen
	MyList listCompare;
	test1.readFile(fileToCompare, &listCompare);
	test1.printTwoFiles(FileInMount, fileToCompare);
	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben ohne offset" << "\033[1;0m" << std::endl;
	//schreiben
	test1.myWrite(FileInMount, 200, 0);
	test1.myWrite(fileToCompare, 200, 0);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fileToCompare, &listCompare);
	test1.printTwoFiles(FileInMount, fileToCompare);
	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m" << std::endl;
	test1.myWrite(FileInMount, 200, 5);
	test1.myWrite(fileToCompare, 200, 5);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fileToCompare, &listCompare);
	test1.printTwoFiles(FileInMount, fileToCompare);
	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m" << std::endl;
	test1.myWrite(FileInMount, 200, 200);
	test1.myWrite(fileToCompare, 200, 200);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fileToCompare, &listCompare);
	test1.printTwoFiles(FileInMount, fileToCompare);

	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m" << std::endl;
	test1.myWrite(FileInMount, 20, 510);
	test1.myWrite(fileToCompare, 20, 510);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fileToCompare, &listCompare);
	test1.printTwoFiles(FileInMount, fileToCompare);

	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();


	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m" << std::endl;
	test1.myWrite(FileInMount, 200, 1000);
	test1.myWrite(fileToCompare, 200, 1000);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fileToCompare, &listCompare);
	test1.printTwoFiles(FileInMount, fileToCompare);

	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();




	return 0;

}
;


#include "test-filesystem.h"
#define BUFFERSIZE 100

TestFilesytem::TestFilesytem() {
}

int TestFilesytem::compareLists(MyList list1, MyList list2) {

	//Tets auf verschiedene Längen
	//std::cerr << "list1.list.size()" << list1.list.size() << "list2.list.size()" << list2.list.size();

	if (list1.list.size() != list2.list.size()) {
		std::cout << "\033[1;31m" << "ERROR: files have different sizes"
				<< "\033[0m" << std::endl;
		return -1;
	}

	std::list<char*>::iterator it1 = list1.list.begin();
	std::list<char*>::iterator it2 = list2.list.begin();

	int noListElement = 0;
	int stelle = 0;
	while (it1 != list1.list.end()) {
		for (int i = 0; i < BUFFERSIZE; i++) {
			//std::cerr<<(*it1)[i]<<(*it2)[i];
			stelle++;
			if ((*it1)[i] != (*it2)[i]) {
				std::cout << "\033[1;31m" << "auf der Stelle: " << stelle - 2
						<< "kein Unterschied, it1[i] ist "
						<< (int) (*it1)[i - 2] << " it2[i] ist "
						<< (int) (*it2)[i - 2] << std::endl;
				std::cout << "\033[1;31m" << "auf der Stelle: " << stelle - 1
						<< "kein Unterschied, it1[i] ist "
						<< (int) (*it1)[i - 1] << " it2[i] ist "
						<< (int) (*it2)[i - 1] << std::endl;
				std::cout << "\033[1;31m" << "auf der Stelle: " << stelle
						<< "ist Unterschied, it1[i] ist " << (int) (*it1)[i]
						<< " it2[i] ist " << (int) (*it2)[i] << std::endl;
				std::cout << "\033[1;31m" << "auf der Stelle: " << stelle + 1
						<< "nach der Unterschied, it1[i] ist "
						<< (int) (*it1)[i + 1] << " it2[i] ist "
						<< (int) (*it2)[i + 1] << std::endl;
				std::cout << "\033[1;31m" << "auf der Stelle: " << stelle + 2
						<< "nach der Unterschied, it1[i] ist "
						<< (int) (*it1)[i + 2] << " it2[i] ist "
						<< (int) (*it2)[i + 2] << std::endl;
				std::cout << "\033[1;31m" << "auf der Stelle: " << stelle + 3
						<< "nach der Unterschied, it1[i] ist "
						<< (int) (*it1)[i + 3] << " it2[i] ist "
						<< (int) (*it2)[i + 3] << std::endl;

				std::cout << "\033[1;31m" << "ERROR: content is different"
						<< "\033[0m" << std::endl;
				return -1;
			}
		}
		noListElement++;
		it1++;
		it2++;
	}

	std::cout << "\033[1;32m" << "SUCCESS: Compared files are identical"
			<< "\033[0m" << std::endl;
	return 1;
}

void TestFilesytem::readFile(char* filename, MyList* list, int offset) {
	int fileDescr = myOpen(filename);
	printf("offset : %i\n", offset);
	lseek(fileDescr, offset, SEEK_SET);

	ssize_t bytesRead;
	do {
		char* buffer = new char[BUFFERSIZE + 1];
		bytesRead = read(fileDescr, buffer, BUFFERSIZE);
		buffer[BUFFERSIZE] = '\0';
		if (bytesRead == -1) {
			std::cerr << "\033[1;31m"
					<< "Error: Could not read from file, code " << "\033[0m"
					<< errno << std::endl;
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
		std::cerr << "\033[1;31m" << "Error: Could not open file, code "
				<< errno << "\033[0m" << std::endl;
		exit(-1);
	}
	return fileDescr;
}

/*
 * bytesToWrite = Number of Bytes to write
 * offset = Offset in bytes
 */
void TestFilesytem::myWrite(char* filename, int bytesToWrite, int offset,
		int r) {

	int fileDescr = open(filename, O_CREAT | O_RDWR,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fileDescr == -1) {
		std::cerr << "\033[1;31m" << "Error: Could not open file, code "
				<< errno << "\033[0m" << std::endl;
		exit(-1);
	}

	std::cerr << "Offset is now set to " << offset << ". There are "
			<< bytesToWrite << " Bytes to write" << std::endl;

//Initializing String to write
	int MAX_LENGTH = bytesToWrite;
	char stringToWrite[MAX_LENGTH];
	int i = 0;

		char c;
	 if (offset % 2 == 0)
	 c = char(49);
	 else
	 c = char(50);

	while (i < MAX_LENGTH) {
		//stringToWrite[i] = char(r % 9 + 48);
		stringToWrite[i] = c;
		i++;
		//	c++;
		//	c = c % 100;
	}

	ssize_t bytesWritten;
	lseek(fileDescr, offset, SEEK_SET);

	bytesWritten = write(fileDescr, stringToWrite, bytesToWrite);

	if (bytesWritten == -1) {
		std::cerr << "\033[1;31m" << "Error: Could not write to file, code "
				<< errno << "\033[0m" << std::endl;
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
		std::cerr << "\033[1;31m" << "Error: Could not close file, code "
				<< errno << "\033[0m" << std::endl;
		exit(-1);
	}
}

void TestFilesytem::writeListToFile(MyList list, char* filename) {

	int fileDescr = open(filename, O_CREAT | O_TRUNC | O_RDWR,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fileDescr == -1) {
		std::cerr << "\033[1;31m" << "Error: Could not open file, code "
				<< errno << "\033[0m" << std::endl;
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
			std::cerr << "\033[1;31m"
					<< "Error: Could not write list to file, code " << errno
					<< "\033[0m" << std::endl;
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

void TestFilesytem::printTwoFiles(char* fn1, char* fn2, int offset) {
	if (printingFiles) {
		MyList list1;
		MyList list2;

		readFile(fn1, &list1, offset);
		readFile(fn2, &list2, offset);

		std::cerr << "Print " << fn1 << std::endl;
		printFile(list1);
		std::cerr << "Print " << fn2 << std::endl;
		printFile(list2);

	}
}

int main(int argc, char *argv[]) {
	TestFilesytem test1;

	/*
	 //char * filename ="../mount-dir/text1.txt";
	 char * filename ="./file2.txt";

	 int offset = 0;
	 int fileDescr = open(filename, O_CREAT |O_TRUNC| O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	 printf("fileDescr:%i\n",fileDescr);
	 lseek(fileDescr, offset, SEEK_SET);
	 int length= 100;
	 char* stringToWrite = new char[length+1];
	 stringToWrite[length]= char(0);
	 for(int i = 0; i<length; i++)
	 stringToWrite[i]= 'a';

	 int bytesWritten = write(fileDescr, stringToWrite, length);
	 printf("bytesWritten:%i\n",bytesWritten);
	 int ret = close(fileDescr);
	 printf("ret:%i\n",ret);

	 delete[] stringToWrite;

	 return(0);


	 */

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

	std::cerr << "Mit Datei: " << FileInMount << " werden nun Tests ausgeführt"
			<< std::endl;

	if (argv[2]) {
		test1.printingFiles = 1;
	} else {
		test1.printingFiles = 0;
	}

//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen text1"
			<< "\033[1;0m" << std::endl;

	MyList listMount;
	MyList listMount2;
	test1.readFile(FileInMount, &listMount);
	//test1.readFile(FileInMount, &listMount2);
	test1.readFile("../text1.txt", &listMount2);
	test1.printTwoFiles(FileInMount, "../text1.txt");

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();
//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen text1"
			<< "\033[1;0m" << std::endl;

	test1.readFile(FileInMount, &listMount);
	//test1.readFile(FileInMount, &listMount2);
	test1.readFile("../text1.txt", &listMount2);
	test1.printTwoFiles(FileInMount, "../text1.txt");

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen text2"
			<< "\033[1;0m" << std::endl;

	test1.readFile("../mount-dir/text2.txt", &listMount);
	//test1.readFile(FileInMount, &listMount2);
	test1.readFile("../text2.txt", &listMount2);
	test1.printTwoFiles("../mount-dir/text2.txt", "../text2.txt");

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen text3"
			<< "\033[1;0m" << std::endl;

	test1.readFile("../mount-dir/text3.txt", &listMount);
	//test1.readFile(FileInMount, &listMount2);
	test1.readFile("../input/text3.txt", &listMount2);
	test1.printTwoFiles("../mount-dir/text3.txt", "../input/text3.txt");

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

	//***************************************************************************************************
	/*std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen big.txt"
			<< "\033[1;0m" << std::endl;

	test1.readFile("../mount-dir/big.txt", &listMount);
	test1.readFile("../input/in/big.txt", &listMount2);

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();*/
	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen 2 big.txt"
			<< "\033[1;0m" << std::endl;

	test1.readFile("../mount-dir/big (copy).txt", &listMount);
	test1.readFile("../input/in/big (copy).txt", &listMount2);

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Read mit offset 5 testen text3"
			<< "\033[1;0m" << std::endl;

	test1.readFile("../mount-dir/text3.txt", &listMount, 5);
	//test1.readFile(FileInMount, &listMount2);
	test1.readFile("../input/text3.txt", &listMount2, 5);
	test1.printTwoFiles("../mount-dir/text3.txt", "../input/text3.txt", 5);

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Read mit offset 15 testen text3"
			<< "\033[1;0m" << std::endl;

	test1.readFile("../mount-dir/text3.txt", &listMount, 15);
	//test1.readFile(FileInMount, &listMount2);
	test1.readFile("../input/text3.txt", &listMount2, 15);
	test1.printTwoFiles("../mount-dir/text3.txt", "../input/text3.txt", 15);

	test1.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben ohne offset" << "\033[1;0m"
			<< std::endl;
	//schreiben
	int r = rand();
	char* fcomp = (char*) "../text1.txt";
	MyList listCompare;
	test1.myWrite(FileInMount, 5, 1, r);
	//test1.myWrite(fileToCompare, 200, 0);
	test1.myWrite(fcomp, 5, 1, r);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fcomp, &listCompare);
	//test1.printingFiles=1;
	test1.printTwoFiles(FileInMount, fcomp);
	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben ohne offset" << "\033[1;0m"
			<< std::endl;
	//schreiben
	r = rand();
	test1.myWrite(FileInMount, 500, 1, r);
	//test1.myWrite(fileToCompare, 200, 0);
	test1.myWrite(fcomp, 500, 1, r);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fcomp, &listCompare);
	//test1.printingFiles=1;
	test1.printTwoFiles(FileInMount, fcomp);
	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m"
			<< std::endl;
	r = rand();
	test1.myWrite(FileInMount, 5, 5, r);
	test1.myWrite(fcomp, 5, 5, r);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fcomp, &listCompare);
	test1.printTwoFiles(FileInMount, fcomp);
	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m"
			<< std::endl;
	r = rand()%100;
	test1.myWrite(FileInMount, 10, 9, r);
	test1.myWrite(fcomp, 10, 9, r);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fcomp, &listCompare);
	test1.printTwoFiles(FileInMount, fcomp);

	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m"
			<< std::endl;
	r = rand();
	test1.myWrite(FileInMount, 1000, 494, r);
	test1.myWrite(fcomp, 1000, 494, r);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fcomp, &listCompare);
	test1.printTwoFiles(FileInMount, fcomp);

	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m"
			<< std::endl;
	r = rand();
	test1.myWrite(FileInMount, 500, 15, r);
	test1.myWrite(fcomp, 500, 15, r);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fcomp, &listCompare);
	test1.printTwoFiles(FileInMount, fcomp);

	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	//***************************************************************************************************
	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m"
			<< std::endl;
	r = rand();
	test1.myWrite(FileInMount, 5000, 515, r);
	test1.myWrite(fcomp, 5000, 515, r);

	//files auslesen
	test1.readFile(FileInMount, &listMount);
	test1.readFile(fcomp, &listCompare);
	test1.printTwoFiles(FileInMount, fcomp);

	test1.compareLists(listMount, listCompare);
	listMount.list.clear();
	listCompare.list.clear();

	return 0;

}
;


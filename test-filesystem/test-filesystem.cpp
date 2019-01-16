#include "test-filesystem.h"
#define BUFFERSIZE 100

TestFilesytem::TestFilesytem() {
}

int TestFilesytem::compareLists(std::list<char*> list1, std::list<char*> list2) {
	int errorOccured = 0;

	if (list1.size() != list2.size()) {
		std::cout << "\033[1;31mERROR: files have different sizes\033[0m" << std::endl;
		errorOccured++;
		return -1;
	}

	std::list<char*>::iterator it1 = list1.begin();
	std::list<char*>::iterator it2 = list1.begin();

	int noListElement = 0;
	while (it1 != list1.end()) {
		if ((*it1) != (*it2)) {
			errorOccured++;
			std::cout << "\033[1;31m" << "ERROR: content is different" << "\033[0m" << std::endl;
			std::cout << "Bytes " << BUFFERSIZE * noListElement << " to " << (BUFFERSIZE * (noListElement + 1)) - 1
					<< "are different: " << std::endl;
			std::cout << "\nContent first file:" << std::endl;
			std::cout << *it1 << std::endl;
			std::cout << "\nContent second file:" << std::endl;
			std::cout << *it2 << std::endl;

			//return -1;
		}
		noListElement++;
		it1++;
		it2++;
	}
	if (errorOccured == 0) {
		std::cout << "\033[1;32m" << "SUCCESS: Compared files are identical" << "\033[0m" << std::endl;
	}
	return 1;
}

std::list<char*> TestFilesytem::readFile(char* filename) {
	int fileDescr = myOpen(filename);

	std::list<char*> list;

	char buffer[BUFFERSIZE + 1];
	buffer[BUFFERSIZE] = 0;
	ssize_t bytesRead;

	do {
		bytesRead = read(fileDescr, buffer, BUFFERSIZE);

		if (bytesRead == -1) {
			std::cerr << "Error: Could not read from file, code " << errno << std::endl;
			exit(-1);
		}

		buffer[bytesRead] = 0;
		list.push_back(buffer);

		//use this to print file
		std::cout << buffer;

	} while (bytesRead > 0);
	std::cout << std::endl;

	myClose(fileDescr);

	return list;
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

	std::cerr << "Wrote " << bytesWritten << " bytes.";
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

void TestFilesytem::printFile(std::list<char*> list) {

	std::list<char*>::iterator it = list.begin();

	int noListElement = 0;
	while (it != list.end()) {
		std::cerr << *it << std::endl;
		it++;
	}
	std::cerr << std::endl;

}
int main(int argc, char *argv[]) {
	TestFilesytem test1;

	//Zusamenfügen von path von Datei

	//for mount-dir
	std::string path("../mount-dir/");

	//for testing in test-filessytem folder
	//std::string path("");

	path += argv[1];
	char pathChar[path.length() + 1];
	for (int i = 0; i <= path.length(); i++) {
		pathChar[i] = path[i];
	}

	std::cerr << "Mit Datei: " << pathChar << " werden nun Tests ausgeführt" << std::endl;

	std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen" << "\033[1;0m" << std::endl;
	std::list<char*> list1 = test1.readFile(pathChar);
	std::list<char*> list2 = test1.readFile(pathChar);
	test1.printFile(list1);
	test1.compareLists(list1, list2);

	std::cerr << "\033[0;45m" << "Test: Schreiben ohne offset" << "\033[1;0m" << std::endl;
	test1.myWrite(pathChar, 200, 0);
	std::list<char*> list3 = test1.readFile(pathChar);

	std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m" << std::endl;
	test1.myWrite(pathChar, 200, 5);
	std::list<char*> list4 = test1.readFile(pathChar);

	test1.compareLists(list3, list4);
	return 0;
}
;

#include "test-filesystem.h"
#define BUFFERSIZE 100

TestFilesytem::TestFilesytem() {
}

int TestFilesytem::compareLists(std::list<char*> list1,
		std::list<char*> list2) {

	if (list1.size() != list2.size()) {
		std::cout << "ERROR: files have different sizes" << std::endl;
		return -1;
	}

	std::list<char*>::iterator it1 = list1.begin();
	std::list<char*>::iterator it2 = list1.begin();

	while (it1 != list1.end()) {
		if ((*it1) != (*it2)) {
			std::cout << "ERROR: content is different" << std::endl;
			return -1;
		}
		it1++;
		it2++;
	}
	std::cout << "SUCCESS: Compared files are identical" << std::endl;

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
			std::cerr << "Error: Could not read from file, code " << errno
					<< std::endl;
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
	//int fileDescr = myOpen(filename);



	int fileDescr = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if(fileDescr == -1) {
				std::cerr << "Error: Could not open file, code " << errno << std::endl;
				exit(-1);
			}

	ssize_t bytesWritten;

	int MAX_LENGTH= bytesToWrite;
	char stringToWrite[MAX_LENGTH];
	int i = 0;
	char c = '0';
	while(i<MAX_LENGTH){
		stringToWrite[i]= c ;
		i++;
		c++;
		c = c%100;
	}


	lseek(fileDescr, offset, SEEK_SET);
	std::cerr << "Offset is now set to "<< offset << std::endl;
	bytesWritten = write(fileDescr, stringToWrite, bytesToWrite);

	if (bytesWritten == -1) {
		std::cerr << "Error: Could not write to file, code " << errno
				<< std::endl;
		exit(-1);
	}

	std::cerr << "Wrote " << bytesWritten << " bytes" << std::endl;

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

	std::cerr << "Mit Datei: " << pathChar << " werden nun Tests ausgeführt"
			<< std::endl;

	std::cerr << "Test1: identisch nach mehrmals lesen" << std::endl;
	std::list<char*> list1 = test1.readFile(pathChar);
	std::list<char*> list2 = test1.readFile(pathChar);
	test1.compareLists(list1, list2);

	std::cerr << "Test1: Schreiben  mit offset" << std::endl;
	test1.myWrite(pathChar,200,5);
	test1.readFile(pathChar);


	return 0;
}
;

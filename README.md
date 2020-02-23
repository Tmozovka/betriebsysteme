# Filesystem
Creating the simplest filesystem that can :
 * Read files:
 *less text1.txt*
 * Create an empty file:
  *touch newfile1.txt*
 * Creating and writing a file:
 *echo Hello World> newfile2.txt*
 * Overwrite a file:
 *echo Hello World> newfile1.txt*
 *echo Hello World 2> newfile2.txt*
 * Attach to a file:
 *echo Hello World 2 >> newfile1.txt*
 * Delete a file:
 *rm newfile2.txt*
 * Filesystem can contain not more that 64 files
 * Filesystem has just 30MB memory space.


## Getting Started

### Prerequisites
 1. Install git. Information about git on https://git-scm.com/book/de/v1/Los-geht%E2%80%99s . 
 2. Register on GitHub
 3. Clone repository from git :
	* Go to the folder where you whant to have this project.
	* write in terminal your git credentials :
	*git config --global user.name "Your Name here"*
	*git config --global user.email "your_email@domainname.com"*
	* And clone: 
	*git clone https://github.com/Tmozovka/Filesystem.git*

Hier you can learn how to use git at team : https://jameschambers.co/writing/git-team-workflow-cheatsheet

### Run simplest filesystem
 1. Remove container.bin if it exists:
	*rm container.bin*
 2. Create container.bin and add there files that you whant to have in your simplest filesystem:
	*./mkfs.myfs container.bin text1.txt text2.txt*
	*./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt  ./input/in2/* *
 3. Mount you filesystem using created container.bin. Add logfile to log execution of filesystem. At the last position give a name of folder, where you whant to have your filesystem:
	*./mount.myfs container.bin logfile ./mount*
 4. Go to folder with the simplest filesystem and try it :) :
	*cd mount*
 5. Go back:
	*cd ..* 
 6. Don't forget to unmount the simplest filesystem:
	*fusermount --unmount ./mount*

### Find Allocation Failed Memory:
 1. sudo apt-get install valgrind
 2. valgrind ./mkfs.myfs container.bin text1.txt text2.txt oder valgrind ./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt ./input/in/* ./input/in2/*
 3. valgrind ./mount.myfs container.bin logfile ./mount-dir -s -f
 4. valgrind ./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt ./input/in/* ./input/in2/*
 5. /valgrind ./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt ./input/in/* ./input/in2/*


### Run tests 

 1. Mount your filesystem.
 2. Compile tests: *g++ test-filesystem.cpp*
 3. Give as arguments: name of the folder with simplest filesystem, name of folder with test-files, names of test-files. Run tests with issue about test runs: *./a.out 1 ../mount-dir ../input_for_tests text1.txt text2.txt*
 4. 3. Run tests without issue about test runs: *./a.out 0 ../mount-dir ../input_for_tests text1.txt text2.txt*

	






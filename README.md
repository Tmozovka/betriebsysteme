# Filesystem
Creating the simplest filesystem that can :
 * Read files:
 less text1.txt
 * Create an empty file:
  touch newfile1.txt
 * Creating and writing a file:
 echo Hello World> newfile2.txt
 * Overwrite a file:
 echo Hello World> newfile1.txt
 echo Hello World 2> newfile2.txt
 * Attach to a file:
 echo Hello World 2 >> newfile1.txt
 * Delete a file:
 rm newfile2.txt


## Getting Started

### Prerequisites
 1. Install git. Information about git on https://git-scm.com/book/de/v1/Los-geht%E2%80%99s . 
 2. Register on GitHub
 3. Clone repository from git :
	* Go to the folder where you whant to have this project.
	* write in terminal your git credentials :
		git config --global user.name "Your Name here"
		git config --global user.email "your_email@domainname.com"
	* And clone: 
		git clone https://github.com/Tmozovka/Filesystem.git

Hier you can learn how to use git at team : https://jameschambers.co/writing/git-team-workflow-cheatsheet

### Run simplest filesystem
 1. Remove container.bin if it exists:
		rm container.bin
 2. Create container.bin and add there files that you whant to have in your simplest filesystem:
		./mkfs.myfs container.bin text1.txt text2.txt
		./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt  ./input/in2/*
 3. Mount you filesystem using created container.bin. Add logfile to log execution of filesystem. At the last position give a name of folder, where you whant to have your filesystem.
		./mount.myfs container.bin logfile ./mount
 4. Go to folder with the simplest filesystem and try it :)
		cd mount
 5. Go back
		cd ..
 6. Don't forget to unmount the simplest filesystem
		fusermount --unmount ./mount





# download project 
how to git? information about all steps https://git-scm.com/book/de/v1/Los-geht%E2%80%99s
1) install git
2) register on GitHub
3) write in terminal
git config --global user.name "Your Name here"
git config --global user.email "your_email@domainname.com"
4) git clone https://github.com/Tmozovka/betriebsysteme.git
5)https://jameschambers.co/writing/git-team-workflow-cheatsheet

Fehle beim Speicher Allockeren anschauen :
1)sudo apt-get install valgrind
2)valgrind ./mkfs.myfs container.bin text1.txt text2.txt oder valgrind ./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt ./input/in/* ./input/in2/*
3)valgrind ./mount.myfs container.bin logfile ./mount-dir -s -f
4)valgrind ./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt ./input/in/* ./input/in2/*
5)/valgrind ./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt ./input/in/* ./input/in2/*


# System mounten:
1) rm container.bin

2)  ./mkfs.myfs container.bin text1.txt text2.txt

3)  ./mount.myfs container.bin logfile ./mount

4) cd mount

system testen 

5) cd ..

6) fusermount --unmount ./mount



# Test-filesystem ausführen

1)Filesystem mounten; der Ordner muss mount-dir heißen

2)Im Ordner test-fileesystem Terminal Öffen und compilieren mit:
 	*g++ test-filesystem.cpp*
 	
3)A) Zum ausführen ohne Ausgabe der Files:
	*./a.out text1.txt*

3)B) Zum ausführen mit Ausgabe der Files:
	*./a.out text1.txt 1*
	







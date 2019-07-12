# betriebsysteme
Creating a filesystem during the Operating Systems Exercise

# download project 
how to git? information about all steps https://git-scm.com/book/de/v1/Los-geht%E2%80%99s
1) install git
2) register on GitHub
3) write in terminal
git config --global user.name "Your Name here"
git config --global user.email "your_email@domainname.com"
4) git clone https://github.com/Tmozovka/betriebsysteme.git
5)https://jameschambers.co/writing/git-team-workflow-cheatsheet/valgrind ./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt ./input/in/* ./input/in2/*

Fehle beim Speicher Allockeren anschauen :
1)sudo apt-get install valgrind
2)valgrind ./mkfs.myfs container.bin text1.txt text2.txt oder valgrind ./mkfs.myfs container.bin text1.txt ./text2.txt ./input/text3.txt ./input/in/* ./input/in2/*
3)valgrind ./mount.myfs container.bin logfile ./mount-dir -s -f

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
	







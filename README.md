# betriebsysteme
how to git? information about all steps https://git-scm.com/book/de/v1/Los-geht%E2%80%99s
1) install git
2) register on GitHub
3) write in terminal
git config --global user.name "Your Name here"
git config --global user.email "your_email@domainname.com"
4) git clone https://github.com/Tmozovka/betriebsysteme.git
5)https://jameschambers.co/writing/git-team-workflow-cheatsheet/

Fehle beim Speicher Allockeren anschauen :
1)sudo apt-get install valgrind
2)valgrind ./mkfs.myfs container.bin text1.txt text2.txt

System mounten:
1) rm container.bin
2)./mkfs.myfs container.bin text1.txt text2.txt
3)./mount.myfs container.bin logfile ./mount
4) cd mount
system testen 
5) cd ..
6)fusermount --unmount ./mount

Test-filesystem ausführen
1)Filesystem mounten
2)Im Ordner test-fileesystem Terminal Öffen
 	g++ test-filesystem.cpp 
3)dann, text1.txt ist datei im mountordner, auf der Tests ausgeführt werden 
	./a.out text1.txt







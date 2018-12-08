#include "fat.h"

void MyFAT::showFat()
{
	printf("****************************************************************\n");
			printf("FAT: \n");
	for(int i=0;i!=50;i++)
	{

		printf("%i -> %i \n", i,table[i]);
	}
}

int  MyFAT::unLink ( int current){ //todo fehle behandeln + header bearbeiten
	if(current>=size||current<0)
	return-1;
	else
	table[current]=-1;
	return 0;

}

 int MyFAT::link (int current, int* next) {

	 if(*next>=size||*next<0||current>=size||current<0)
	 return-1;
	 else
	 table[current]=*next;
	 return 0;
}

 int MyFAT::getNext(int current, int*next) {
 	 if(*next>=size||*next<0||current>=size||current<0)
 		 return-1;
 	 else
 		 *next=table[current];
 	 return 0;
 }


MyFAT::MyFAT()
{

for(int i=0;i<size;i++)
	table[i]=-1;

printf("Konstruktor von MyFat ist beendet \n");

}

MyFAT::~MyFAT()
{
	for(int i=0;i<size;i++)
		table[i]=-1;

	printf("Destruktor von MyFat ist beendet \n");
}

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <iterator>

int main(int argc, char *argv[]) {

std::string arg1(argv[1]);
std::string Input_x(argv[2]);
std::string Input_y(argv[3]);

    //call Progman as the parent directory
 HWND hwnd=FindWindowA("Progman", NULL);

    //from within Progman call ShellDLL_DefView as an inner child directory
 hwnd = FindWindowEx(hwnd, 0, "SHELLDLL_DefView", NULL);

    //from within SHELLDLL_DefView call SysListView32 as the final inner child direcotry
 hwnd = FindWindowEx(hwnd, NULL, "SysListView32", NULL);

    //gather a list of all our destkop icons
 int count=(int)SendMessage(hwnd, LVM_GETITEMCOUNT, 0, 0);

    //get the items desktop files/folders/links etc
 LVITEM lvi, *_lvi;
    
    //allocate 512 bytes for string names
 char item[512], subitem[512];

    //objects to create for the listview to recognize our commands to grab all of them
 char *_item, *_subitem;

    //we need to get the right pid for listview to get all the stuffs
 unsigned long pid;

    //how we negotiat the cpu via taskmanager
 HANDLE process;

    //Start the process to inject into listview for desktop icons
 GetWindowThreadProcessId(hwnd, &pid);

    //Make sure we can read all the necessary operations
 process=OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ| PROCESS_VM_WRITE|PROCESS_QUERY_INFORMATION, FALSE, pid);

    //Allocate Memory for each process /Begin
 _lvi=(LVITEM*)VirtualAllocEx(process, NULL, sizeof(LVITEM),MEM_COMMIT, PAGE_READWRITE);
 _item=(char*)VirtualAllocEx(process, NULL, 512, MEM_COMMIT,PAGE_READWRITE);
 _subitem=(char*)VirtualAllocEx(process, NULL, 512, MEM_COMMIT,PAGE_READWRITE);

 lvi.cchTextMax=512;
    //############################### /End

 //create an array to gather the shortcut names from the for loop
std::list<int> desktopArrayInt;

//i is to be part of the forloop
 int i;

//forloop to get the shortcut desktop names
 for(i=0; i<count; i++) {
  lvi.iSubItem=0;
  lvi.pszText=_item;
  WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEM), NULL);
  SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

  lvi.iSubItem=1;
  lvi.pszText=_subitem;
  WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEM), NULL);
  SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

  ReadProcessMemory(process, _item, item, 512, NULL);
  ReadProcessMemory(process, _subitem, subitem, 512, NULL);

  //printf("%s - %s\n", item, subitem);
  
    if(strcmp(item,arg1.c_str())==0)
    {
        desktopArrayInt.push_back(i);
        //printf("%s - %s\n", item, subitem);
        //printf("yes!");
    }
 }

 VirtualFreeEx(process, _lvi, 0, MEM_RELEASE);
 VirtualFreeEx(process, _item, 0, MEM_RELEASE);
 VirtualFreeEx(process, _subitem, 0, MEM_RELEASE);

///////////////////////////////////////////// REPOSITION DESKOTP ICONS ////////////////////////

    HWND hd;

	HANDLE he;
	DWORD Pi;

	hd = FindWindowA("Progman", NULL);

	hd = FindWindowEx(hd, 0, "SHELLDLL_DefView", NULL);

	hd = FindWindowEx(hd, 0, "SysListView32", NULL);

	GetWindowThreadProcessId(hd, &Pi);
	he = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, Pi);

	POINT *pC = (POINT*) VirtualAllocEx(he, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(he, pC, &pC, sizeof(POINT), NULL);

    POINT pt;
	ReadProcessMemory(he, pC, &pt, sizeof(POINT), NULL);

   for(std::list<int>::iterator it = desktopArrayInt.begin(); it != desktopArrayInt.end(); it++)
   {
         //c = desktopArrayInt[b];
        ListView_SetItemPosition(hd, *it, std::stoi(Input_x), std::stoi(Input_y));
   }
        
    VirtualFreeEx(he, pC, 0, MEM_RELEASE);


////////////////////////////////////// END OF REPOSITION DESKOTP ICONS ////////////////////////

 return 0;
}
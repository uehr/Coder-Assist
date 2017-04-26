#include <windows.h>
#include <iostream>
#include <psapi.h>
#include <stdio.h>
#include <string.h>

#pragma comment( lib, "psapi.lib" )

#define app_name "Coder-Assist"
#define exe_name "Coder-Assist.exe"
#define toggle_key1 VK_LCONTROL // ctrl
#define toggle_key2 VK_LMENU // alt
#define toggle_key3 'A'
#define wait_nsec 500
#define OPEN_PROCESS  (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ)

using namespace std;

//二重起動防止
int check_process_overlap() {
    DWORD ProcessID[ 1024 ];
    DWORD dwSize;
    DWORD dwMax;
    DWORD dwNow;
    HANDLE hProcess;
    int result_buf = 0;

    // プロセス識別子の取得
    EnumProcesses( ProcessID, sizeof(ProcessID), &dwSize );
    dwMax = (dwSize / sizeof(DWORD));
    // プロセス識別子からプロセス名を列挙
    for ( dwNow = 0 ; dwNow < dwMax ; dwNow++ ){
        TCHAR   szFile[ 1024 ] = { 0 };
        HMODULE Module[ 1024 ] = { 0 };

        // プロセスのフルパス名を取得
        if ( (hProcess = OpenProcess(OPEN_PROCESS,FALSE,ProcessID[dwNow])) != NULL ){
            if ( EnumProcessModules(hProcess,Module,sizeof(Module),&dwSize) ){
                GetModuleFileNameEx( hProcess, Module[0], szFile, sizeof(szFile) );
            }
            CloseHandle( hProcess );
        }
        if(strstr(szFile, exe_name) != NULL)
          result_buf++;
    }

    //２つ以上プロセスが起動してたら1を返す
    if(result_buf > 1)
      return 1;

    return 0;
}

int add_aim_key(int check_key,int next_check_key,int enter_key){
  bool checked_key,
       checked_next_key,
       aim_key_pushing;

  aim_key_pushing= false;

  if(!next_check_key){
    if(GetAsyncKeyState(check_key) && 0x80)
      aim_key_pushing = true;
  }else{
    if(((GetAsyncKeyState(next_check_key) && 0x80) == 1) && ((GetAsyncKeyState(check_key) && 0x80) == 1)) 
      aim_key_pushing = true;
  }

  if(aim_key_pushing){
    //補完記号が シフト + Nキー ならシフトを押す
    if(VK_SHIFT == (check_key | next_check_key))
      keybd_event(VK_SHIFT,0,0,0);

    keybd_event(enter_key,0,0,0);
    keybd_event(enter_key,0,KEYEVENTF_KEYUP,0);

    if(VK_SHIFT == (check_key | next_check_key))
      keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);

    //補完後、一文字分左へ移動
    keybd_event(VK_LEFT,0,0,0);
    keybd_event(VK_LEFT,0,KEYEVENTF_KEYUP,0);

    Sleep(wait_nsec);
  }

  return 0;
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
  bool is_run = true;

  if(check_process_overlap() == 1) {
    MessageBeep( MB_OK );
    MessageBox(NULL, "is already started", app_name, MB_OK);
    return -1;
  };

  MessageBox(NULL, "Coder-Assist started", app_name, MB_OK);

  while(true){
    //トグルキーが押されるとon/off切り替え
    if((GetAsyncKeyState(toggle_key1) && 0x80) &&
       (GetAsyncKeyState(toggle_key2) && 0x80) &&
       (GetAsyncKeyState(toggle_key3) && 0x80))
    {
      is_run = !is_run;
      Sleep(1000);
    }

    if(is_run){
        add_aim_key(VK_OEM_4, 0, VK_OEM_6); //[],「」, {}, 【】
        add_aim_key(VK_OEM_COMMA, VK_SHIFT, VK_OEM_PERIOD); //<>
        add_aim_key('8', VK_SHIFT, '9'); //()
        add_aim_key('2', VK_SHIFT, '2'); //""
        add_aim_key('7', VK_SHIFT, '7'); //''
    }
    Sleep(10);
  }
}

#include <windows.h>
#include <iostream>

#define toggle_key1 VK_LCONTROL
#define toggle_key2 VK_LMENU
#define toggle_key3 'A'
#define wait_nsec 300

using namespace std;

bool checked_key, aim_key_pushing, is_run = true;

int add_aim_key(int check_key,int next_check_key,int put_key,bool shift_pushing){
  aim_key_pushing= false;
  checked_key = GetAsyncKeyState(check_key) && 0x80;

  if(!next_check_key){
    if(checked_key)
      aim_key_pushing = true;
  }else{
    if(checked_key && (GetAsyncKeyState(next_check_key) && 0x80))
      aim_key_pushing = true;
  }

  if(aim_key_pushing){
    if(shift_pushing){
      keybd_event(VK_SHIFT,0,0,0);
    }
    keybd_event(put_key,0,0,0);
    keybd_event(put_key,0,KEYEVENTF_KEYUP,0);
    if(shift_pushing){
      keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);
    }
    keybd_event(VK_LEFT,0,0,0);
    keybd_event(VK_LEFT,0,KEYEVENTF_KEYUP,0);
    Sleep(wait_nsec);
  }
  return 0;
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
  while(true){
    if((GetAsyncKeyState(toggle_key1) && 0x80) &&
       (GetAsyncKeyState(toggle_key2) && 0x80) &&
       (GetAsyncKeyState(toggle_key3) && 0x80))
    {
      is_run = !is_run;
      Sleep(1000);
    }
    if(is_run){
        add_aim_key(VK_OEM_4, 0, VK_OEM_6, false);
        add_aim_key(VK_SHIFT, VK_OEM_COMMA, VK_OEM_PERIOD, true);
        add_aim_key('8', VK_SHIFT,'9', true);
        add_aim_key('2', VK_SHIFT,'2', true);
        add_aim_key('7', VK_SHIFT,'7', true);
    }
    Sleep(10);
  }
}

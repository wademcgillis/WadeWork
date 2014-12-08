#include <WadeWork/gui.h>
#include <windows.h>
#include <cstdio>
#include <CommCtrl.h>
#include <vector>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
						version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df'\
						language='*'\"")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"msimg32.lib")

namespace ww
{
	namespace gui
	{
		typedef struct {
			HWND hwnd;
			int id;
			int value;
		} ControlEvent;

		void paint_rectangle(int,int,int,int,unsigned int);

		const unsigned int ELEMENTS = 128;
		const unsigned int MAX_ELEMENTS_TO_BE_UPDATED_PER_TICK = 64;

		bool initialized = false;
		bool wmpainting;

		HBITMAP images[ELEMENTS];
		int IMAGE_ID = 0;

		HIMAGELIST imagelists[ELEMENTS];
		int IMAGELIST_ID = 0;

		HTREEITEM treeitems[ELEMENTS];
		int TREEITEM_ID = 0;

		HWND hwnds[ELEMENTS*3];
		int HWND_ID = 0;

		HMENU menus[ELEMENTS];
		int MENU_ID = 0;

		std::vector<int> controlgroups[ELEMENTS];
		unsigned int CONTROLGROUP_ID = 0;

		int scrollbars_position;
		int updowns_delta[ELEMENTS];
		int comboboxes_activeindex[ELEMENTS];
		int listboxes_activeindex[ELEMENTS];
		bool windowhandle_isopen[ELEMENTS];
		bool handle_is_tab[ELEMENTS];
		int treeviews_selectedindex[ELEMENTS];

		ControlEvent controlevents[MAX_ELEMENTS_TO_BE_UPDATED_PER_TICK];
		unsigned int controlevents_count;

		void (*paintFuncPtr)(void);

		WNDCLASSEX wc;
		HINSTANCE hinstance;

		HDC imagedraw_srcHDC;
		HDC imagedraw_destHDC;
		BLENDFUNCTION wmpainting_bfn;

		HBRUSH rectbrush;
		unsigned int rectcolor;

		void set_paint_function(void (*a)(void))
		{
			paintFuncPtr = a;
		}

		int _mouse_x;
		int _mouse_y;
		bool _mouse_left_pressed;
		bool _mouse_left_down;
		bool _mouse_left_released;

		bool mouse_left_pressed(int x, int y, int w, int h)
		{
			if (_mouse_left_pressed)
				if (_mouse_x >= x && _mouse_x < x+w && _mouse_y >= y && _mouse_y < y+h)
					return true;
			return false;
		}
		bool mouse_left_down(int x, int y, int w, int h)
		{
			if (_mouse_left_down)
				if (_mouse_x >= x && _mouse_x < x+w && _mouse_y >= y && _mouse_y < y+h)
					return true;
			return false;
		}
		bool mouse_left_released(int x, int y, int w, int h)
		{
			if (_mouse_left_released)
				if (_mouse_x >= x && _mouse_x < x+w && _mouse_y >= y && _mouse_y < y+h)
					return true;
			return false;
		}
		int mouse_x()
		{
			return _mouse_x;
		}
		int mouse_y()
		{
			return _mouse_y;
		}

		LRESULT CALLBACK handle_win32_event(HWND Handle, UINT Message, WPARAM wParam, LPARAM lParam) // WndProc
		{
			char stuff[32];
			WORD wHI = HIWORD(wParam);
			WORD wLO = LOWORD(wParam);
			WORD lHI = HIWORD(lParam);
			WORD lLO = LOWORD(lParam);

			bool DEF = true;
			NMTREEVIEW *TV;
			switch (Message)
			{
			case WM_CREATE:
				break;
			case WM_PAINT:
				//printf("WM_PAINT. Handle = %i\n",Handle);
#if 0
				PAINTSTRUCT ps;
				HDC hdc;
				BITMAP bitmap;
				HDC hdcMem;
				HGDIOBJ oldBitmap;
				HGDIOBJ oldBitmap2;
				hdc = BeginPaint(Handle, &ps);
				hdcMem = CreateCompatibleDC(hdc);
				oldBitmap = SelectObject(hdcMem, images[2]);
				GetObject(images[2], sizeof(bitmap), &bitmap);
				BitBlt(hdc, 400, 100, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
				BitBlt(hdc, 500, 100, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
				SelectObject(hdcMem, oldBitmap);
				DeleteDC(hdcMem);
				EndPaint(Handle, &ps);
#else
				PAINTSTRUCT ps;
				imagedraw_destHDC = BeginPaint(Handle, &ps);
				if (paintFuncPtr != NULL)
					paintFuncPtr();
				EndPaint(Handle, &ps);
#endif
				break;
			case WM_LBUTTONDOWN:
				_mouse_x = lLO;
				_mouse_y = lHI;
				_mouse_left_pressed = true;
				_mouse_left_down = true;
				_mouse_left_released = false;
				break;
			case WM_LBUTTONUP:
				_mouse_x = lLO;
				_mouse_y = lHI;
				_mouse_left_pressed = false;
				_mouse_left_down = false;
				_mouse_left_released = true;
				break;
			case WM_MOUSEMOVE:
				_mouse_x = lLO;
				_mouse_y = lHI;
				break;
			case WM_CLOSE:
				DestroyWindow(Handle);
				for(unsigned int i=0;i<=HWND_ID;i++)
				{
					if (hwnds[i] == Handle)
					{
						windowhandle_isopen[i] = false;
						i = HWND_ID + 1;
					}
				}
				//PostQuitMessage(0);
				return 0;
				break;
			case WM_COMMAND:
				switch(wHI)
				{
				case BN_CLICKED:
					//printf("Button clicked! lParam: %u wLO: %i\n",lParam,wLO);
					int K;
					K = controlevents_count++;
					controlevents[K].hwnd = (HWND)lParam;
					controlevents[K].id = wLO;
					//printf("Button clicked! HANDLE TO CHECK IS %i\n",controlevents[K].hwnd);
					break;
				case CBN_SELCHANGE:
					//printf("CBN_SELCHANGE! lParam: %u wLO: %i\n",lParam);
					for(unsigned int i=0;i<=HWND_ID;i++)
						if (hwnds[i] == (HWND)lParam)
							comboboxes_activeindex[i] = (int)SendMessage((HWND)lParam,CB_GETCURSEL,0,0);

					for(unsigned int i=0;i<=HWND_ID;i++)
						if (hwnds[i] == (HWND)lParam)
							listboxes_activeindex[i] = (int)SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
					break;
				default:
					//if (wHI != 768 && wHI != 1024)
					//	printf("wHI not handled: %u\n",wHI);
					break;
				}
				break;
			case WM_NOTIFY:
				unsigned int code;
				code = ((NMHDR*)lParam)->code;
				HWND hwndFrom;
				hwndFrom = ((NMHDR*)lParam)->hwndFrom;
				//if (code == NM_CUSTOMDRAW)
				//	printf("POSTPAINT %i\n",Handle);
				if (code == UDN_DELTAPOS)
				{
					NMUPDOWN *bob;
					bob = (LPNMUPDOWN) lParam;
					for(unsigned int i=0;i<=HWND_ID;i++)
					{
						if (hwnds[i] == hwndFrom)
						{
							updowns_delta[i] = bob->iDelta;
							i = HWND_ID+1;
						}
					}
				}
				if (code == TCN_SELCHANGE)
				{
					//printf("Button clicked! lParam: %u wLO: %i\n",lParam,wLO);
					int K;
					K = controlevents_count++;
					controlevents[K].hwnd = (HWND)SendMessage(((NMHDR*)lParam)->hwndFrom,TCM_GETCURSEL,0,0);
					controlevents[K].id = -1;
					//printf("SelChg clicked! HANDLE TO CHECK IS %i\n",controlevents[K].hwnd);
				}
				if (code == TVN_SELCHANGED)
				{
					TV = (NMTREEVIEW*)lParam;
					int treeitemindex = -1;
					for(unsigned int i=0;i<=TREEITEM_ID;i++)
					{
						if (treeitems[i] == TV->itemNew.hItem)
						{
							treeitemindex = i;
							i = TREEITEM_ID + 1;
						}
					}
					for(unsigned int i=0;i<=HWND_ID;i++)
					{
						if (hwnds[i] == hwndFrom)
						{
							treeviews_selectedindex[i] = treeitemindex;
							i = HWND_ID+1;
						}
					}
				}
				break;
			case WM_HSCROLL: case WM_VSCROLL:
				//printf("V wHi = %i\twLO = %i\tlHi = %i\tlLO = %i\n",wHI,wLO,lHI,lLO);//,SetScrollPos((HWND)lParam,SB_CTL,wHI,TRUE));
				if (wLO == 5)
					SetScrollPos((HWND)lParam,SB_CTL,wHI,TRUE);
				else if (wLO == 1)
				{
					int minval = 0;
					int maxval = 0;
					GetScrollRange((HWND)lParam,SB_CTL,&minval,&maxval);
					int pos = GetScrollPos((HWND)lParam,SB_CTL);
					if (pos < maxval)
						pos++;
					else
						pos = maxval;
					SetScrollPos((HWND)lParam,SB_CTL,pos,TRUE);
				}
				else if (wLO == 0)
				{
					int minval = 0;
					int maxval = 0;
					GetScrollRange((HWND)lParam,SB_CTL,&minval,&maxval);
					int pos = GetScrollPos((HWND)lParam,SB_CTL);
					if (pos > minval)
						pos--;
					else
						pos = minval;
					SetScrollPos((HWND)lParam,SB_CTL,pos,TRUE);
				}
				return 0;
				break;
			}
			if (DEF)
				return DefWindowProc(Handle, Message, wParam, lParam);
			else
				return 0;
		}

		LRESULT CALLBACK mySubClassProc(HWND Handle, UINT Message, WPARAM wParam, LPARAM lParam, UINT_PTR SubClass, DWORD_PTR RefData)
		{
			char stuff[32];
			WORD wHI = HIWORD(wParam);
			WORD wLO = LOWORD(wParam);
			WORD lHI = HIWORD(lParam);
			WORD lLO = LOWORD(lParam);
			switch(Message)
			{
			case WM_CTLCOLORSTATIC:
				GetClassNameA((HWND)lParam,(LPSTR)&stuff,32);
				if (strcmp(stuff,"Static") == 0)
				{
					RECT rect;
					GetClientRect(Handle, &rect);
					FillRect((HDC)wParam, &rect, GetSysColorBrush(-1));
					return 0;
				}
				break;
			case WM_COMMAND:// case WM_CTLCOLORSTATIC:
				handle_win32_event(Handle, Message,wParam,lParam);
				break;
			case WM_HSCROLL: case WM_VSCROLL:
				//printf("V wHi = %i\twLO = %i\tlHi = %i\tlLO = %i\n",wHI,wLO,lHI,lLO);//,SetScrollPos((HWND)lParam,SB_CTL,wHI,TRUE));
				if (wLO == 5)
					SetScrollPos((HWND)lParam,SB_CTL,wHI,TRUE);
				else if (wLO == 1)
				{
					int minval = 0;
					int maxval = 0;
					GetScrollRange((HWND)lParam,SB_CTL,&minval,&maxval);
					int pos = GetScrollPos((HWND)lParam,SB_CTL);
					if (pos < maxval)
						pos++;
					else
						pos = maxval;
					SetScrollPos((HWND)lParam,SB_CTL,pos,TRUE);
				}
				else if (wLO == 0)
				{
					int minval = 0;
					int maxval = 0;
					GetScrollRange((HWND)lParam,SB_CTL,&minval,&maxval);
					int pos = GetScrollPos((HWND)lParam,SB_CTL);
					if (pos > minval)
						pos--;
					else
						pos = minval;
					SetScrollPos((HWND)lParam,SB_CTL,pos,TRUE);
				}
				return 0;
				break;
			}
			return DefSubclassProc(Handle, Message, wParam, lParam);
		}

		void initialize()
		{
			if (initialized)
				return;
			initialized = true;
			wmpainting = false;
			paintFuncPtr = NULL;
			rectcolor = 0x00FFFFFF;

			rectbrush = CreateSolidBrush(rectcolor);
			wmpainting_bfn.BlendOp = AC_SRC_OVER;
			wmpainting_bfn.BlendFlags = 0;
			wmpainting_bfn.SourceConstantAlpha = 255;
			wmpainting_bfn.AlphaFormat = AC_SRC_ALPHA;

			imagedraw_srcHDC = CreateCompatibleDC(NULL);

			InitCommonControlsEx(NULL);
			hinstance = (HINSTANCE)GetWindowLong(NULL, GWL_HINSTANCE);

			wc.cbSize        = sizeof(WNDCLASSEX);
			wc.style         = 0;
			wc.lpfnWndProc   = handle_win32_event;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = hinstance;
			wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH) (COLOR_WINDOW);
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = "WUMBOGUI_WIN32_WINDOW";
			wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

			if(!RegisterClassEx(&wc))
				MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		}

		void handle_messages(bool arg)
		{
			controlevents_count = 0;
			for(unsigned int i=0;i<=HWND_ID;i++)
				updowns_delta[i] = 0;
			_mouse_left_pressed = false;
			_mouse_left_released = false;
			if (!arg)
				return;
			MSG Message;
			Message.message = ~WM_QUIT;
			while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
		}

		HWND get_handle(int element)
		{
			return hwnds[element];
		}

		void enable_control(int control)
		{
			EnableWindow(hwnds[control],true);
		}

		void disable_control(int control)
		{
			EnableWindow(hwnds[control],false);
		}

		void hide_control(int control)
		{
			ShowWindow(hwnds[control],SW_HIDE);
		}

		void show_control(int control)
		{
			ShowWindow(hwnds[control],SW_SHOW);
		}

		void control_sendtoback(int element)
		{
			SetWindowPos(hwnds[element],HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}

		void show_OKdialog(const char* text, const char *caption, int icon)
		{
			DWORD iconflag = 0;
			switch(icon)
			{
			case 1:
				iconflag = MB_ICONASTERISK;
				break;
			case 2:
				iconflag = MB_ICONERROR;
				break;
			case 3:
				iconflag = MB_ICONEXCLAMATION;
				break;
			case 4:
				iconflag = MB_ICONHAND;
				break;
			case 5:
				iconflag = MB_ICONINFORMATION;
				break;
			case 6:
				iconflag = MB_ICONMASK;
				break;
			case 7:
				iconflag = MB_ICONQUESTION;
				break;
			case 8:
				iconflag = MB_ICONSTOP;
				break;
			case 9:
				iconflag = MB_ICONWARNING;
				break;
			}
			MessageBoxA(NULL,text,caption,MB_OK | iconflag);
		}

		bool show_YESNOdialog(const char* text, const char *caption, int icon)
		{
			DWORD iconflag = 0;
			switch(icon)
			{
			case 1:
				iconflag = MB_ICONASTERISK;
				break;
			case 2:
				iconflag = MB_ICONERROR;
				break;
			case 3:
				iconflag = MB_ICONEXCLAMATION;
				break;
			case 4:
				iconflag = MB_ICONHAND;
				break;
			case 5:
				iconflag = MB_ICONINFORMATION;
				break;
			case 6:
				iconflag = MB_ICONMASK;
				break;
			case 7:
				iconflag = MB_ICONQUESTION;
				break;
			case 8:
				iconflag = MB_ICONSTOP;
				break;
			case 9:
				iconflag = MB_ICONWARNING;
				break;
			}
			return (MessageBoxA(NULL,text,caption,MB_YESNO | iconflag) == IDYES);
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//												CONTROL GROUPS												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int controlgroup_create()
		{
			controlgroups[CONTROLGROUP_ID].push_back(1);
			CONTROLGROUP_ID++;
			return CONTROLGROUP_ID-1;
		}

		void controlgroup_addcontrol(int controlgroup, int control)
		{
			if (controlgroup >= CONTROLGROUP_ID)
				return;
			controlgroups[controlgroup].push_back(control);
		}

		void controlgroup_removecontrol(int controlgroup, int control)
		{
			if (controlgroup >= CONTROLGROUP_ID)
				return;
			for(int i=1;i<controlgroups[controlgroup].size();i++)
				if (controlgroups[controlgroup][i] == control)
					controlgroups[controlgroup].erase(controlgroups[controlgroup].begin()+i);
		}

		bool controlgroup_getvisible(int controlgroup)
		{
			if (controlgroup >= CONTROLGROUP_ID)
				return 0;
			return controlgroups[controlgroup][0];
		}

		void controlgroup_setvisible(int controlgroup, bool visible)
		{
			if (controlgroup >= CONTROLGROUP_ID)
				return;
			if (controlgroups[controlgroup][0] == visible)
				return;
			controlgroups[controlgroup][0] = visible;

			if (controlgroups[controlgroup].size() == 1)
				return;
			if (visible)
			{
				for(int i=1;i<controlgroups[controlgroup].size();i++)
					show_control(controlgroups[controlgroup].at(i));
			}
			else
			{
				for(int i=1;i<controlgroups[controlgroup].size();i++)
					hide_control(controlgroups[controlgroup].at(i));
			}
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													MENUS													  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int menu_create()
		{
			MENU_ID++;
			menus[MENU_ID] = CreateMenu();
			return MENU_ID;
		}

		HMENU menu_get_menu(int menu)
		{
			return menus[menu];
		}

		int menu_append_submenu(int menu, const char *text)
		{
			MENU_ID++;
			menus[MENU_ID] = CreatePopupMenu();
			AppendMenu(menus[menu], MF_STRING | MF_POPUP, (UINT)menus[MENU_ID], text);
			return MENU_ID;
		}

		int submenu_append_string(int submenu, const char *text)
		{
			MENU_ID++;
			AppendMenu(menus[submenu], MF_STRING, MENU_ID, text);
			return MENU_ID;
		}

		int submenu_append_separator(int submenu)
		{
			MENU_ID++;
			MENUITEMINFO info;
			info.cbSize = sizeof(MENUITEMINFO);
			info.fMask = MIIM_FTYPE;
			info.fType = MFT_SEPARATOR;
			info.cch = 0;
			InsertMenuItem(menus[submenu],MENU_ID,FALSE,&info);
			return MENU_ID;
		}

		bool menu_check(int itemID)
		{
			for(unsigned int i=0;i<controlevents_count;i++)
				if (controlevents[i].id == itemID)
					return true;
			return false;
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													WINDOW													  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		void FIXWINDOWSIZE(HWND hwnd, int width, int height)
		{
			RECT inner, outer;
			GetWindowRect(hwnd, &outer);
			GetClientRect(hwnd, &inner);
			//printf("Window: %i %i %i %i\n",outer.left,outer.top,outer.right-outer.left,outer.bottom-outer.top);
			//printf("Client: %i %i %i %i\n",inner.left,inner.top,inner.right-inner.left,inner.bottom-inner.top);
			int dx = (outer.right - outer.left) - inner.right;
			//printf("Style: %u\n",GetWindowLong(hwnd,GWL_STYLE));
			//printf("MENU: %u\n",WS_SYSMENU);
			//printf(" S&M: %u\n",GetWindowLong(hwnd,GWL_STYLE) & WS_SYSMENU);
			int h = 0;
			if ((GetWindowLong(hwnd,GWL_STYLE) & WS_SYSMENU) > 0)
				h = GetSystemMetrics(SM_CYMENU);
			int dy = (outer.bottom - outer.top) - inner.bottom + h;
			MoveWindow(hwnd, outer.left, outer.top, width+dx, height+dy, true);
		}

		unsigned int window_create(int parent, unsigned int width, unsigned int height, const char *text)
		{
			HWND_ID++;
			NONCLIENTMETRICS bob;
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS,sizeof(NONCLIENTMETRICS),&bob,NULL);
			//printf("BorderWidth: %i\n",bob.iBorderWidth);
			//printf("MenuHeight: %i\n",bob.iMenuHeight);
			DWORD flags = 0;
			DWORD superflags = 0;
			if (parent != NULL)
			{
				//printf("WE GOT A PARENT\n");
				flags = 0;//WS_CAPTION | WS_SYSMENU;
				superflags = WS_EX_TOOLWINDOW | WS_EX_APPWINDOW;
			}
			else
			{
				flags = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
				superflags = WS_EX_CLIENTEDGE;
			}

				hwnds[HWND_ID] = CreateWindowExA(superflags,
					"WUMBOGUI_WIN32_WINDOW",
					text,
					flags,
					CW_USEDEFAULT, CW_USEDEFAULT, width, height,
					hwnds[parent], NULL, hinstance, NULL);
			
			/*hwnds[HWND_ID] = CreateWindowExA(WS_EX_TOOLWINDOW,
					"WUMBOGUI_WIN32_WINDOW",
					text,
					flags,
					CW_USEDEFAULT, CW_USEDEFAULT, width, height,
					hwnds[parent], NULL, hinstance, NULL);*/

			
			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			ShowWindow(hwnds[HWND_ID],SW_NORMAL);
			FIXWINDOWSIZE(hwnds[HWND_ID],width,height);
			UpdateWindow(hwnds[HWND_ID]);
			windowhandle_isopen[HWND_ID] = true;
			return HWND_ID;
		}

		void window_set_menu(int window, int menu)
		{
			SetMenu(hwnds[window],menus[menu]);
		}

		bool window_isopen(int window)
		{
			return windowhandle_isopen[window];
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//														TABS												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int tabcontroller_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			HWND_ID++;
			hwnds[HWND_ID] = CreateWindowExW(
					0, WC_TABCONTROLW, 
					NULL,
					WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
					x,y,width,height, 
					hwnds[parent],
					(HMENU)HWND_ID, 
					hinstance, 
					NULL);
			handle_is_tab[HWND_ID] = true;

			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			SetWindowSubclass(hwnds[HWND_ID], (SUBCLASSPROC)&mySubClassProc, 0, 0);
			return HWND_ID;
		}

		int tabcontroller_addtab(int parent, unsigned int index, const char *text)
		{
			HWND_ID++;
			TCITEM tcitem;
			tcitem.mask = TCIF_TEXT;
			tcitem.pszText =(char*)text;
			tcitem.lParam = HWND_ID;
			hwnds[HWND_ID] = (HWND)SendMessage(hwnds[parent],(UINT) TCM_INSERTITEMA,(WPARAM) HWND_ID,(LPARAM) &tcitem);
			return HWND_ID;
		}

		bool tab_check(int tab)
		{
			for(unsigned int i=0;i<controlevents_count;i++)
				if (controlevents[i].hwnd == hwnds[tab])
					return true;
			return false;
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													COMBO BOX												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int combobox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			HWND_ID++;
			hwnds[HWND_ID] = CreateWindowExW(
									0, WC_COMBOBOXW,
									NULL, 
									CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL, 
									x,y,width,height, 
									hwnds[parent],
									(HMENU)HWND_ID,
									hinstance, 
									NULL);

			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			comboboxes_activeindex[HWND_ID] = -1;
			return HWND_ID;
		}

		int combobox_addstring(int combobox, const char *text)
		{
			//HWND_ID++;
			SendMessage(hwnds[combobox],(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) text);
			return SendMessage(hwnds[combobox],(UINT) CB_GETCOUNT,(WPARAM) 0,(LPARAM) 0)-1;
		}

		void combobox_deletestring(int combobox, int index)
		{
			SendMessage(hwnds[combobox],(UINT) CB_DELETESTRING,(WPARAM) index,(LPARAM) 0);
		}

		int combobox_getselectedindex(int combobox)
		{
			return comboboxes_activeindex[combobox];
		}

		void combobox_setselectedindex(int combobox, int index)
		{
			comboboxes_activeindex[combobox] = index;
			SendMessage(hwnds[combobox],CB_SETCURSEL,index,0);
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													LIST BOX												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int listbox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			HWND_ID++;
			hwnds[HWND_ID] = CreateWindowExW(
									0, WC_LISTBOXW, 
									L"clams", 
									LBS_NOTIFY | LBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | WS_BORDER, 
									x,y,width,height, 
									hwnds[parent], 
									(HMENU)HWND_ID, 
									hinstance, 
									NULL); 

			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			listboxes_activeindex[HWND_ID] = -1;
			return HWND_ID;
		}

		int listbox_addstring(int listbox, const char *text)
		{
			//HWND_ID++;
			SendMessage(hwnds[listbox],(UINT) LB_ADDSTRING,(WPARAM) 0,(LPARAM) text);
			return SendMessage(hwnds[listbox],(UINT) LB_GETCOUNT,(WPARAM) 0,(LPARAM) 0)-1;
		}

		void listbox_deletestring(int listbox, int index)
		{
			SendMessage(hwnds[listbox],(UINT) LB_DELETESTRING,(WPARAM) index,(LPARAM) 0);
		}

		void listbox_setstring(int listbox, int index, const char *text)
		{
			SendMessage(hwnds[listbox],(UINT) LB_DELETESTRING,(WPARAM) index,(LPARAM) 0);
			SendMessage(hwnds[listbox],(UINT) LB_INSERTSTRING,(WPARAM) index,(LPARAM) text);
		}

		int listbox_getselectedindex(int listbox)
		{
			return listboxes_activeindex[listbox];
		}

		void listbox_setselectedindex(int listbox, int index)
		{
			listboxes_activeindex[listbox] = index;
			SendMessage(hwnds[listbox],LB_SETCURSEL,index,0);
		}

		int listbox_getstringcount(int listbox)
		{
			return SendMessage(hwnds[listbox],LB_GETCOUNT,0,0);
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													TEXT													  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int text_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char *text)
		{
			HWND_ID++;
			DWORD flags = 0;
			/*if (readonly)
				flags = flags | ES_READONLY;
			if (multiline)
				flags = flags | ES_MULTILINE;
			if (hscroll)
				flags = flags | WS_HSCROLL | ES_AUTOHSCROLL;
			if (vscroll)
				flags = flags | WS_VSCROLL | ES_AUTOVSCROLL;
			if (border)
				flags = flags | WS_BORDER;*/
			hwnds[HWND_ID] = CreateWindowExA(
									0, WC_STATICA, 
									text,
									WS_CHILD | WS_VISIBLE | ES_LEFT | flags, 
									x,y,width,height, 
									hwnds[parent], 
									(HMENU)HWND_ID, 
									hinstance, 
									NULL); 

			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			return HWND_ID;
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													TEXT BOX												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int textbox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool readonly, bool multiline, int hscroll, int vscroll, bool border)
		{
			HWND_ID++;
			DWORD exflags = 0;
			DWORD flags = 0;
			if (readonly)
				flags = flags | ES_READONLY;
			if (multiline)
				flags = flags | ES_MULTILINE;
			if (hscroll > 0)
				flags = flags | ES_AUTOHSCROLL;
			if (hscroll > 1)
				flags = flags | WS_HSCROLL;
			if (vscroll > 0)
				flags = flags | ES_AUTOVSCROLL;
			if (vscroll > 1)
				flags = flags | WS_VSCROLL;
			if (border)
				exflags = exflags | WS_EX_CLIENTEDGE;
			//WS_BORDER;
			hwnds[HWND_ID] = CreateWindowExW(
									exflags, WC_EDITW, 
									NULL,
									WS_CHILD | WS_VISIBLE | ES_LEFT | flags,// | WS_EX_TRANSPARENT, 
									x,y,width,height, 
									hwnds[parent], 
									(HMENU)HWND_ID, 
									hinstance, 
									NULL); 

			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			return HWND_ID;
		}

		void textbox_settext(int parent, const char *text)
		{
			SendMessage(hwnds[parent], WM_SETTEXT, 0, (LPARAM) text);
		}

		void textbox_gettext(int parent, char *dest, unsigned int length)
		{
			if (length == 0)
				return;
			SendMessage(hwnds[parent], WM_GETTEXT, (WPARAM)length, (LPARAM)dest);
		}

		void textbox_setmaxlength(int parent, unsigned int length)
		{
			SendMessage(hwnds[parent], EM_SETLIMITTEXT, (WPARAM)length, 0);
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													BUTTON BOX												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int button_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char *text)
		{
			MENU_ID++;
			HWND_ID++;
			//printf("HWND = %i\n",HWND_ID);
			hwnds[HWND_ID] = CreateWindowExA(0,WC_BUTTONA,text,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
				x,y,width,height,hwnds[parent],(HMENU)HWND_ID,(HINSTANCE)GetWindowLong(NULL, GWL_HINSTANCE),NULL);

			
			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			return HWND_ID;
		}

		int button_create_bitmap(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int bitmap)
		{
			MENU_ID++;
			HWND_ID++;
			hwnds[HWND_ID] = CreateWindowExA(0,WC_BUTTON,NULL,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_BITMAP,
				x,y,width,height,hwnds[parent],(HMENU)HWND_ID,(HINSTANCE)GetWindowLong(NULL, GWL_HINSTANCE),NULL);

			SendMessage(hwnds[HWND_ID], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[bitmap]);
			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			return HWND_ID;
		}

		bool button_check(int button)
		{
			for(unsigned int i=0;i<controlevents_count;i++)
				if (controlevents[i].hwnd == hwnds[button])
					return true;
			return false;
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													BUTTON BOX												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int groupbox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char *text)
		{
			MENU_ID++;
			HWND_ID++;
			hwnds[HWND_ID] = CreateWindowExA(0,WC_BUTTONA,text,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_GROUPBOX | WS_CLIPSIBLINGS,
				x,y,width,height,hwnds[parent],(HMENU)HWND_ID,(HINSTANCE)GetWindowLong(NULL, GWL_HINSTANCE),NULL);

			SetWindowPos(hwnds[HWND_ID],HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			return HWND_ID;
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													UPDOWN													  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int updown_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			MENU_ID++;
			HWND_ID++;
			hwnds[HWND_ID] = CreateWindowExA(0,UPDOWN_CLASS,NULL,
				/*WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPSIBLINGS,*/WS_CHILDWINDOW | WS_VISIBLE
                              /*| UDS_AUTOBUDDY | UDS_SETBUDDYINT*/ | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK,
				x,y,width,height,hwnds[parent],(HMENU)HWND_ID,(HINSTANCE)GetWindowLong(NULL, GWL_HINSTANCE),NULL);

			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			return HWND_ID;
		}

		int updown_get_delta(int updown)
		{
			if (updown > HWND_ID)
				return 0;
			return updowns_delta[updown];
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													TREEVIEW												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int treeview_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			HWND_ID++;
			hwnds[HWND_ID] = CreateWindowExA(
									0, WC_TREEVIEWA, 
									"clams", 
									TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | WS_CHILD | WS_VISIBLE | WS_BORDER, 
									x,y,width,height, 
									hwnds[parent], 
									(HMENU)HWND_ID, 
									hinstance, 
									NULL); 

			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			//listboxes_activeindex[HWND_ID] = -1;
			return HWND_ID;
		}

		int treeview_addstring(int parent, const char *text, int indentLevel, int image)
		{
			TREEITEM_ID++;

			TVITEM item; 
			TVINSERTSTRUCT insert; 
			static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
			static HTREEITEM hPrevRootItem = NULL; 
			static HTREEITEM hPrevLev2Item = NULL; 
			HTREEITEM hti; 

			item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM; 

			item.pszText = (LPSTR)text; 
			item.cchTextMax = strlen(text); 

			item.iImage = image;//g_nDocument; 
			item.iSelectedImage = image;//g_nDocument; 

			// Save the heading level in the item's application-defined 
			// data area. 
			item.lParam = (LPARAM)indentLevel; 
			insert.item = item; 
			insert.hInsertAfter = hPrev; 

			// Set the parent item based on the specified level. 
			if (indentLevel == 1) 
				insert.hParent = TVI_ROOT; 
			else if (indentLevel == 2) 
				insert.hParent = hPrevRootItem; 
			else 
				insert.hParent = hPrevLev2Item; 

			// Add the item to the tree-view control. 
			hPrev = (HTREEITEM)SendMessage(hwnds[parent], TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&insert); 

			if (hPrev == NULL)
				return NULL;

			// Save the handle to the item. 
			if (indentLevel == 1) 
				hPrevRootItem = hPrev; 
			else if (indentLevel == 2) 
				hPrevLev2Item = hPrev; 

			// The new item is a child item. Give the parent item a 
			// closed folder bitmap to indicate it now has child items. 
			if (indentLevel > 1)
			{ 
				hti = TreeView_GetParent(hwnds[parent], hPrev); 
				item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
				item.hItem = hti; 
				item.iImage = -1;//g_nClosed; 
				item.iSelectedImage = -1;//g_nClosed; 
				TreeView_SetItem(hwnds[parent], &item);
			} 

			treeitems[TREEITEM_ID] = hPrev;
			if (indentLevel == 1)
			{//IDI_
				TVITEM tvitem;
				tvitem.mask = TVIF_STATE;
				tvitem.hItem = hPrev;
				tvitem.stateMask = TVIS_STATEIMAGEMASK | TVIS_BOLD | TVIS_EXPANDED;
				tvitem.state = TVIS_STATEIMAGEMASK | TVIS_BOLD | TVIS_EXPANDED;
				tvitem.lParam = 1;
				SendMessage(hwnds[parent],TVM_SETITEM,0,(LPARAM)&tvitem);
			}

			return TREEITEM_ID; 
		}

		void treeview_setimagelist(int parent, int imagelist)
		{
			TreeView_SetImageList(hwnds[parent], imagelists[imagelist], TVSIL_NORMAL); 
		}

		int treeview_getselecteditem(int parent)
		{
			if (parent <= HWND_ID)
				return treeviews_selectedindex[parent];
			return -1;
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													IMAGE LIST												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int imagelist_create(int width, int height)
		{
			IMAGELIST_ID++;
			imagelists[IMAGELIST_ID] = ImageList_Create(width,height,ILC_COLOR | ILC_COLOR32,1,1);
			return IMAGELIST_ID;
		}
		int imagelist_addimage(int imagelist, int image)
		{
			return ImageList_Add(imagelists[imagelist],images[image],NULL);
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													IMAGE BOX												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************


		int imagebox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			HWND_ID++;
			DWORD flags = 0;
			/*if (readonly)
				flags = flags | ES_READONLY;
			if (multiline)
				flags = flags | ES_MULTILINE;
			if (hscroll)
				flags = flags | WS_HSCROLL | ES_AUTOHSCROLL;
			if (vscroll)
				flags = flags | WS_VSCROLL | ES_AUTOVSCROLL;
			if (border)
				flags = flags | WS_BORDER;*/
			hwnds[HWND_ID] = CreateWindowExA(
									0, WC_STATICA, 
									"MOOSEFARTS",
									WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_BITMAP | WS_BORDER, 
									x,y,width,height, 
									hwnds[parent], 
									(HMENU)HWND_ID, 
									hinstance, 
									NULL); 

			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			return HWND_ID;
		}
		int imagebox_set_image_from_hbitmap(int parent, HBITMAP bitmap)
		{
			BITMAP dabitmap;
			GetObject(bitmap, sizeof(dabitmap), &dabitmap);
			//printf("set bitmap : w %i h %i ptr %X\n",dabitmap.bmWidth,dabitmap.bmHeight,(unsigned int)dabitmap.bmBits);
			//BitBlt(imagedraw_destHDC, x, y, bitmap.bmWidth, bitmap.bmHeight, imagedraw_srcHDC, 0, 0, SRCCOPY);


			SendMessage(hwnds[parent], STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bitmap);
			return 0;
			//SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													IMAGES													  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************

		int image_createfromptr(const unsigned char *ptr, unsigned int width, unsigned int height)
		{
			IMAGE_ID++;
			for(unsigned int i=0;i<width*height;i++)
				((unsigned int*)ptr)[i] = (((unsigned int*)ptr)[i] & 0xFF000000) << 0 | (((unsigned int*)ptr)[i] & 0x00FF0000) >> 16 | (((unsigned int*)ptr)[i] & 0x0000FF00) >> 0 | (((unsigned int*)ptr)[i] & 0x000000FF) << 16;
			BITMAPINFOHEADER header;
			header.biSize = sizeof(BITMAPINFOHEADER);
			header.biWidth = width;
			header.biHeight = -height;
			header.biPlanes = 1;
			header.biBitCount = 32;
			header.biCompression = BI_RGB;
			header.biSizeImage = 0;
			header.biXPelsPerMeter = 16;
			header.biYPelsPerMeter = 16;
			header.biClrUsed = 0;
			header.biClrImportant = 0;
			BITMAPINFO info;
			info.bmiHeader = header;
			info.bmiColors->rgbBlue = 0;
			info.bmiColors->rgbGreen = 0;
			info.bmiColors->rgbRed = 0;
			info.bmiColors->rgbReserved = 0;
			images[IMAGE_ID] = CreateDIBitmap(GetDC(NULL), &header, CBM_INIT, (void*)ptr, &info, DIB_RGB_COLORS);
			//DeleteObject(bitmap);
			for(unsigned int i=0;i<width*height;i++)
				((unsigned int*)ptr)[i] = (((unsigned int*)ptr)[i] & 0xFF000000) << 0 | (((unsigned int*)ptr)[i] & 0x00FF0000) >> 16 | (((unsigned int*)ptr)[i] & 0x0000FF00) >> 0 | (((unsigned int*)ptr)[i] & 0x000000FF) << 16;
			return IMAGE_ID;
		}
		int image_createfromptrsubrect(const unsigned char *ptr, unsigned int width, unsigned int height, unsigned int destX, unsigned int destY, unsigned int destWidth, unsigned int destHeight)
		{
			unsigned int *newptr = new unsigned int[destWidth*destHeight];
			for(unsigned int i=0;i<destWidth;i++)
				for(unsigned int j=0;j<destHeight;j++)
					newptr[destWidth*j + i] = ((unsigned int*)ptr)[width*(j+destY) + destX+i];
			int v = image_createfromptr((unsigned char*)newptr,destWidth,destHeight);
			delete newptr;
			return v;
		}

		void image_delete(int image)
		{
			DeleteObject(images[image]);
		}

		HBITMAP image_getbitmap(int image)
		{
			return images[image];
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													PAINTING												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************


		void paint_resetDC()
		{
			imagedraw_destHDC = NULL;
			//imagedraw_srcHDC = NULL;
		}
		void paint_setDC(HDC hdc)//, HDC hdcmem)
		{
			imagedraw_destHDC = hdc;
			//imagedraw_srcHDC = hdcmem;
		}

		void paint_rectangle(int x, int y, int w, int h, unsigned int color)
		{
			if (rectcolor != color)
			{
				DeleteObject(rectbrush);
				rectcolor = color;
				rectbrush = CreateSolidBrush(rectcolor);
			}
			RECT rect;
			rect.left = x;
			rect.top = y;
			rect.right = x+w;
			rect.bottom = y+h;
			FillRect(imagedraw_destHDC, &rect, rectbrush);
		}

		void paint_image(int image, int x, int y, int xscale, int yscale)
		{
			BITMAP bitmap;
			HGDIOBJ old = SelectObject(imagedraw_srcHDC, images[image]);
			GetObject(images[image], sizeof(bitmap), &bitmap);
			AlphaBlend(imagedraw_destHDC, x,y, bitmap.bmWidth*xscale, bitmap.bmHeight*yscale, imagedraw_srcHDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, wmpainting_bfn);
			//AlphaBlend(wmpainting_hdc, 500, 100, bitmap.bmWidth, bitmap.bmHeight, wmpainting_hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, wmpainting_bfn);
			SelectObject(imagedraw_srcHDC, old);
		}

		void paint_hbitmap(HBITMAP hbitmap, int x, int y)
		{
			BITMAP bitmap;
			HGDIOBJ old = SelectObject(imagedraw_srcHDC, hbitmap);
			GetObject(hbitmap, sizeof(bitmap), &bitmap);
			BitBlt(imagedraw_destHDC, x, y, bitmap.bmWidth, bitmap.bmHeight, imagedraw_srcHDC, 0, 0, SRCCOPY);
			SelectObject(imagedraw_srcHDC, old);
		}

		void paint_DC(HDC hdc, int x, int y, int width, int height)
		{
			BitBlt(imagedraw_destHDC, x, y, width, height, hdc, 0, 0, SRCCOPY);
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													SCROLLBARS												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************


		int scrollbar_create(int parent, int x, int y, int width, int height, bool horz)
		{
			HWND_ID++;
			MENU_ID++;
			DWORD flags = 0;
			if (horz)
				flags = flags | SBS_HORZ;// | WS_HSCROLL;
			else
				flags = flags | SBS_VERT;// | WS_VSCROLL;
			hwnds[HWND_ID] = CreateWindowExA(0,WC_SCROLLBARA,"",
				WS_CHILD | WS_VISIBLE | flags,
				x,y,width,height,hwnds[parent],(HMENU)HWND_ID,(HINSTANCE)GetWindowLong(NULL, GWL_HINSTANCE),NULL);
			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_RANGE;
			info.nMin = 45;
			info.nMax = 55;
			SetScrollInfo(hwnds[HWND_ID],SB_CTL,&info,TRUE);

			SetWindowSubclass(hwnds[HWND_ID], (SUBCLASSPROC)&mySubClassProc, 0, 0);
			
			return HWND_ID;
		}
		void scrollbar_setminmax(int parent, int minval, int maxval)
		{

		}
		int scrollbar_get_position(int parent)
		{
			return 0;
		}

		//	***********************************************************************************************************
		//																											  *
		//																											  *
		//													LISTVIEW												  *
		//																											  *
		//																											  *
		//	***********************************************************************************************************



		int listview_create(int parent, int x, int y, int width, int height, bool icon)
		{
			MENU_ID++;
			HWND_ID++;

			DWORD flags = 0;
			if (icon)
				flags |= LVS_ICON;
			else
				flags |= LVS_LIST;
			hwnds[HWND_ID] = CreateWindowExA(WS_EX_CLIENTEDGE,WC_LISTVIEWA,"MOOGLE",
				WS_CHILD | WS_VISIBLE | LVS_SHOWSELALWAYS | LVS_SINGLESEL | flags,
				x,y,width,height,hwnds[parent],(HMENU)HWND_ID,(HINSTANCE)GetWindowLong(NULL, GWL_HINSTANCE),NULL);

			
			SendMessage(hwnds[HWND_ID], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

			SIZE size = { 32,32 };
			LVTILEVIEWINFO tileViewInfo;


			tileViewInfo.cbSize   = sizeof(tileViewInfo);
			tileViewInfo.dwFlags  = LVTVIF_FIXEDSIZE;
			tileViewInfo.dwMask   = LVTVIM_COLUMNS | LVTVIM_TILESIZE;
			tileViewInfo.cLines   = 2;
			tileViewInfo.sizeTile = size;

			SendMessage(hwnds[HWND_ID],LVM_SETTILEVIEWINFO,0,(LPARAM) &tileViewInfo);

			return HWND_ID;
		}

		void listview_seticonspacing(int listview, int width, int height)
		{
			
			SendMessage(hwnds[listview],LVM_SETICONSPACING,0,MAKELONG(width,height));
		}

		void listview_setimagelist(int listview, int imagelist)
		{
			SendMessage(hwnds[listview], LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)imagelists[imagelist]); 
			SendMessage(hwnds[listview], LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)imagelists[imagelist]);
		}

		int listview_additem(int listview, char* text, int image)
		{
			LVITEM lvI;
			// Initialize LVITEM members that are common to all items.
			lvI.pszText   = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
			lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
			lvI.stateMask = 0;
			lvI.iSubItem  = 0;
			lvI.state     = 0;
			lvI.pszText   = text;

			lvI.iItem  = SendMessage(hwnds[listview],LVM_GETITEMCOUNT,0,0);
			lvI.iImage = image;
			SendMessage(hwnds[listview],LVM_INSERTITEM,0,(LPARAM)&lvI);
			return SendMessage(hwnds[listview],LVM_GETITEMCOUNT,0,0)-1;
		}

		int listview_get_selected(int listview)
		{
			return ListView_GetSelectionMark(ww::gui::get_handle(listview));
		}
	}
}
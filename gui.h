#ifndef __WadeWork__gui_h__
#define __WadeWork__gui_h__
#include <WadeWork/sys_defines.h>
#if PLATFORM_WINDOWS
#include <windows.h>
#else
typedef unsigned int HWND;
typedef unsigned int HMENU;
typedef unsigned int HBITMAP;
typedef unsigned int HDC;
#endif

namespace ww
{
	namespace gui
	{
		extern void set_paint_function(void (*a)(void));
		extern void initialize();
		extern void handle_messages(bool arg = true);
		// stuff
		extern HWND get_handle(int element);
		extern void control_sendtoback(int element);
		extern void hide_control(int control);
		extern void show_control(int control);
		extern void enable_control(int control);
		extern void disable_control(int control);
		// more stuff
		extern void show_OKdialog(const char* text, const char* caption, int icon = 0);
		extern bool show_YESNOdialog(const char* text, const char* caption, int icon = 0);
		// mouse
		extern bool mouse_left_pressed(int x, int y, int w, int h);
		extern bool mouse_left_down(int x, int y, int w, int h);
		extern bool mouse_left_released(int x, int y, int w, int h);
		extern int mouse_x();
		extern int mouse_y();
		// controlgroup
		int controlgroup_create();
		void controlgroup_addcontrol(int controlgroup, int control);
		void controlgroup_removecontrol(int controlgroup, int control);
		bool controlgroup_getvisible(int controlgroup);
		void controlgroup_setvisible(int controlgroup, bool visible);
		// menu
		extern int menu_create();
		extern HMENU menu_get_menu(int menu);
		extern int menu_append_submenu(int menu, const char *text);
		extern int submenu_append_string(int submenu, const char *text);
		extern int submenu_append_separator(int submenu);
		extern bool menu_check(int itemID);
		// window
		extern unsigned int window_create(int parent, unsigned int width, unsigned int height, const char *text);
		extern void window_set_menu(int window, int menu);
		extern bool window_isopen(int window);
		// tabs
		extern int tabcontroller_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		extern int tabcontroller_addtab(int parent, unsigned int index, const char *text);
		extern bool tab_check(int tab);
		// combobox
		extern int combobox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		extern int combobox_addstring(int combobox, const char *text);
		extern void combobox_deletestring(int combobox, int index);
		extern int combobox_getselectedindex(int combobox);
		extern void combobox_setselectedindex(int combobox, int index);
		// listbox
		extern int listbox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		extern int listbox_addstring(int listbox, const char *text);
		extern void listbox_setstring(int listbox, int index, const char *text);
		extern void listbox_deletestring(int listbox, int index);
		extern int listbox_getselectedindex(int listbox);
		extern void listbox_setselectedindex(int listbox, int index);
		extern int listbox_getstringcount(int listbox);
		// text
		extern int text_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char *text);
		// textbox
		extern int textbox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool readonly, bool multiline, int hscroll, int vscroll, bool border);
		extern void textbox_settext(int parent, const char *text);
		extern void textbox_gettext(int parent, char *dest, unsigned int length);
		extern void textbox_setmaxlength(int parent, unsigned int length);
		// button
		extern int button_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char *text);
		extern int button_create_bitmap(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int bitmap);
		extern bool button_check(int button);
		// groupbox
		extern int groupbox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char *text);
		// updown
		extern int updown_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		extern int updown_get_delta(int updown);
		// treeview
		extern int treeview_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		extern int treeview_addstring(int parent, const char *text, int indentLevel, int image = -1);
		extern int treeview_getselectedindex(int parent);
		extern void treeview_setimagelist(int parent, int imagelist);
		// image list
		extern int imagelist_create(int width, int height);
		extern int imagelist_addimage(int imagelist, int image);
		// imagebox
		extern int imagebox_create(int parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		extern int imagebox_set_image_from_hbitmap(int parent, HBITMAP bitmap);
		// images
		extern int image_createfromptr(const unsigned char *ptr, unsigned int width, unsigned int height);
		extern int image_createfromptrsubrect(const unsigned char *ptr, unsigned int width, unsigned int height, unsigned int destX, unsigned int destY, unsigned int destWidth, unsigned int destHeight);
		extern HBITMAP image_getbitmap(int image);
		extern void image_delete(int image);
		// painting
		extern void paint_resetDC();
		extern void paint_setDC(HDC hdc);
		extern void paint_image(int image, int x, int y, int xscale = 1, int yscale = 1);
		extern void paint_hbitmap(HBITMAP hbitmap, int x, int y);
		extern void paint_DC(HDC hdc, int x, int y, int width, int height);
		extern void paint_rectangle(int x, int y, int w, int h, unsigned int color);
		// scrollbars
		extern int scrollbar_create(int parent, int x, int y, int width, int height, bool horz);
		extern void scrollbar_setminmax(int parent, int minval, int maxval);
		extern int scrollbar_get_position(int parent);
		// listview
		extern int listview_create(int parent, int x, int y, int width, int height, bool icon);
		extern void listview_seticonspacing(int listview, int width, int height);
		extern void listview_setimagelist(int listview, int imagelist);
		extern int listview_additem(int listview, char *text, int image = NULL);
		extern int listview_get_selected(int listview);
	} // namespace gui
} // namespace ww
#endif
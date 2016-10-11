/*! Time-stamp: <@(#)main.cpp   24.11.2005 - 24.11.2005 20:57:46   Dmitry Chuchva>
 *********************************************************************
 *  @file   : main.cpp
 *
 *  Project : Plain Text Editor
 *
 *  Package : no
 *
 *  Company : Mitro Software Inc.
 *
 *  Author  : Dmitry Chuchva aka Mitro                Date: 24.11.2005
 *
 *  Purpose : Implementation of methods
 *
 *********************************************************************
 * Version History:
 *
 * V 0.10  24.11.2005  BN : First Revision
 *
 *********************************************************************
 */

                                        /*! ���������� ������ WinAPI      */
#define WIN32_LEAN_AND_MEAN
                                        /*! ����������� ������������ �����
										������� ������ */
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <vector>
#include <string>
                                        /*! ����������� ��������� ��������
										���� */
#define WM_MOUSEWHEEL 0x020A
                                        /*! ����������� ������������� ����
										� �������� */
#include "mainrc.h"
                                        /*! ����������� ������� �� �������
										��� ��� ������ */
#define MAX_STR 300
                                        /*! ���-�� ��������, ���������� ��
										���� ��������� */
#define TAB_SIZE 8
                                        /*! �������� ���������            */
#define PROGRAMNAME "PICO v.0.1a"
                                        /*! ��� ����� �� ���������        */
#define DEFFILENAME "UNNAMED.txt"
                                        /*! �����, �����, ��������� ����  */
#define CLASSNAME "PICOCLASS"
#define WINDOWSTYLE (WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL)
#define WINDOWEXSTYLE 0
#define WINDOWPOSX CW_USEDEFAULT
#define WINDOWPOSY CW_USEDEFAULT
#define WINDOWWIDTH CW_USEDEFAULT
#define WINDOWHEIGHT CW_USEDEFAULT
                                        /*! ������ �������                */
#define CARETWIDTH 2
                                        /*! ������ � ������ ��������      */
#define PAGEWIDTH 740
#define PAGEHEIGHT 248
                                        /*! ������ ��������� �������� �� �
										����� */
#define GETPAGE(line,linesperpage) ((line) / (linesperpage))
                                        /*! ������ "������ �����"         */
#define ABS(x) (((x) >= 0) ? (x) : (-x))

                                        /*! ��� ������� ������� � ������� 
										����������� ���������� */
using namespace std;

                              /*! ������ ����, ������, ����. �������������*/
HWND hWnd = NULL;
HINSTANCE hInst = NULL;
HACCEL hAccTable = NULL;
                              /*! ������ �������, ��������� ������        */
HFONT hFont = NULL, hFontOld = NULL;
LOGFONT lf;
                              /*! ��������� �� ������� ������ ������      */
void (*error)(char*) = NULL;
                              /*! ������� ��� ���� � ��� �����            */
char windowname[MAX_STR];
char filename[MAX_STR];
                              /*! ������� ������ � ������ ����            */
int wwidth,wheight;
                              /*! ������� ������ � ������ ��������        */
int page_width, page_height;
                              /*! ���� ����������� ������                 */
int paged = 0;
                              /*! ������� ��������; ����� �������         */
int page = 0, pages = 0; 
                              /*! ������� "�������" ������ - ������, �����
							  �� � ������� ��������� �����; ����� �� �����
							  ��� */
int page_line = 0, page_lines = 0;
                              /*! ����� ���� ��������� �� �����������     */
int win_x = 0;
                              /*! ����� � ���� ������                     */
int scr_lines = 0;
                              /*! ������ ���� � �������� �����            */
char path[MAX_STR];
                              /*! ������ ����� �������������� ������      */
vector<string> strings;
vector<int> wrapped;
                              /*! ������� ������� ��������� ���������     */
TEXTMETRIC tm;
                              /*! ������� ������������                    */
int valign = IDM_VALIGNTOP,halign = IDM_HALIGNLEFT;
                              /*! ������� ������� �������                 */
int carx = 0,cary = 0;
                              /*! ������� ����� (������,������)           */
int carline = 0, carchar = 0;
                              /*! ���� ���� � ������                      */
COLORREF background_color = RGB(255,255,255), text_color = RGB(0,0,0);
                              /*! ���� ����������� �����                  */
int modified = 0;
                              /*! ���-�� ���������� �����                 */
unsigned int count_saves = 0;
bool passed_save_dialog = false;

                                        /*! ��������� �������             */
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd);
BOOL APIENTRY DlgProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd,UINT messg,WPARAM wParam,LPARAM lParam);
int LoadFile(char *filename);
int SaveFile(char *filename);
int ProcessCmdLine(LPSTR line,char *filename);
void errore(char *mesg);
void errorwe(char *mesg);
void cleanup();
void SetPageWidth(int w);
void SetPageHeight(int h);
int GetLineCharsLength(const char*,int);
int FindLargestString(vector<string> &s);
int CharLength(int carl,int carc);
int GetPageWidth();
void DoBeforeCheckBound(HWND hWnd);
void DoAfterCheckBound(HWND hWnd);
void UpdateVScrollInfo(HWND hWnd);
void UpdateHScrollInfo(HWND hWnd);
void DrawPages(HWND hWnd,HDC,int s_page,int l_page,int page_L);
void SetWinX(int winx);
void SetLine(int lin);
void OutLine(HDC hdc,int line,int x,int y);
int CalcCarX(int carline, int carchar);
int CalcCarY(int carline, int carchar);
void CheckStrings(bool wrap);
bool StringWrapped(int carline);
void UnCheckWrap(int carline);
void ShiftWrap(int from_val, int delta);
string GetUnwrapped(int carline);
void CheckWrap(int carline);

/*!
 * @function : cleanup
 * @brief    : ���������� ������������ ������� �������� � �� ������� 
 *
 * @param none
 *
 * @return void : 
 */

void cleanup()
{
	strings.clear();
	return;
}

/*!
 * @function : errore
 * @brief    : ������� ������, ��������� ������ �� ����� � ��������� ������� � ��������
 *			   ������������ �� �������� ����
 * @param char *mesg : ������ � ������� ��� ������
 *
 * @return void : 
 */

void errore(char *mesg)
{
	MessageBox(HWND_DESKTOP,mesg,"Error",MB_OK | MB_ICONSTOP);
	cleanup();
	ExitProcess(0);
}

/*!
 * @function : errorwe
 * @brief    : ������� ������, ��������� ������ �� ����� � ��������� ������� � ��������
 *			   ������������ ����� �������� ����
 * @param char *mesg : ������ � ������� ��� ������
 *
 * @return void : 
 */

void errorwe(char *mesg)
{
	MessageBox(hWnd,mesg,"Error",MB_OK | MB_ICONSTOP);
	PostQuitMessage(0);
}


/*!
 * @function : WndProc
 * @brief    : ������� ��������� ���������
 *
 * @param HWND hWnd     : ����� ����
 * @param UINT messg    : ��������� ��� ����� ����
 * @param WPARAM wParam : ������ �������� ���������
 * @param LPARAM lParam : ������ �������� ���������
 *
 * @return LRESULT CALLBACK : ��������� ��������� (�� - 0, ��� - �����) 
 */

LRESULT CALLBACK WndProc(HWND hWnd,UINT messg,WPARAM wParam,LPARAM lParam)
{
                              /*! ��������� ��������� ��� WM_PAINT        */
	PAINTSTRUCT ps;
                              /*! ��������� ��� �������� ���������� ������
							  � ����*/
	RECT rc;
                              /*! �������� ���������� ����                */
	HDC hdc = NULL;
                              /*! ��� ��������� WM_COMMAND                */
	WORD wID = LOWORD(wParam);
	HWND hwndCtl = (HWND)lParam;
		
	switch (messg)
	{
	case WM_CREATE:
                                        /*! ��������� ������������ �� ����
										����� ����� � �������� ��� �������*/ 
		hdc = GetDC(hWnd);
		hFontOld = (HFONT)GetCurrentObject(hdc,OBJ_FONT);
		GetTextMetrics(hdc,&tm);
                                        /*! ������������� "����������" ���
										�� ������ ������ */
		SetBkMode(hdc,TRANSPARENT);
		ReleaseDC(hWnd,hdc);		
		return 0;
	case WM_CLOSE:
                                        /*! ���� ���� ��� �������, �������
										��� ��������� ��������� ����� ����
										��� */
		if (modified)
		{
			switch (MessageBox(hWnd,"File not saved. Do you wish to save before exit?","Confirm exit",MB_ICONQUESTION | MB_YESNOCANCEL))
			{
			case IDYES:
				SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SAVE,0),0);
				break;
			case IDNO:
				break;
			case IDCANCEL:
				return 0;
			}
		}
                                        /*! ���������� ����               */
		SendMessage(hWnd,WM_DESTROY,0,0);
		return 0;
	case WM_DESTROY:
                                        /*! �������� WM_QUIT (�����)      */
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
                                        /*! �������� ���������� ������� � 
										�������� �������� :) */
		GetClientRect(hWnd,&rc);
		hdc = BeginPaint(hWnd,&ps);
                                        /*! ���� ���� ��� ��������        */
		if (!strings.empty())
		{
                                        /*! ���������� ������ �������, ���
										������� � ���� ������ */
			int start_page = GETPAGE(page_line,page_lines);
                                        /*! ���������� ��� ��������� �����
										��� */
			int last_page = start_page + wheight / page_lines;
                                        /*! ���������� �������� � ��������
										������ ������ ��� ��������� */
			int y_offset = start_page * page_height + (page_line - start_page * page_lines) * tm.tmHeight;
                                        /*! ���� ������������� ������ �� �
										���� ��������� */
			bool exit_need = false;
                                        /*! ������������ ����� ������ ����
										�� ��� ������������� ������������ */
			int addh = 0, addv = 0;

                                        /*! ��� ������ ���������� ��������
										�� �������, �������� ��� � �������
										�� �������� �������� */
			if (last_page >= pages)
				last_page = pages - 1;
                                        /*! � ���������� ������ ������ ���
										������� �������� */
			if (paged)
				DrawPages(hWnd,hdc,start_page,last_page,page_line);
                                        /*! ����������� ����������� ������
										��������� ������������ */
			switch (halign)  
			{ 
			case IDM_HALIGNLEFT:
				break;
			case IDM_HALIGNRIGHT:
				addh = page_width;
				break;
            case IDM_HALIGNCENTER:
				addh = page_width / 2;
				break;
			}
                                        /*! ���� ���������� �� ���������  */
			for (int page_i = start_page; page_i <= last_page; page_i++)
			{
                                        /*! ����������� ��������� � ������
										�� ������ � ������ ���������������
										�������� */
				int start_line = 0;
				int last_line = page_lines - 1;

                                        /*! ���� ��� ������ ��������������
										� �������� */
				if (page_i == start_page)
                                        /*! ������������ ����� �������� ��
										� ��������� ������, ������ �������
										����� ���� */
					start_line = page_line - start_page * page_lines;
                                        /*! ���� ���� �����, �������      */
				if (exit_need)
					break;
				                        /*! ��� ������ ���������� ��������
										�� �������, �������� ��� � �������
										�� �������� �������� */
				if (last_line + page_i * page_lines >= (int)strings.size())
				{
					last_line = strings.size() - page_i * page_lines - 1;
                                        /*! ������ ���������, ���� �����  */
					exit_need = true;
				}
                                        /*! � ���������� ������ ���������
										��� ��� � ������������ �����������
										������������ */
				if (paged)
				{
					switch (valign)  
					{ 
					case IDM_VALIGNTOP:
						break;
					case IDM_VALIGNBOTTOM:
						addv = page_height - (last_line + 1) * tm.tmHeight;
						break;
					case IDM_VALIGNCENTER:
						addv = (page_height - (last_line + 1) * tm.tmHeight) / 2;
						break;
					}
				}
                                        /*! ���� ��������� �� �������     */
				for (int line_i = start_line; line_i <= last_line; line_i++)
					OutLine(hdc,
							page_i * page_lines + line_i,
							addh - win_x,
							addv + (page_i * page_height) + line_i * tm.tmHeight - y_offset
							);
			}
		}
                                        /*! ����� ���������               */
		EndPaint(hWnd,&ps);
		return 0;
	case WM_SIZE:
		{
			HDC hdc = GetDC(hWnd);
                                        /*! ��������� ����� �������� �����
										�� ������ */
			GetClientRect(hWnd,&rc);
			wheight = rc.bottom - rc.top;
			wwidth = rc.right - rc.left - 2;
                                        /*! ������������� ���-�� ������� �
										������� ������ */
			scr_lines = wheight / tm.tmHeight;
			ReleaseDC(hWnd,hdc);
                                        /*! ���� �� ���������� �����      */
			if (!paged)
			{
                                        /*! ���������� ����� �������� ����
										������ �������� */
				SetPageHeight(wheight);
				SetPageWidth(GetPageWidth());
			}
                                        /*! �������� ����������           */
			UpdateVScrollInfo(hWnd);
			UpdateHScrollInfo(hWnd);
                                        /*! �������� �������              */
			PostMessage(hWnd,WM_USER,0,0);
		}		
		return 0;
	case WM_COMMAND:
		switch (wID)
		{
		case IDM_NEW:
			{
                                        /*! ���� ���� ��� �������, �������
										��� ��������� ��������� ����� ����
										����� ������ ����� */
				if (modified)
				{
					switch (MessageBox(hWnd,"File not saved. Do you wish to save?","Confirm",MB_ICONQUESTION | MB_YESNOCANCEL))
					{
					case IDYES:
						SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SAVE,0),0);
						break;
					case IDNO:
						break;
					case IDCANCEL:
						return 0;
					}
				}

				char temp[MAX_STR];

                                        /*! ������������� ��� ���� � ��� �
										���� �� ��������� */
				sprintf(temp,"%s - %s",PROGRAMNAME,DEFFILENAME);
				sprintf(filename,"%s",DEFFILENAME);
				SetWindowText(hWnd,temp);
                                        /*! �������������� ������ �����   */
				strings.clear();
				wrapped.clear();
                carline = carchar = 0;
                                        /*! ����� ����� �����������       */
				modified = 0;
                                        /*! ���� �� ���� �� ���������     */
				count_saves = 0;
                                        /*! ������������                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! �������� �������              */
				PostMessage(hWnd,WM_USER,0,0);
			}
			break;
		case IDM_NEWINSTANCE:
                                        /*! �������� ��� ���� ��������    */
			WinExec(path,SW_SHOW);
			break;
		case IDM_OPEN:
			{
										/*! ���� ���� ��� �������, �������
										��� ��������� ��������� ����� ����
										����� ����� */
                if (modified)
				{
					switch (MessageBox(hWnd,"File not saved. Do you wish to save?","Confirm",MB_ICONQUESTION | MB_YESNOCANCEL))
					{
					case IDYES:
						SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SAVE,0),0);
						if (passed_save_dialog)
						{
							passed_save_dialog = false;
							return 0;
						}
						break;
					case IDNO:
						break;
					case IDCANCEL:
						return 0;
					}
				}

				OPENFILENAME ofn;

                                        /*! �������������� ��������� ��� �
										������ ������ ����� ��� �������� */
				memset(&ofn,0,sizeof(ofn));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = "Text files\0*.txt\0All files\0*.*\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = filename;
				ofn.nMaxFile = MAX_STR;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

                                        /*! ���� ������������ ���-�� �����
										� */
				if (GetOpenFileName(&ofn))
				{
                                        /*! ����������� ���������         */
					if (LoadFile(ofn.lpstrFile))
					{
                                        /*! �������� ��� �����            */
						sprintf(filename,"%s",ofn.lpstrFile);
                                        /*! �������� ���������� �����     */
						carline = carchar = 0;
                                        /*! �������� ��������� ��������   */
						SetPageHeight(page_height);
                                        /*! ������������                  */
						InvalidateRect(hWnd,NULL,TRUE);
					}
					else
                                        /*! ������                        */
						MessageBox(hWnd,"Error while loading file","Error",MB_OK | MB_ICONSTOP);
				}
			}
                                        /*! �������� �������              */
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_SAVE:
                                        /*! ���� ���� ���������� �����    */
			if ((strcmp(filename,DEFFILENAME) == 0) && (!count_saves))
			{
                                        /*! ����������� Save as...        */
				SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SAVEAS,0),0);
				break;
			}
			else
                                        /*! ����� ������ ���������        */
				if (!SaveFile(filename))
					MessageBox(hWnd,"Error while saving file","Error",MB_OK | MB_ICONSTOP);
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_SAVEAS:
			{
				OPENFILENAME ofn;
				char defext[] = "txt";

                                        /*! ������� ������ ����������     */
				memset(&ofn,0,sizeof(ofn));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = "Text files\0*.txt\0All files\0*.*\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = filename;
				ofn.lpstrDefExt = defext;
				ofn.nMaxFile = MAX_STR;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

                                        /*! �������� ������               */
				if (GetSaveFileName(&ofn))
				{
					passed_save_dialog = false;
					                    /*! � ������ ������ ���������     */
					if (SaveFile(ofn.lpstrFile))
						sprintf(filename,"%s",ofn.lpstrFile);
					else
						MessageBox(hWnd,"Error while saving file","Error",MB_OK | MB_ICONSTOP);
				}
				else
					passed_save_dialog = true;
			}
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_EXIT:
                                        /*! �������� WM_CLOSE             */
			SendMessage(hWnd,WM_CLOSE,0,0);
			break;
		case IDM_COLORS:
			{
				CHOOSECOLOR chclr;
				static COLORREF initbclr[16];

                                        /*! ������� ������ ������ �����   */
				for (int i = 0; i < 16; i++)
					initbclr[i] = RGB(255,255,255);
				memset(&chclr,0,sizeof(CHOOSECOLOR));
				initbclr[0] = background_color;
				chclr.lStructSize = sizeof(CHOOSECOLOR);
				chclr.hwndOwner = hWnd;
				chclr.lpCustColors = initbclr;
                                        /*! �������� ���                  */
				if (ChooseColor(&chclr))
				{
                                        /*! ������������� ������� ����    */
					background_color = chclr.rgbResult;
                                        /*! ������������                  */
					InvalidateRect(hWnd,NULL,true);
				}
			}
			break;
		case IDM_FONT:
			{
				HideCaret(hWnd);

				CHOOSEFONT cf;
				HDC hdc = GetDC(hWnd);

                                        /*! ������� ������ ������ ������  */
				if (hFont)
				{
					GetObject(hFont,sizeof(LOGFONT),&lf);
				}
				else
                    GetObject(hFontOld,sizeof(LOGFONT),&lf);
				cf.lStructSize = sizeof(CHOOSEFONT);
				cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_SCREENFONTS |
					CF_EFFECTS;
				cf.hwndOwner = hWnd;
				cf.lpLogFont = &lf;
				cf.rgbColors = text_color;
                                        /*! �������� ���; ���� �������... */
				if (ChooseFont(&cf))
				{
                                        /*! ...������� ���������� �����   */
					if (hFont)
					{
		                SelectObject(hdc,hFontOld);
						DeleteObject(hFont);
						hFont = NULL;
					}
                                        /*! ������� �����                 */
					hFont = CreateFontIndirect(&lf);
                                        /*! ������������� ���             */
					hFontOld = (HFONT)SelectObject(hdc,hFont);
					text_color = cf.rgbColors;
                                        /*! ������������� ���� ������     */
					SetTextColor(hdc,text_color);
				}
				ReleaseDC(hWnd,hdc);
                                        /*! ��������� ������� ������      */
				hdc = GetDC(hWnd);
				memset(&tm,0,sizeof(TEXTMETRIC));
				GetTextMetrics(hdc,&tm);
				ReleaseDC(hWnd,hdc);
                                        /*! ����������� ������� � ����� ��
										����� */
				DestroyCaret();
				CreateCaret(hWnd,(HBITMAP)NULL,CARETWIDTH,tm.tmHeight);
				ShowCaret(hWnd);
                                        /*! ��������� ��������� ��������  */
				SetPageHeight(page_height);
				if (paged)
                    CheckStrings(true);
                                        /*! ������������                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! �������� �������              */
				PostMessage(hWnd,WM_USER,0,0);
			}
			break;
		case IDM_PAGED:
                                        /*! �������� ����� �� ������������
										��� */
			paged = 1 - paged;
			HideCaret(hWnd);
			if (paged)
			{
                                        /*! ��������� ��� ����            */
				CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),IDM_PAGED,MF_BYCOMMAND | MF_CHECKED);
				EnableMenuItem(GetSubMenu(GetMenu(hWnd),1),4,MF_BYPOSITION | MF_ENABLED);
                                        /*! ������������� ����� ��������� 
										�������� */
				SetPageWidth(PAGEWIDTH);
				SetPageHeight(PAGEHEIGHT);
                                        /*! ��������� ������              */
				CheckStrings(true);
                                        /*! ������������                  */
				InvalidateRect(hWnd,NULL,true);
			}
			else
			{
                                        /*! ��������� ����                */
				CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),IDM_PAGED,MF_BYCOMMAND | MF_UNCHECKED);
				EnableMenuItem(GetSubMenu(GetMenu(hWnd),1),4,MF_BYPOSITION | MF_GRAYED);
				HDC hdc = GetDC(hWnd);
				RECT rc;
                                        /*! ������������� ��������� ������
										�� ��� �������� ������ */
				GetClientRect(hWnd,&rc);
				SetPageWidth(GetPageWidth());
				SetPageHeight(rc.bottom - rc.top);
				ReleaseDC(hWnd,hdc);
				CheckStrings(false);
                                        /*! ������������                  */
				InvalidateRect(hWnd,NULL,true);
			}
			carchar = carline = 0;
			ShowCaret(hWnd);
                                        /*! �������� �������              */
			PostMessage(hWnd,WM_USER,0,0);
			DoAfterCheckBound(hWnd);
			break;
		case IDM_VALIGNTOP:
                                        /*! ��������� ����                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNTOP,MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNBOTTOM,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNCENTER,MF_BYCOMMAND | MF_UNCHECKED);
                                        /*! ������������� ������������    */
			valign = wID;
                                        /*! ������������                  */
			InvalidateRect(hWnd,NULL,true);
                                        /*! �������� �������              */
			PostMessage(hWnd,WM_USER,0,0);
            break;
		case IDM_VALIGNBOTTOM:
			                            /*! ��������� ����                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNTOP,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNBOTTOM,MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNCENTER,MF_BYCOMMAND | MF_UNCHECKED);
			                            /*! ������������� ������������    */
			valign = wID;
                                        /*! ������������                  */
			InvalidateRect(hWnd,NULL,true);
                                        /*! �������� �������              */
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_VALIGNCENTER:
			                            /*! ��������� ����                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNTOP,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNBOTTOM,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNCENTER,MF_BYCOMMAND | MF_CHECKED);
			                            /*! ������������� ������������    */
			valign = wID;
                                        /*! ������������                  */
			InvalidateRect(hWnd,NULL,true);
                                        /*! �������� �������              */
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_HALIGNLEFT:
			                            /*! ��������� ����                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNLEFT,MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNCENTER,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNRIGHT,MF_BYCOMMAND | MF_UNCHECKED);
			                            /*! ������������� ������������    */
			halign = wID;
			{
				HideCaret(hWnd);
				HDC hdc = GetDC(hWnd);
                                        /*! ��������� ������������ ��� ���
										������ ���������� */
				SetTextAlign(hdc,TA_LEFT | TA_TOP | TA_NOUPDATECP);
				ReleaseDC(hWnd,hdc);
                ShowCaret(hWnd);
				                        /*! ������������                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! �������� �������              */
				PostMessage(hWnd,WM_USER,0,0);
			}
			break;
		case IDM_HALIGNRIGHT:
			                            /*! ��������� ����                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNLEFT,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNCENTER,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNRIGHT,MF_BYCOMMAND | MF_CHECKED);
			                            /*! ������������� ������������    */
			halign = wID;
			{
				HideCaret(hWnd);
				HDC hdc = GetDC(hWnd);
                                        /*! ��������� ������������ ��� ���
										������ ���������� */
				SetTextAlign(hdc,TA_RIGHT | TA_TOP | TA_NOUPDATECP);
				ReleaseDC(hWnd,hdc);
				ShowCaret(hWnd);
				                        /*! ������������                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! �������� �������              */
				PostMessage(hWnd,WM_USER,0,0);
			}
			break;
		case IDM_HALIGNCENTER:
			                            /*! ��������� ����                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNLEFT,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNCENTER,MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNRIGHT,MF_BYCOMMAND | MF_UNCHECKED);
			                            /*! ������������� ������������    */
			halign = wID;
			{
				HideCaret(hWnd);
				HDC hdc = GetDC(hWnd);
                                        /*! ��������� ������������ ��� ���
										������ ���������� */
				SetTextAlign(hdc,TA_CENTER | TA_TOP | TA_NOUPDATECP);
				ReleaseDC(hWnd,hdc);
				ShowCaret(hWnd);
				                        /*! ������������                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! �������� �������              */
				PostMessage(hWnd,WM_USER,0,0);				
			}
			break;
		case IDM_HELP:
                                        /*! ������� ������� :)            */
			MessageBox(hWnd,"Help yourself :)","Info",MB_OK | MB_ICONINFORMATION);
			break;
		case IDM_ABOUT:
                                        /*! ������� ������ "� ���������"  */
			DialogBoxParam(hInst,"aboutdialog",hWnd,DlgProc,(long)hWnd);
			break;
		}
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
                                        /*! ��������� ������� Home,C+Home */
		case VK_HOME:
                                        /*! ���� ����� ���, �����         */
			if (strings.empty())
				break;
                                        /*! ���� ������ Ctrl + Home       */
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
                                        /*! ������� � ������ ���������    */
				page = 0; page_line = 0;
				carline = 0;
				carchar = 0;
			}
			else
			{
                                        /*! ����� � ������ ������         */
				carchar = 0;
			}
                                        /*! ������������                  */
			InvalidateRect(hWnd,NULL,true);
			break;
		case VK_END:
                                        /*! ���� ����� ���, �����         */
			if (strings.empty())
				break;
                                        /*! ���� ������ Ctrl + End        */
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
                                        /*! ������� � ����� ���������     */
				carline = strings.size() - 1;
				carchar = strings[carline].length();
				page = pages - 1;
				page_line = strings.size() - scr_lines - 1;
			}
			else
			{
                                        /*! ����� � ����� ������          */
				carchar = strings[carline].length();
			}
                                        /*! ������������                  */
			InvalidateRect(hWnd,NULL,true);
			break;
		case VK_ESCAPE:
                                        /*! ���� ������ Esc, ������� WM_CL
										OSE */
			SendMessage(hWnd,WM_CLOSE,0,0);
			break;
		case VK_DOWN:
                                        /*! �������� ����� ����� �� ������
										���� */
			carline++;
			break;
		case VK_UP:
                                        /*! �������� ����� ����� �� ������
										����� */
			carline--;
			break;		
		case VK_NEXT:
                                        /*! ���� ������ PgDown, �������� �
										� ���� ����� ����*/
			carline += scr_lines;
                                        /*! ��������� ����� �� �������    */
			if (carline >= (int)strings.size() - scr_lines)
				carline = (int)strings.size() - 1;
                                        /*! �������� ����� ������ ������  */
			page_line += scr_lines;
                                        /*! ��������� ����� �� �������    */
			if (page_line >= (int)strings.size() - scr_lines)
				page_line = (int)strings.size() - scr_lines;
                                        /*! ���������� ������ ������      */
			SetLine(page_line);
                                        /*! ������������                  */
			InvalidateRect(hWnd,NULL,true);
		    break;
		case VK_PRIOR:
                                        /*! ���� ������ PgUp, �������� �� 
										���� ����� ����� */
			carline -= scr_lines;
                                        /*! ��������� ����� �� �������    */
			if (carline < 0)
				carline = 0;
		    break;
		case VK_RIGHT:
                                        /*! ���� ����� ���, �����         */
			if (strings.empty())
				break;
                                        /*! ���������� ���������� ������� 
										����� */
			carchar++;
                                        /*! ���� ����� �� ����� ������    */
			if (carchar > (int)strings[carline].length())
			{
				if (carline < (int)strings.size() - 1)
				{
                                        /*! ���� �� ��������� ������, ����
										������ ���������� ������ ����� */
					carchar = 0;
					carline++;
				}
				else
                                        /*! ����� �������� �� �����       */
					carchar = strings[carline].length();
			}
			break;
		case VK_LEFT:
                                        /*! ���� ����� ���, �����         */
			if (strings.empty())
				break;
                                        /*! ��������� ���������� ������ ��
										���*/
			carchar--;
                                        /*! ���� ����� �� ������ ������   */
			if (carchar < 0)
			{
				if (carline > 0)
				{
                                        /*! ���� �� ������ ������, �������
										�� ������ ����� */
					carline--;
                                        /*! ���������� ������ ����� � ����
										� ������ */
					carchar = strings[carline].length();
				}
				else
                                        /*! ����� �������� �� �����       */
					carchar = 0;
			}
		    break;
		case VK_TAB:
			{
                                        /*! ���� ������ TAB, �������� ��� 
										TAB_SIZE ��������� */
				for (int i = 0; i < TAB_SIZE; i++)
                                        /*! ������� ���� ������           */
					SendMessage(hWnd,WM_CHAR,32,0);
			}
			break;
                                        /*! ��������� Enter               */
		case VK_RETURN:
			{
                string app;

                                        /*! ���� ����� ���                */
				if (strings.empty())
                                        /*! ������� ���� ������           */
					strings.push_back("");
                                        /*! ���� ���������� ������� ����� 
										� ����� ������ */
				if (carchar == strings[carline].length())
                                        /*! ������ �� ����������          */
					app = "";
				else
				{
                                        /*! ����� ��������� ��� �� �������
										��� ������ ����� �� ����� ������ */
					app.append(strings[carline],carchar,strings[carline].length() - carchar);
                                        /*! ������� ������������ �� ������*/
					strings[carline].erase(carchar,strings[carline].length() - carchar);
				}
                                        /*! ������� �� ����� ������       */
				carline++;
                                        /*! ���� �� ���������             */
				if (strings.size() != carline)
				{
                                        /*! �������� ��, ��� ����� �������
										�� */
					if (paged)
                        if (!StringWrapped(carline - 1))
						{
							strings.insert(strings.begin() + carline,app);
							for (int i = 0; i < (int)wrapped.size(); i++)
							{
								if (wrapped[i] >= carline)
									wrapped[i]++;
							}
						}
						else
						{
							UnCheckWrap(carline - 1);
							strings[carline].insert(0,app);
							CheckStrings(true);
						}
				}
				else
                                        /*! ����� �������� � �����        */
					strings.push_back(app);
                                        /*! ���������� ����� ������� � ���
										��� ����� ������*/
				carchar = 0;
                                        /*! ���������� ���� �����������   */
				modified = 1;
				InvalidateRect(hWnd,NULL,true);
			}
			break;
                                        /*! ��������� Backspace           */
		case VK_BACK:
                                        /*! ���� ������ ����              */
			if (!strings.empty())
			{
                                        /*! ���� � ������ ������          */
				if (carchar == 0)
				{
                                        /*! ���� �� ������ ������         */
					if (carline != 0)
					{
                                        /*! ���� �� ���������� �����      */
						if (!paged)
						{
                                        /*! ���������� ������� ����� - ���
										�� ������ ��� ������� */
							carchar = strings[carline - 1].length();
                                        /*! ���� ����� ������� ������ �� 0*/
							if (strings[carline].length() != 0)
								        /*! ������ �������� ������� ������
										� ������ ��� �������*/
								strings[carline - 1].append(strings[carline],0,strings[carline].length());
                                        /*! ������� ������� ������        */
							strings.erase(strings.begin() + carline,strings.begin() + carline + 1);
                                        /*! ������� �� ������, � ������� �
										��������*/
							carline--;
						}
						else
						{
                                        /*! ���� ��� ���������� ������ ���
										���� �� ������� �������� */
						/*	if ((CharLength(carline - 1,strings[carline - 1].length()) + 
                                CharLength(carline,strings[carline].length())) > 
								page_width)
							{
                                        /*! ������� � ����� ������ ������ 
										���� ������ */
						/*		strings[carline - 1].erase(strings[carline - 1].length() - 1,1);
							} */
                                        /*! ����� �� �� �����, ��� � � ���
										���� ������ */
							carchar = strings[carline - 1].length();
							if (strings[carline].length() != 0)
								strings[carline - 1].append(strings[carline],0,strings[carline].length());
							strings.erase(strings.begin() + carline);
							carline--;

							int ind = 0;
							while (ind < (int)wrapped.size())
							{
								if (wrapped[ind] == carline)
									wrapped.erase(wrapped.begin() + ind);
								if (wrapped[ind] > carline)
								{
									if (wrapped[ind] == carline + 1)
										wrapped[ind] = carline;
									else
                                        wrapped[ind] -= 1;
								}
								ind++;
							}
							CheckStrings(true);

						} // if (!paged)

                                        /*! ���������� ���� �����������   */
						modified = 1;
					} // if (carline != 0)
				}
				else
				{
                                        /*! ���� � �������� ������, �� ���
										���� ���� ������ ����� �����������
										����� */
					carchar--;
					strings[carline].erase(carchar,1);
					if (paged)
						if (StringWrapped(carline))
//							if (CharLength(carline,strings[carline].length()) + CharLength(carline + 1,
//								strings[carline + 1].length()) <= page_width)
                                    CheckStrings(true);
                                        /*! ��������� ���� �����������    */
					modified = 1;
				} // if (carchar == 0)
                                        /*! ������������                  */
				InvalidateRect(hWnd,NULL,true);
			} // if (!strings.empty())
			break;
                                        /*! ��������� ������� Delete      */
		case VK_DELETE:
                                        /*! ���� ����� ���, �����         */
			if (strings.empty())
				break;
                                        /*! ���� ������� ������ �� �����  */
			if (!strings[carline].empty())
			{
                                        /*! ���� ������� ����� � ����� ���
										��� */
				if (carchar == strings[carline].length())
				{
                                        /*! ���� �� ��������� ������      */
					if (carline != strings.size() - 1)
					{
                                        /*! ���� �� ���������� �����      */
						if (!paged)
						{
                                        /*! �������� � ����� ������� �����
										� ��������� �� ��� ������ */
							strings[carline] += strings[carline + 1];
                                        /*! ������� ����������� ������ � �
										� ����� */
							strings.erase(strings.begin() + carline + 1,strings.begin() + carline + 2);
						}
						else
						{
                                        /*! ���� ������ �������� ��� �����
										� �� ������� �������� */
							if ((CharLength(carline + 1,strings[carline + 1].length()) + 
                                CharLength(carline,strings[carline].length())) > 
								page_width)
							{
                                        /*! ������� � ������ ��������� ���
										��� ���� ������*/
								strings[carline + 1].erase(0,1);
							}
							strings[carline] += strings[carline + 1];
							strings.erase(strings.begin() + carline + 1);
							if (StringWrapped(carline + 1))
								CheckWrap(carline);
							else
								UnCheckWrap(carline);
							UnCheckWrap(carline + 1);
							ShiftWrap(carline + 1,-1);
							CheckStrings(true);

//							}
						} // if (!paged)
                                        /*! ���������� ���� �����������   */
						modified = 1;
					} // if (carline != strings.size() - 1)
				} // if (carchar == strings[carline].length())
				else
				{
                                        /*! ���� � �������� ������, ������
										� ������ �� ������� �������� �����*/
					strings[carline].erase(carchar,1);
					if (paged)
						if (StringWrapped(carline))
//							if (CharLength(carline,strings[carline].length()) + CharLength(carline + 1,
//								strings[carline + 1].length()) <= page_width)
                                    CheckStrings(true);
                                        /*! ���������� ���� �����������   */
					modified = 1;
				}
			} // if (!strings[carline].empty())
			else
                                        /*! ���� ������� ������ �����     */
				if (carline != strings.size() - 1)
				{
                                        /*! ������� ��                    */
					strings[carline] += strings[carline + 1];
                    strings.erase(strings.begin() + carline + 1);
					UnCheckWrap(carline + 1);
					ShiftWrap(carline + 1,-1);
                                        /*! ���������� ���� �����������   */
					modified = 1;
				}
                                        /*! ������������                  */
			InvalidateRect(hWnd,NULL,true);
        	break;
		}
                                        /*! �������� ������ ������� ������
										� �����*/
		DoBeforeCheckBound(hWnd);
                                        /*! ���������� �������            */
		PostMessage(hWnd,WM_USER,0,0);
                                        /*! �������� ������ ���������� ���
										���� */
		DoAfterCheckBound(hWnd);
		return 0;
                                        /*! ��������� ���������-�������� �
										������� */
	case WM_CHAR:
                                        /*! ���� ������ ���������         */
		if ((wParam >= 32) && (wParam <= 255))
		{
                                        /*! ���� "������� ����� � ����� ���
										���" */
			int car_in_end = false;
                                        /*! ���� ����� ���                */
			if (strings.empty())
                                        /*! ������� ���� ������           */
				strings.push_back("");
                                        /*! ���� ������� ����� � ����� ���
										��� */
			if (strings[carline].length() != carchar)
                                        /*! �������� ������               */
				strings[carline].insert(strings[carline].begin() + carchar,(char)wParam);
			else
			{
                                        /*! ���������� �����. ����        */
				car_in_end = true;
                                        /*! �������� ������ � ����� ������*/
				strings[carline] += (char)wParam;
			}
                                        /*! ���������� ���� �����������   */
			modified = 1;
                                        /*! ������� �� ��������� �������  */
			carchar++;
                                        /*! ���� ���������� �����         */
			if (paged)
                                        /*! ���� ������ ����� �� ������� �
										������� */
				if (CharLength(carline,strings[carline].length()) > page_width)
				{
                                        /*! ��������� ������              */
					CheckStrings(true);
                                        /*! ���� ������� ���� � �����     */
					if (car_in_end)
					{
                                        /*! ������� �� ����� ������       */
						carline++;
                                        /*! ���������� ���� �� ������ ����
										�� */
						carchar = 1;
					}
				}
                                        /*! ��������������� ������� ����� */
			DoBeforeCheckBound(hWnd);
                                        /*! �������� �������              */
			PostMessage(hWnd,WM_USER,0,0);
                                        /*! ��������������� �������       */
			DoAfterCheckBound(hWnd);
                                        /*! ������������                  */
			InvalidateRect(hWnd,NULL,true);
		}
		return 0;
                                        /*! ��������� ������������� ������*/
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEDOWN:
                                        /*! ���������� ����� ��������� ���
										��� ������ */
			SetLine(++page_line);
			break;
		case SB_LINEUP:
                                        /*! ��������� ����� ��������� ����
										�� ������ */
			SetLine(--page_line);
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
                                        /*! ���������� ����� ��������� ���
										��� ������ � ����������� � �������
										� ���������� */
			page_line = HIWORD(wParam);
			SetLine(page_line);
			break;
		case SB_PAGEUP:
                                        /*! ��������� ����� ��������� ����
										�� ������ �� ���-�� ����� � ���� �
										����� */
			page_line -= scr_lines;
			if (page_line < 0)
				page_line = 0;
			SetLine(page_line);
			break;
		case SB_PAGEDOWN:
                                        /*! ��������� ����� ��������� ����
										�� ������ �� ���-�� ����� � ���� �
										����� */
			page_line += scr_lines;
			if (page_line > (int)strings.size() - scr_lines)
				page_line = strings.size() - scr_lines;
			SetLine(page_line);
			break;
		}
                                        /*! ������������                  */
		InvalidateRect(hWnd,NULL,TRUE);
                                        /*! �������� ��� ����������       */
		UpdateVScrollInfo(hWnd);
                                        /*! �������� �������              */
		PostMessage(hWnd,WM_USER,0,0);
		return 0;
                                        /*! ��������� �������� ����       */
	case WM_MOUSEWHEEL:
                                        /*! ����� ����                    */
		if ((short)HIWORD(wParam) < 0)
		{
			if ((int)strings.size() > scr_lines)
				SetLine(++page_line);
			else
				SetLine(strings.size() - scr_lines);
		}
		else
                                        /*! ����� �����                   */
			SetLine(--page_line);
                                        /*! ������������                  */
		InvalidateRect(hWnd,NULL,true);
                                        /*! �������� ��� ����������       */
		UpdateVScrollInfo(hWnd);
                                        /*! �������� �������              */
		PostMessage(hWnd,WM_USER,0,0);
		return 0;
                                        /*! ��������� ��������������� ����
										������ */
	case WM_HSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
                                        /*! ���� ������ �������� ������ ��
										���� ������� ������, �� ����������
										�� �������������� */
			if (page_width < wwidth)
				break;
                                        /*! �������� ���������� ����������
										����� ������ ����� �� ��. ������ �
										����� �������� ������ */
			win_x -= tm.tmAveCharWidth;
			SetWinX(win_x);
			break;
		case SB_LINERIGHT:
			                            /*! ���� ������ �������� ������ ��
										���� ������� ������, �� ����������
										�� �������������� */
			if (page_width < wwidth)
				break;
			                            /*! �������� ���������� ����������
										����� ������ ������ �� ��. ������ 
										������� �������� ������ */
			win_x += tm.tmAveCharWidth;
			SetWinX(win_x);
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
                                        /*! ���������� ���������� �������
										�������� ������ � ������������ � 
										�������� ���������� */
			win_x = HIWORD(wParam);
			SetWinX(win_x);
			break;
		case SB_PAGELEFT:
                                        /*! �������� ���������� ����������
										���� ������ �� ������ ������� ����
										�� ����� */
			win_x -= wwidth;
			if (win_x < 0)
				win_x = 0;
			SetWinX(win_x);
			break;
		case SB_PAGERIGHT:
			                            /*! �������� ���������� ����������
										���� ������ �� ������ ������� ����
										�� ������ */
			win_x += wwidth;
			SetWinX(win_x);
			break;
		}
                                        /*! ������������                  */
		InvalidateRect(hWnd,NULL,TRUE);
                                        /*! �������� ��� ���. ����������  */
		UpdateHScrollInfo(hWnd);
                                        /*! �������� �������              */
		PostMessage(hWnd,WM_USER,0,0);
		return 0;
	case WM_SETFOCUS:
                                        /*! �������� � ����������� �������*/
		CreateCaret(hWnd,(HBITMAP)NULL,CARETWIDTH,tm.tmHeight);
		ShowCaret(hWnd);
                                        /*! ���������� �������            */
		PostMessage(hWnd,WM_USER,0,0);
		return 0;
	case WM_KILLFOCUS:
                                        /*! ������ � ���������� �������   */
		HideCaret(hWnd);
		DestroyCaret();
		return 0;
                                        /*! ��� �������� ����             */
	case WM_ERASEBKGND:
		{
			RECT rc;
			HPEN hPen, hPenOld;
			HBRUSH hBrush, hBrushOld;

			HideCaret(hWnd);
			GetClientRect(hWnd,&rc);
                                        /*! ������� ����� �������� �����  */
			hBrush = CreateSolidBrush(background_color);
                                        /*! ������� ����� �����, �������� 
										������ */
			hBrushOld = (HBRUSH)SelectObject((HDC)wParam,hBrush);
                                        /*! ������� ���� �������� �����   */
			hPen = CreatePen(PS_SOLID,0,background_color);
                                        /*! ������� ����� ����, �������� �
										����� */
			hPenOld = (HPEN)SelectObject((HDC)wParam,hPen);
                                        /*! ���������� ����������� �������
										������ �� ��� ������� ������ */
			Rectangle((HDC)wParam,0,0,rc.right - rc.left,rc.bottom - rc.top);
                                        /*! ������������ ����� � ���� �� �
										��������, ������� ��������� */
			SelectObject((HDC)wParam,hPenOld);
			DeleteObject(hPen);
			SelectObject((HDC)wParam,hBrushOld);
			DeleteObject(hBrush);
			ShowCaret(hWnd);
		}
		return 1;
	case WM_USER:
		{
                                        /*! ����������� ���������� �������*/
			carx = CalcCarX(carline,carchar);
			cary = CalcCarY(carline,carchar);
                                        /*! ���������� �� �������         */
			SetCaretPos(carx,cary);
		}
		return 0;
	default:
                                        /*! ��������� ��������� �� �������
										�� */
		return DefWindowProc(hWnd,messg,wParam,lParam);
	}
}


/*!
 * @function : WinMain
 * @brief    : ����� ����� � ���������
 *
 * @param HINSTANCE hInstance     : ����� ������������ ������ �� ��� �����
 * @param HINSTANCE hPrevInstance : ��� ������������� � Win3.1 
 * @param LPSTR lpCmdLine         : ������ �������
 * @param int nShowCmd            : ������� �������
 *
 * @return int WINAPI : ��� ������
 */

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
                              /*! ��������� ���������                     */
	MSG msg;
                              /*! ��������� ������ ����                   */
	WNDCLASSEX wc;

                                        /*! �������� ������ ���� � ���    */
	GetModuleFileName(NULL,path,MAX_STR);
                                        /*! ��������� ����� ����. ������  */
	hInst = hInstance;
                                        /*! ���������� ������� ��������� �
										����� */
	error = errore;
                                        /*! ���������� ��������� ������ ��
										�� */
	wc.cbSize = 48;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.hbrBackground = NULL;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
                                        /*! ��������� ������ �� ��������  */
	wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MYICON));
	wc.hIconSm = wc.hIcon;
	wc.hInstance = hInstance;
                                        /*! ���� �� ��������              */
	wc.lpszMenuName = "picomenu";
	wc.lpszClassName = CLASSNAME;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;

                                        /*! ������������ ����� ����       */
	if (!RegisterClassEx(&wc))
		error("Invalid class");

                                        /*! �������������� ������ � ������
										���� ���� �� ����� ��������� � ���
										�� ����� �� ��������� */
	sprintf(windowname,"%s - %s",PROGRAMNAME,DEFFILENAME);

                                        /*! ������� ����                  */
	hWnd = CreateWindowEx(WINDOWEXSTYLE,CLASSNAME,windowname,
		WINDOWSTYLE,WINDOWPOSX,WINDOWPOSY,WINDOWWIDTH,WINDOWHEIGHT,
		NULL,NULL,hInstance,NULL);

                                        /*! ��������� �� ������           */
	if (!hWnd)
		error("Invalid window");

                                        /*! ���������� ������� ��������� �
										����� � ������ ���������� ���� */
	error = errorwe;

                                        /*! �������� ������ �����         */
	strings.clear();
                                        /*! ��������� "�������" �������   */
	hAccTable = LoadAccelerators(hInst,"picoaccels");
                                        /*! ��������� ��������� ������    */
	if (ProcessCmdLine(lpCmdLine,filename))
	{
		char fmesg[MAX_STR];

                                        /*! ����������� ��������� ����    */
		if (!LoadFile(filename))
		{
                                        /*! � ������ ������ - ������� ��  */
			sprintf(fmesg,"File %s not found!",filename);
			MessageBox(hWnd,fmesg,"Error",MB_OK | MB_ICONSTOP);
			sprintf(filename,"UNNAMED.txt");
		}
	}
	else
		sprintf(filename,"UNNAMED.txt");

                                        /*! ���� ��������� ���������      */
	while (GetMessage(&msg,NULL,0,0))
	{
		if (!TranslateAccelerator(hWnd,hAccTable,&msg))
			TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	hWnd = NULL;

                                        /*! ���������� ������� �������    */
	cleanup();

                                        /*! ����������� �������           */
	return msg.wParam;
}


/*!
 * @function : DlgProc
 * @brief    : ������� ��������� ��������� �������
 *
 * @param HWND hwndDlg  : ����� ���� �������
 * @param UINT message  : ���������
 * @param WPARAM wParam : ������ �������� ���������
 * @param LPARAM lParam : ������ �������� ���������
 *
 * @return BOOL APIENTRY : ����������?
 */

BOOL APIENTRY DlgProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
                                        /*! ��������� ������� �� ���������
										��� ������ */
		case IDB_OK:
			if (HIWORD(wParam) == BN_CLICKED)
			{
                                        /*! ��������� ������              */
				EndDialog(hwndDlg,0);
				return TRUE;
			}
			return FALSE;
		default:
			return FALSE;
		}
		return FALSE;
	default:
		return FALSE; 
    } 
} 


/*!
 * @function : ProcessCmdLine
 * @brief    : ��������� ��������� ������
 *
 * @param LPSTR line     : ��������� ������ 
 * @param char *filename : ���������� ��� ������� �����
 *
 * @return int : 1 - filename �������� ��� ����������� ����� ���. ������ �����, 0 - ������ 
 *				����� ���. ������ �������� �� ����
 */

int ProcessCmdLine(LPSTR line,char *filename)
{
                              /*! ������� � ���. ������ ������ �������� ��
							  ������� */
	int start = 0;
                              /*! ���-�� ����������                       */
	int count = 0;
                              /*! ����� ��� ���������� �������� �������� �
							  �������� */
	char buff[MAX_STR];

                                        /*! �������� ������               */
	memset(buff,0,MAX_STR);
	memset(filename,0,MAX_STR);
                                        /*! ���� �� ������� ���. ������   */
	for (UINT i = 0; i < strlen(line) + 1; i++)
	{
                                        /*! ���� ��� �����������, ���� ���
										�� ������ */
		if ((line[i] == ' ') || (line[i] == '\0'))
		{
                                        /*! ���� ����� ����               */
			if (!buff[0])
			{
                                        /*! ����������� ��������� �� �����
										� ��������� �� 1 ������ */
				start++;
                                        /*! ���� ������                   */
				continue;
			}
                                        /*! ����� �� ����                 */
                                        /*! ���� ������ n >= 2 ��������, �
										��������� ��� � ����� ���������� �
										��������� ����� ��������� ������ */
			if (count)
			{
				char cline[MAX_STR];

				sprintf(cline,"%s %s",path,buff);
				WinExec(cline,SW_SHOW);
			}
			else
                                        /*! ����� �������� � ������ � ����
										������� ����� */
				strcpy(filename,buff);
                                        /*! ��������� ���-�� ��������� ���
										������� */
			count++;
                                        /*! �������� �����                */
			memset(buff,0,MAX_STR);
                                        /*! �������� �������� �� ������ ��
										������� �� 1 ����� */
			start = i + 1;
		}
		else
		{
                                        /*! �������� ������ � �����       */
			buff[i - start] = line[i];
			buff[i - start + 1] = '\0';
		}
	}
                                        /*! ���� ���-�� �������           */
	if (count)
                                        /*! ���������� ��� ��������� �����
										����� */
		return 1;
	else
                                        /*! ����� - �����������           */
		return 0;
}


/*!
 * @function : LoadFile
 * @brief    : ��������� ����� �� ����� � ������ �����
 *
 * @param char *filename : ��� �����
 *
 * @return int : ������� (1) / ��������� (0)
 */

int LoadFile(char *filename)
{
                              /*! ����� �����, ��� �������������          */
	HANDLE f = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,
			OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
                              /*! ����� ����������� ����                  */
	ULONG read = 0;
                              /*! ������� � ������ ������ ������� ������  */
	UINT pos = 0, 
                              /*! ������ �����                            */
		 filesize = 0;
                              /*! ����� ��� �������� ������ �� �����      */
	char *ptext = NULL;

                                        /*! ���� ���� �� ��������         */
	if ((!f) || (f == INVALID_HANDLE_VALUE))
                                        /*! ������� ������                */
		return 0;
	else
	{
                                        /*! ���������� ������ �����       */
		filesize = GetFileSize(f,NULL);
                                        /*! ������� ������ �����          */
		strings.clear();
		wrapped.clear();
                                        /*! �������� ������ ��� ������ �� 
										����� ���������� */
		ptext = new char [filesize + 1];
                                        /*! �������� ����������           */
		memset(ptext,0,filesize + 1);
                                        /*! ������� � ����� ����          */
		ReadFile(f,ptext,filesize,&read,NULL);
                                        /*! ���� �� ������                */
		for (UINT i = 0; i <= filesize; i++)
		{
                                        /*! ���� ������� ������, ��� �����*/
			if ((ptext[i] == '\r') || (i == filesize))
			{
				UINT j;
				int ind = 0;
                              /*! ��� �������� ���-�� �������� ��������� �
							  ������� ������ */
				int tabscount = 0;

                                        /*! ���������� ������ ���������   */
				for (j = 0; j < i - pos; j++)
					if (ptext[j + pos] == '\t')
                        tabscount++;

                                        /*! �������� ������ ��� ������ � �
										����� ������ ������� ��������� ���
										������ */
				char *temp = new char [i - pos + 1 - tabscount + TAB_SIZE * tabscount];
                                        /*! �������� ����������           */
				memset(temp,0,i - pos + 1 - tabscount + TAB_SIZE * tabscount);
                                        /*! ���� �� ������                */
				for (j = 0; j < i - pos; j++)
				{
                                        /*! ���� ��������� ������ '\t'    */
					if (ptext[j + pos] == '\t')
					{
						                /*! �������� ������ ��������� TAB_
										SIZE ��������� */
                        for (int k = 0; k < TAB_SIZE; k++)
						{
							temp[ind++] = ' ';
						}
					}
					else
					{
                                        /*! ����� ������ ����������� �� ��
										���� ������ � ������ */
						temp[ind++] = ptext[j + pos];
					}
				}

                                        /*! ��������� �������������� �����
										� � ������ ����� */
				strings.push_back(string(temp));
                                        /*! ����������� ��������� ������ �
										���� � ������ �� ����� ������� */
				pos = i + 1;
                                        /*! ���������� ������ ��� ������  */
				delete [] temp;
			}
                                        /*! ���� ��� ����������� ����� ���
										��� ������������ ���� "\r\n", ��� 
										�������� ���������� ��� windows ��
										������� ������, �� �������������� 
										��������� �� ��������� ������ �� 1*/
			if (ptext[i] == '\n')
				pos++;
		}

                                        /*! ���� ���������� ���������� ���
										��, ����� ��������� ������ */
		if (paged)
            CheckStrings(true);

                                        /*! ����� ����� �����������       */
		modified = 0;
                                        /*! ����� �������� ����������     */
		count_saves = 0;

                                        /*! ���������� ��������� ����     */
		sprintf(windowname,"%s - %s",PROGRAMNAME,filename);
		SetWindowText(hWnd,windowname);

                                        /*! ���������� ���� �����������   */
		UpdateHScrollInfo(hWnd);
		UpdateVScrollInfo(hWnd);
				
                                        /*! ������� ����                  */
		CloseHandle(f); 
		f = NULL;
                                        /*! ���������� ������, ������� ���
										����� */
		if (ptext)
		{
			delete [] ptext;
            ptext = NULL;
		}
		return 1;
	}
}


/*!
 * @function : GetLineCharsLength
 * @brief    : �������� ������ � �������� ������ num_chars �������� ������
 *
 * @param const char *line  : ������
 * @param int num_chars     : ���-�� ��������
 *
 * @return int : ������ � ��������
 */

int GetLineCharsLength(const char *line,int num_chars)
{
	char *temp = NULL;
	HideCaret(hWnd);
	HDC hdc = GetDC(hWnd);
	SIZE sz;
	
                                        /*! ���� ���-�� �������� > 0      */
	if (num_chars)
	{
                                        /*! ���� ���-�� �������� > ����� �
										��������� ������, ��������������� */
		if (num_chars > (int)strlen(line))
			num_chars = strlen(line);
                                        /*! ������� ������ � ������� num_c
										hars �������� */
		temp = new char [num_chars + 1];
		memset(temp,0,num_chars + 1);
		strncpy(temp,line,num_chars);
                                        /*! ��������� �� ������, ���������
										��������� �������� ������ */
		GetTextExtentPoint32(hdc,temp,strlen(temp),&sz);
		delete [] temp;
	}
	else
		sz.cx = 0;
	ReleaseDC(hWnd,hdc);
	ShowCaret(hWnd);
                                        /*! ���������� ������             */
	return sz.cx;
}


/*!
 * @function : FindLargestString
 * @brief    : ������� ������ � ������������ ������
 *
 * @param vector<string> &s : ��� ������
 *
 * @return int : ������ ��������� ������ � ������ ������, ����� -1.
 */

int FindLargestString(vector<string> &s)
{
                                        /*! ������ �� ������, ���� ����� �
										�� */
	if (s.empty())
		return -1;
	int len = s[0].length(), ind = 0, i;

                                        /*! ����� �� ������� ������� �����
										������� ����� ������ */
	for (i = 1; i < (int)s.size(); i++)
	{
		if ((int)s[i].length() > len)
		{
			len = s[i].length();
                                        /*! ��������� ������              */
			ind = i;
		}
	}
                                        /*! ���������� ������             */
	return ind;
}


/*!
 * @function : GetPageWidth
 * @brief    : ���������� ������ ��������, ����������� �� ������� ������
 *
 * @param none
 *
 * @return int : ����������� ������ �������� 
 */

int GetPageWidth()
{
                                        /*! � ���������� ������ ������ ���
										������� ����������� �������� */
	if (paged)
		return PAGEWIDTH;
                                        /*! �������� ������ ������ �������
										����� ����� */
                                        /*! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
										��� ��� ����� �� ������ ����������
										��, ������ ������ ����� ������ ���
										��� ��� ����������� ������ �������
										�, ��� ��� �� ����� ���� ���������
										� ������ ������ �� � ������������ 
										������ � �������, � ������������ �
										������ � �������� !!!!!!!!!!!!!!! */
	int imaxlength = FindLargestString(strings);
                                        /*! ���� �������                  */
	if (imaxlength >= 0)
	{
                                        /*! ��������� �� ������           */
		int L = GetLineCharsLength(strings[imaxlength].c_str(),
					strings[imaxlength].length());
                                        /*! � ������, ���� ������ ������ �
										����� ������ ���� ������ */
		if (L > wwidth)
                                        /*! ���������� ������ ������      */
			return L;
		else
                                        /*! ����� - ���� ������           */
			return wwidth;
	}
	else
                                        /*! ���� ����� ���, ���������� ���
										��� ���� ������ */
		return wwidth;
}



/*!
 * @function : DoBeforeCheckBound
 * @brief    : ������ �������� �� ����� �� ���������� ������� ���������� ������� �����
 *
 * @param HWND hWnd : ��� ������ ���� ������ ��������
 *
 * @return void : 
 */

void DoBeforeCheckBound(HWND hWnd)
{
                                        /*! ���� ����� ���                */
	if (strings.empty())
	{
                                        /*! ���������� �������� �� �����. */
		carline = 0;
		carchar = 0;
	}
	else
	{
                                        /*! �������� ������ �����         */
		if (carline < 0)
			carline = 0;
		if (carline > (int)strings.size() - 1)
			carline = strings.size() - 1;
                                        /*! �������� ������ ������� ����� */
		if (carchar > (int)strings[carline].length())
			carchar = strings[carline].length();
	}
}


/*!
 * @function : DoAfterCheckBound
 * @brief    : �������� ������������ ����������� �������� ��������� �������
 *
 * @param HWND hWnd : ��� ������ ���� ���������
 *
 * @return void : 
 */

void DoAfterCheckBound(HWND hWnd)
{
                                        /*! �������� �� ���������         */
	if (cary < 0)
	{
		if (ABS(cary) <= tm.tmHeight)
			SetLine(--page_line);
		else
            SetLine(carline);
		InvalidateRect(hWnd,NULL,true);
	}
	if (cary + tm.tmHeight > wheight)
	{
        while (CalcCarY(carline,carchar) + tm.tmHeight > wheight)
		{
			if (ABS(cary - wheight) <= tm.tmHeight)
				SetLine(++page_line);
			else
				SetLine(carline);
		}
		InvalidateRect(hWnd,NULL,true);
	}
	                                        /*! �������� �� ����������        */
	if (carx >= wwidth)
	{
		SetWinX(win_x + carx - wwidth);
		carx -= carx - wwidth;
		InvalidateRect(hWnd,NULL,true);
	}
	if (carx < 0)
	{
		SetWinX(win_x + carx);
		carx -= carx;
		InvalidateRect(hWnd,NULL,true);
	}
                                        /*! ����������� ��������� ��������*/
	SetPageWidth(GetPageWidth());
                                        /*! �������� �������              */
	PostMessage(hWnd,WM_USER,0,0);
                                        /*! �������� ����������           */
	UpdateVScrollInfo(hWnd);
	UpdateHScrollInfo(hWnd);
}


/*!
 * @function : SaveFile         
 * @brief    : ��������� ����� � ���� � �������� ��������                  
 *
 * @param *filename : ��� ����� ��� ����������
 *
 * @return int  : 1 - �������, 0 - ������
 */

int SaveFile(char *filename)
{
                              /*! ����� �����, ��� �������������          */
	HANDLE f = CreateFile(filename,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,
		NULL);
                              /*! ������� ����� ������                    */
	char crlf[] = "\r\n";
                              /*! ���-�� ���������� ���� � ���� �������� �
							  ����� */
	DWORD written;


                                        /*! ���� �� ��������, �����       */
	if ((f == INVALID_HANDLE_VALUE) || (!f))
		return 0;

	if (paged)
        CheckStrings(false);
                                        /*! ���� �� �������               */
	for (UINT i = 0; i < strings.size(); i++)
	{
                                        /*! ������ ������� ������         */
		WriteFile(f,strings[i].c_str(),strings[i].length(),&written,NULL);
                                        /*! ���� ������, �����            */
		if (written != strings[i].length())
		{
			CloseHandle(f);
			DeleteFile(filename);
			return 0;
		}
                                        /*! �������� ������� ������       */
        WriteFile(f,crlf,2,&written,NULL);
                                        /*! ���� ������, �����            */
		if (written != 2)
		{
			CloseHandle(f);
			DeleteFile(filename);
			return 0;
		}
	}
                                        /*! ������� ����                  */
	CloseHandle(f);
	if (paged)
        CheckStrings(true);
                                        /*! ��������� ����� ����������    */
	count_saves++;
                                        /*! �������� ���� �����������     */
	modified = 0;
                                        /*! �������� ��������� ����       */
	sprintf(windowname,"%s - %s",PROGRAMNAME,filename);
	SetWindowText(hWnd,windowname);
	return 1;
}

/*!
 * @function : UpdateVScrollInfo                 
 * @brief    : ���������� ���� ������������� ����������                     
 *
 * @param HWND hWnd : ��� ������ ���� ���������
 *
 * @return void : 
 */

void UpdateVScrollInfo(HWND hWnd)
{
	SCROLLINFO si;

                                        /*! ���������� ��������� �������  */
    si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
                                        /*! ������� ������� - ���� ������ */
	si.nMax = pages * page_lines - 1;
	si.nMin = 0;
	si.nPage = scr_lines;
	si.nPos = page_line;
                                        /*! ���������� ��� �������        */
	SetScrollInfo(hWnd,SB_VERT,&si,TRUE);
}

/*!
 * @function : UpdateHScrollInfo                 
 * @brief    : ���������� ���� ��������������� ����������
 *
 * @param HWND hWnd : ��� ������ ���� ���������
 *
 * @return void : 
 */

void UpdateHScrollInfo(HWND hWnd)
{
	SCROLLINFO si;
                                        /*! ���� ���� ������              */
	if (!strings.empty())
	{
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
                                        /*! ������� ������� - ������      */
		si.nMax = page_width + 1;// + 1;
		si.nMin = 0;
		si.nPage = wwidth;// + 1;
		si.nPos = win_x;
                                        /*! �������������� ��� �������    */
		SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
	}
	else
	{
                                        /*! �������� ��������� �������    */
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
		si.nMax = 0;
		si.nMin = 0;
		si.nPage = 0;
		si.nPos = 0;
                                        /*! �������������� ��� �������    */
		SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
	}

}

/*!
 * @function : DrawPages
 * @brief    : ���������� ��������� �������� �������     
 *
 * @param hWnd : �� ����� ���� ��������
 * @param hdc : �� ����� ��������� ����������
 * @param s_page : � ����� ��������
 * @param l_page : �� ����� ��������
 * @param page_L : ����� � ��������
 *
 * @return void  : 
 */

void DrawPages(HWND hWnd,HDC hdc,int s_page,int l_page,int page_L)
{
	HideCaret(hWnd);

	RECT rc;
                              /*! ����� �����, ������������� ������� �����
							  �, ��������� ��� ��������� ���������� */
	HBRUSH hBrush = CreateSolidBrush(background_color);
                              /*! ����� ������ �����                      */
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc,hBrush);

                                        /*! ��������� �������� ���� ������*/
	GetClientRect(hWnd,&rc);

                                        /*! ���������� �������� ����� ����
										�� ��������� �� ��� ������� */
	int y = - (page_L - s_page * page_lines)* tm.tmHeight;
                                        /*! ������ ������ ������          */
	Rectangle(hdc,-win_x,y,page_width - win_x,y + page_height);

                                        /*! ���� �� ���������             */
	for (int page_i = s_page; page_i <= l_page + 1; page_i++)
	{
                                        /*! ���������� ��������           */
        y += page_height;        	
                                        /*! ������� ��������� ������      */
		Rectangle(hdc,-win_x,y,page_width - win_x,y + page_height);
	}
	
                                        /*! �������������� ������ �����   */
	SelectObject(hdc,hBrushOld);
	DeleteObject(hBrush);
	
	ShowCaret(hWnd);
}

/*!
 * @function : SetPageWidth
 * @brief    : ������������� ������ ��������
 *
 * @param w : ����� �������� ������
 *
 * @return void  : 
 */

void SetPageWidth(int w)
{
                                        /*! ��������� ������ ��������     */
	page_width = w;
}

/*!
 * @function : SetPageHeight
 * @brief    : ������������� ������ ��������
 *
 * @param h : ����� �������� ������
 *
 * @return void  : 
 */

void SetPageHeight(int h)
{
                                        /*! ��������� ���������� �������� */
	page_height = 0;
	pages = 0;
	page_lines = 0;
                                        /*! ���� �������� ��������� ������
										�� */
	if (h)
	{
                                        /*! ����������� ���-�� ����� � ���
										����� */
		page_lines = h / tm.tmHeight;
                                        /*! ���������� ������ ��������    */
        page_height = h;
                                        /*! ���� �� ���������� �����      */
		if (!paged)
                                        /*! ���������� ������, ��������� �
										� ������ ������ */
			page_height = page_lines * tm.tmHeight;
                                        /*! ���������� ���-�� ������      */
		if (page_lines)
            pages = strings.size() / page_lines + 1;
	}
}

/*!
 * @function : CharLength
 * @brief    : ���������� ������ � �������� ������ carc �������� ������ � �������� carl
 *
 * @param carl : ������ ������
 * @param carc : ����� �������
 *
 * @return int  : ������ 
 */

int CharLength(int carl,int carc)
{
                                        /*! ���������� ������ ������ ��r� 
										�������� ������ �� ������� */
	return GetLineCharsLength(strings[carl].c_str(),carc);
}

/*!
 * @function : SetWinX
 * @brief    : ������������� �������� ������� ������ �� ����������� 
 *
 * @param winx : ����� �������� ��������
 *
 * @return void  : 
 */

void SetWinX(int winx)
{
                                        /*! ���������� �������� ������� ��
										���� �� ����������� */
	win_x = winx;
	if (win_x < 0)
		win_x = 0;
}

/*!
 * @function : SetLine
 * @brief    : ������������� ��������� ������ ������
 *
 * @param lin : ����� �������� ������ 
 *
 * @return void  : 
 */

void SetLine(int lin)
{
                                        /*! ���������� ��������� ������ ��
										���� */
	page_line = lin;
	if (page_line < 0)
		page_line = 0;	
}

/*!
 * @function : OutLine
 * @brief    : ������� ������ � �������� �����������
 *
 * @param hdc : �������� ���������� ������
 * @param line : ������ ������ ��� ������
 * @param x : ��������
 * @param y : ��������
 *
 * @return void  : 
 */

void OutLine(HDC hdc, int line,int x,int y)
{
                                        /*! �������� ������               */
	if ((line >= (int)strings.size()) || (line < 0))
	{
		error("Line invalid");
		return;
	}

                                        /*! ������� ������                */
	TextOut(hdc,x,y,strings[line].c_str(),strings[line].length());
}

/*!
 * @function : CalcCarX
 * @brief    : ���������� �������� �������
 *
 * @param carline : ����� ������ �������
 * @param carchar : ����� ������� �������
 *
 * @return int  : ������������ ��������
 */

int CalcCarX(int carline, int carchar)
{
	int add = 0;
	int str_len = 0;
	int carx = 0;

                                        /*! ���� ���� ������              */
	if (!strings.empty())
	{
                                        /*! �������� ������ ������        */
        str_len = CharLength(carline,strings[carline].length());
    
                                        /*! � ����������� �� ������������ 
										��������� ��������� �� �����������*/
		switch (halign)
		{
		case IDM_HALIGNLEFT:
            break;
		case IDM_HALIGNCENTER:
			add = (page_width - str_len) / 2;
			break;
		case IDM_HALIGNRIGHT:
			add = page_width - str_len;
		}

                                        /*! ���������� �������� �������   */
		carx = add - win_x + CharLength(carline,carchar);
	}
	else
        carx = add - win_x;
	return carx;
}

/*!
 * @function : CalcCarY
 * @brief    : ������������ �������� �������
 *
 * @param carline : ����� ������ �������
 * @param carchar : ����� ������� �������
 *
 * @return int  : ������������ ��������
 */

int CalcCarY(int carline, int carchar)
{
	int cary;

                                        /*! ����������� ��������� ��������*/
	SetPageHeight(page_height);
                                        /*! �������� ����� ������� �������
										� */
	if (page_lines)
		page = GETPAGE(carline,page_lines);
	if (page_lines)
	{
		int pline = carline % page_lines;
		int s_page = GETPAGE(page_line,page_lines);
		int curr_lines = page_lines;

		if (curr_lines + page * page_lines >= (int)strings.size())
			curr_lines = strings.size() - page * page_lines;

                                        /*! ���� ���������� �����         */
		if (paged)
                                        /*! ������������ ������. ��������.*/
			switch (valign)  
			{ 
                                        /*! ������� � ����������� �� �����
										������� */
			case IDM_VALIGNTOP:
				cary = page * page_height + pline * tm.tmHeight - s_page * page_height
					- (page_line - s_page * page_lines) * tm.tmHeight;
				break;
			case IDM_VALIGNCENTER:
				cary = page * page_height + pline * tm.tmHeight + (page_lines - curr_lines) * tm.tmHeight / 2 
					 - s_page * page_height	- (page_line - s_page * page_lines) * tm.tmHeight + page_height % tm.tmHeight / 2;
				break;
			case IDM_VALIGNBOTTOM:
				cary = page * page_height + pline * tm.tmHeight + (page_lines - curr_lines) * tm.tmHeight
					- s_page * page_height	- (page_line - s_page * page_lines) * tm.tmHeight + page_height % tm.tmHeight;
				break;
			}
		else
                                        /*! ���������� �������� ������� ��
										��������� ������������ */
			cary = page * page_height + (pline - page_line) * tm.tmHeight;

    }
	else
                                        /*! �������� �� ���������         */
		cary = page * page_height - page_line * tm.tmHeight;
	return cary;
}

/*!
 * @function : CheckStrings
 * @brief    : ��������� ������
 *
 * @param wrap : ��������� �������� �������, � �� ���������
 *
 * @return void  : 
 */

void CheckStrings(bool wrap)
{
	string app;
	bool found = false;
	int i;

                                        /*! ���� ��� �����, �����         */
	if (strings.empty())
		return;

    if (!wrapped.empty())
	{
		i = 0;
		
		while (i < (int)wrapped.size())
		{
			int wrind = wrapped[i], wrind_bckp = wrapped[i];
			if (wrind >= (int)strings.size())
				break;

			int unwr = 0;
			string full_unwrap = strings[wrind];
            
			do
			{
				if (wrind + 1 < (int)strings.size())
					full_unwrap += strings[wrind + 1];
				wrind++;
				i++;
				if (i < (int)strings.size())
                    if (wrapped[i] != wrind)
						unwr = 1;
			}
			while ((i != (int)wrapped.size()) && (!unwr));
			strings.erase(strings.begin() + wrind_bckp,strings.begin() + wrind + 1);  
			strings.insert(strings.begin() + wrind_bckp,full_unwrap);
            
			for (int k = i; k < (int)wrapped.size(); k++)
				wrapped[k] -= wrind - wrind_bckp;
		}
	}
	wrapped.clear();
	if (!wrap)
		return;
	                                    /*! ���� �� ������� �� from_line  */
	for (i = 0/*from_line*/; i < (int)strings.size(); i++)
	{
                                        /*! ��������� ����� �� ��������   */
		if (CharLength(i,strings[i].length()) > page_width)
		{
			wrapped.push_back(i);
                                        /*! ���������� ������� ������     */
			found = true;
			app = "";
                                        /*! ���� ������ �� ������ � ������
										��, ���������� �� 1-�� ������� */
			while (CharLength(i,strings[i].length()) > page_width)
			{
				app.insert(0,strings[i],strings[i].length() - 1,1);
				strings[i].erase(strings[i].length() - 1,1);
			}
                                        /*! ���� �� ��������� ������      */
			if (strings.size() != i + 1)
                                        /*! �������� � ��������           */
				strings.insert(strings.begin() + i + 1,app);
				//strings[i + 1].insert(0,app);
			else
                                        /*! ����� � �����                 */
				strings.push_back(app);
                                        /*! ���������� ���� �����������   */
		}
	}
}


/*!
 * @function : StringWrapped
 * @brief    : ����������, �������� �� ������ ������������
 *
 * @param int carline : ����� ������
 *
 * @return bool : �� / ���
 */

bool StringWrapped(int carline)
{
    int i;

	for (i = 0; i < (int)wrapped.size(); i++)
		if (wrapped[i] == carline)
			return true;
	return false;
}


/*!
 * @function : UnCheckWrap
 * @brief    : ������� ����� "����������" �� ������
 *
 * @param int carline : ����� ������
 *
 * @return void : 
 */

void UnCheckWrap(int carline)
{
    int i;

	for (i = 0; i < (int)wrapped.size(); i++)
		if (wrapped[i] == carline)
		{
			wrapped.erase(wrapped.begin() + i);
			return;
		}
}


/*!
 * @function : ShiftWrap
 * @brief    : �������� ����� ����� �� delta
 *
 * @param int from_i  : ������ �������� �����
 * @param int delta   : �� ������� ��������
 *
 * @return void : 
 */

void ShiftWrap(int from_val, int delta)
{
	int i = 0;

	for (i = 0; i < (int)wrapped.size(); i++)
		if (wrapped[i] >= from_val)
            wrapped[i] += delta;
}


/*!
 * @function : GetUnwrapped
 * @brief    : ���������� ������ ������������ ������ (��������������)
 *
 * @param int carline : ����� ������
 *
 * @return string : ������������ ������
 */

string GetUnwrapped(int carline)
{
	string unwr = strings[carline];

    if (!StringWrapped(carline))
		return unwr;

	int i;
	int prev = carline;

	for (i = 0; i < (int)wrapped.size(); i++)
	{
		if (wrapped[i] > carline)
		{
			if (wrapped[i] - prev > 1)
				break;
			prev = wrapped[i];
			unwr += strings[wrapped[i]];
		}
	}
	return unwr;
}


/*!
 * @function : CheckWrap
 * @brief    : �������� ������ ��� ������������
 *
 * @param int carline : ����� ������
 *
 * @return void : 
 */

void CheckWrap(int carline)
{
	if (!StringWrapped(carline))
	{
		int i;
		for (i = 0; i < (int)wrapped.size(); i++)
			if (wrapped[i] > carline)
			{
				wrapped.insert(wrapped.begin() + i - 1, carline);
				break;
			}
	}
}
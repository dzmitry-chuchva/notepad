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

                                        /*! используем чистый WinAPI      */
#define WIN32_LEAN_AND_MEAN
                                        /*! подключение используемых загол
										овочных файлов */
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <vector>
#include <string>
                                        /*! определение сообщения колесика
										мыши */
#define WM_MOUSEWHEEL 0x020A
                                        /*! подключение заголовочного файл
										а ресурсов */
#include "mainrc.h"
                                        /*! определения размера по умолчан
										нию для строки */
#define MAX_STR 300
                                        /*! кол-во пробелов, заменяющих си
										мвол табуляции */
#define TAB_SIZE 8
                                        /*! название программы            */
#define PROGRAMNAME "PICO v.0.1a"
                                        /*! имя файла по умолчанию        */
#define DEFFILENAME "UNNAMED.txt"
                                        /*! класс, стиль, положение окна  */
#define CLASSNAME "PICOCLASS"
#define WINDOWSTYLE (WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL)
#define WINDOWEXSTYLE 0
#define WINDOWPOSX CW_USEDEFAULT
#define WINDOWPOSY CW_USEDEFAULT
#define WINDOWWIDTH CW_USEDEFAULT
#define WINDOWHEIGHT CW_USEDEFAULT
                                        /*! ширина каретки                */
#define CARETWIDTH 2
                                        /*! высота и ширина страницы      */
#define PAGEWIDTH 740
#define PAGEHEIGHT 248
                                        /*! макрос получения страницы по с
										троке */
#define GETPAGE(line,linesperpage) ((line) / (linesperpage))
                                        /*! макрос "модуль числа"         */
#define ABS(x) (((x) >= 0) ? (x) : (-x))

                                        /*! для прямого доступа к классам 
										стандартной библиотеки */
using namespace std;

                              /*! хэндлы окна, модуля, табл. акселераторов*/
HWND hWnd = NULL;
HINSTANCE hInst = NULL;
HACCEL hAccTable = NULL;
                              /*! хэндлы шрифтов, описатель шрифта        */
HFONT hFont = NULL, hFontOld = NULL;
LOGFONT lf;
                              /*! указатель на функцию вывода ошибок      */
void (*error)(char*) = NULL;
                              /*! текущие имя окна и имя файла            */
char windowname[MAX_STR];
char filename[MAX_STR];
                              /*! текущие ширина и высота окна            */
int wwidth,wheight;
                              /*! текущие ширина и высота страницы        */
int page_width, page_height;
                              /*! флаг страничного режима                 */
int paged = 0;
                              /*! текущая страница; всего страниц         */
int page = 0, pages = 0; 
                              /*! текущая "опорная" строка - строка, начин
							  ая с которой выводится текст; строк на стран
							  ице */
int page_line = 0, page_lines = 0;
                              /*! сдвиг окна просмотра по горизонтали     */
int win_x = 0;
                              /*! строк в окне вывода                     */
int scr_lines = 0;
                              /*! полный путь к текущему файлу            */
char path[MAX_STR];
                              /*! вектор строк редактируемого текста      */
vector<string> strings;
vector<int> wrapped;
                              /*! текущая метрика контекста устройств     */
TEXTMETRIC tm;
                              /*! текущие выравнивания                    */
int valign = IDM_VALIGNTOP,halign = IDM_HALIGNLEFT;
                              /*! текущая позиция каретки                 */
int carx = 0,cary = 0;
                              /*! позиция ввода (строка,символ)           */
int carline = 0, carchar = 0;
                              /*! цвет фона и текста                      */
COLORREF background_color = RGB(255,255,255), text_color = RGB(0,0,0);
                              /*! флаг измененного файла                  */
int modified = 0;
                              /*! кол-во сохранений файла                 */
unsigned int count_saves = 0;
bool passed_save_dialog = false;

                                        /*! прототипы функций             */
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
 * @brief    : производит освобождение занятых ресурсов и их очистку 
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
 * @brief    : функция ошибки, выводящая ошибку на экран и корректно выходит с очисткой
 *			   используется до создания окна
 * @param char *mesg : строка с ошибкой для вывода
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
 * @brief    : функция ошибки, выводящая ошибку на экран и корректно выходит с очисткой
 *			   используется после создания окна
 * @param char *mesg : строка с ошибкой для вывода
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
 * @brief    : оконная процедура программы
 *
 * @param HWND hWnd     : хэндл окна
 * @param UINT messg    : сообщение для этого окна
 * @param WPARAM wParam : первый параметр сообщения
 * @param LPARAM lParam : второй параметр сообщения
 *
 * @return LRESULT CALLBACK : результат обработки (да - 0, нет - иначе) 
 */

LRESULT CALLBACK WndProc(HWND hWnd,UINT messg,WPARAM wParam,LPARAM lParam)
{
                              /*! структура рисования для WM_PAINT        */
	PAINTSTRUCT ps;
                              /*! структура для хранения клиентской област
							  и окна*/
	RECT rc;
                              /*! контекст устройтсва окна                */
	HDC hdc = NULL;
                              /*! для сообщения WM_COMMAND                */
	WORD wID = LOWORD(wParam);
	HWND hwndCtl = (HWND)lParam;
		
	switch (messg)
	{
	case WM_CREATE:
                                        /*! сохраняем используемый по умол
										чанию шрифт и получаем его метрику*/ 
		hdc = GetDC(hWnd);
		hFontOld = (HFONT)GetCurrentObject(hdc,OBJ_FONT);
		GetTextMetrics(hdc,&tm);
                                        /*! устанавливаем "прозрачный" реж
										им вывода текста */
		SetBkMode(hdc,TRANSPARENT);
		ReleaseDC(hWnd,hdc);		
		return 0;
	case WM_CLOSE:
                                        /*! если файл был изменен, предлаг
										аем сохранить изменения перед выхо
										дом */
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
                                        /*! уничтожаем окно               */
		SendMessage(hWnd,WM_DESTROY,0,0);
		return 0;
	case WM_DESTROY:
                                        /*! посылаем WM_QUIT (выход)      */
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
                                        /*! получаем клиентскую область и 
										начинаем рисовать :) */
		GetClientRect(hWnd,&rc);
		hdc = BeginPaint(hWnd,&ps);
                                        /*! если есть что рисовать        */
		if (!strings.empty())
		{
                                        /*! определяем первую стрницу, поп
										адающую в окно вывода */
			int start_page = GETPAGE(page_line,page_lines);
                                        /*! аналогично для последней стран
										ицы */
			int last_page = start_page + wheight / page_lines;
                                        /*! абсолютное смещение в пикселях
										первой строки для рисования */
			int y_offset = start_page * page_height + (page_line - start_page * page_lines) * tm.tmHeight;
                                        /*! флаг необходимости выхода из ц
										икла рисования */
			bool exit_need = false;
                                        /*! модификаторы точки вывода стро
										ки при использовании выравнивания */
			int addh = 0, addv = 0;

                                        /*! при выходе расчетного значения
										за границы, приводим его в требуем
										ый диапазон значений */
			if (last_page >= pages)
				last_page = pages - 1;
                                        /*! в страничном режиме рисуем соб
										ственно страницы */
			if (paged)
				DrawPages(hWnd,hdc,start_page,last_page,page_line);
                                        /*! расчитываем модификатор горизо
										нтального выравнивания */
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
                                        /*! цикл прорисовки по страницам  */
			for (int page_i = start_page; page_i <= last_page; page_i++)
			{
                                        /*! определение начальной и конечн
										ой строки в рамках прорисовываемой
										страницы */
				int start_line = 0;
				int last_line = page_lines - 1;

                                        /*! если это первая прорисовываема
										я страница */
				if (page_i == start_page)
                                        /*! рассчитываем новое значение дл
										я начальной строки, вместо стандар
										тного нуля */
					start_line = page_line - start_page * page_lines;
                                        /*! если надо выйти, выходим      */
				if (exit_need)
					break;
				                        /*! при выходе расчетного значения
										за границы, приводим его в требуем
										ый диапазон значений */
				if (last_line + page_i * page_lines >= (int)strings.size())
				{
					last_line = strings.size() - page_i * page_lines - 1;
                                        /*! строки кончились, надо выйти  */
					exit_need = true;
				}
                                        /*! в страничном режиме рассчитыв
										аем еще и вертикальный модификатор
										выравнивания */
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
                                        /*! цикл рисования по строкам     */
				for (int line_i = start_line; line_i <= last_line; line_i++)
					OutLine(hdc,
							page_i * page_lines + line_i,
							addh - win_x,
							addv + (page_i * page_height) + line_i * tm.tmHeight - y_offset
							);
			}
		}
                                        /*! конец рисования               */
		EndPaint(hWnd,&ps);
		return 0;
	case WM_SIZE:
		{
			HDC hdc = GetDC(hWnd);
                                        /*! сохраняем новые значения облас
										ти вывода */
			GetClientRect(hWnd,&rc);
			wheight = rc.bottom - rc.top;
			wwidth = rc.right - rc.left - 2;
                                        /*! пересчитываем кол-во страниц в
										области вывода */
			scr_lines = wheight / tm.tmHeight;
			ReleaseDC(hWnd,hdc);
                                        /*! если не страничный режим      */
			if (!paged)
			{
                                        /*! установить новые значения пара
										метров страницы */
				SetPageHeight(wheight);
				SetPageWidth(GetPageWidth());
			}
                                        /*! обновить скроллбары           */
			UpdateVScrollInfo(hWnd);
			UpdateHScrollInfo(hWnd);
                                        /*! обновить каретку              */
			PostMessage(hWnd,WM_USER,0,0);
		}		
		return 0;
	case WM_COMMAND:
		switch (wID)
		{
		case IDM_NEW:
			{
                                        /*! если файл был изменен, предлаг
										аем сохранить изменения перед созд
										анием нового файла */
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

                                        /*! устанавливаем имя окна и имя ф
										айла по умолчанию */
				sprintf(temp,"%s - %s",PROGRAMNAME,DEFFILENAME);
				sprintf(filename,"%s",DEFFILENAME);
				SetWindowText(hWnd,temp);
                                        /*! подготавливаем вектор строк   */
				strings.clear();
				wrapped.clear();
                carline = carchar = 0;
                                        /*! сброс флага модификации       */
				modified = 0;
                                        /*! файл ни разу не сохраняли     */
				count_saves = 0;
                                        /*! перерисовать                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! обновить каретку              */
				PostMessage(hWnd,WM_USER,0,0);
			}
			break;
		case IDM_NEWINSTANCE:
                                        /*! запустим еще один редактор    */
			WinExec(path,SW_SHOW);
			break;
		case IDM_OPEN:
			{
										/*! если файл был изменен, предлаг
										аем сохранить изменения перед откр
										ытием файла */
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

                                        /*! подготавливаем структуру для д
										иалога выбора файла для открытия */
				memset(&ofn,0,sizeof(ofn));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = "Text files\0*.txt\0All files\0*.*\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = filename;
				ofn.nMaxFile = MAX_STR;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

                                        /*! если пользователь что-то выбра
										л */
				if (GetOpenFileName(&ofn))
				{
                                        /*! попробовать загрузить         */
					if (LoadFile(ofn.lpstrFile))
					{
                                        /*! обновить имя файла            */
						sprintf(filename,"%s",ofn.lpstrFile);
                                        /*! обновить координаты ввода     */
						carline = carchar = 0;
                                        /*! обновить параметры страницы   */
						SetPageHeight(page_height);
                                        /*! перерисовать                  */
						InvalidateRect(hWnd,NULL,TRUE);
					}
					else
                                        /*! ошибка                        */
						MessageBox(hWnd,"Error while loading file","Error",MB_OK | MB_ICONSTOP);
				}
			}
                                        /*! обновить каретку              */
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_SAVE:
                                        /*! если файл совершенно новый    */
			if ((strcmp(filename,DEFFILENAME) == 0) && (!count_saves))
			{
                                        /*! форсировать Save as...        */
				SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SAVEAS,0),0);
				break;
			}
			else
                                        /*! иначе просто сохранить        */
				if (!SaveFile(filename))
					MessageBox(hWnd,"Error while saving file","Error",MB_OK | MB_ICONSTOP);
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_SAVEAS:
			{
				OPENFILENAME ofn;
				char defext[] = "txt";

                                        /*! готовим диалог сохранения     */
				memset(&ofn,0,sizeof(ofn));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = "Text files\0*.txt\0All files\0*.*\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = filename;
				ofn.lpstrDefExt = defext;
				ofn.nMaxFile = MAX_STR;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

                                        /*! вызываем диалог               */
				if (GetSaveFileName(&ofn))
				{
					passed_save_dialog = false;
					                    /*! в случае успеха сохраняем     */
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
                                        /*! посылаем WM_CLOSE             */
			SendMessage(hWnd,WM_CLOSE,0,0);
			break;
		case IDM_COLORS:
			{
				CHOOSECOLOR chclr;
				static COLORREF initbclr[16];

                                        /*! готовим диалог выбора цвета   */
				for (int i = 0; i < 16; i++)
					initbclr[i] = RGB(255,255,255);
				memset(&chclr,0,sizeof(CHOOSECOLOR));
				initbclr[0] = background_color;
				chclr.lStructSize = sizeof(CHOOSECOLOR);
				chclr.hwndOwner = hWnd;
				chclr.lpCustColors = initbclr;
                                        /*! вызываем его                  */
				if (ChooseColor(&chclr))
				{
                                        /*! устанавливаем фоновый цвет    */
					background_color = chclr.rgbResult;
                                        /*! перерисовать                  */
					InvalidateRect(hWnd,NULL,true);
				}
			}
			break;
		case IDM_FONT:
			{
				HideCaret(hWnd);

				CHOOSEFONT cf;
				HDC hdc = GetDC(hWnd);

                                        /*! готовим диалог выбора шрифта  */
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
                                        /*! вызываем его; если успешно... */
				if (ChooseFont(&cf))
				{
                                        /*! ...удаляем предыдущий шрифт   */
					if (hFont)
					{
		                SelectObject(hdc,hFontOld);
						DeleteObject(hFont);
						hFont = NULL;
					}
                                        /*! создаем новый                 */
					hFont = CreateFontIndirect(&lf);
                                        /*! устанавливаем его             */
					hFontOld = (HFONT)SelectObject(hdc,hFont);
					text_color = cf.rgbColors;
                                        /*! устанавливаем цвет шрифта     */
					SetTextColor(hdc,text_color);
				}
				ReleaseDC(hWnd,hdc);
                                        /*! обновляем метрику шрифта      */
				hdc = GetDC(hWnd);
				memset(&tm,0,sizeof(TEXTMETRIC));
				GetTextMetrics(hdc,&tm);
				ReleaseDC(hWnd,hdc);
                                        /*! пересоздаем каретку с новой вы
										сотой */
				DestroyCaret();
				CreateCaret(hWnd,(HBITMAP)NULL,CARETWIDTH,tm.tmHeight);
				ShowCaret(hWnd);
                                        /*! обновляем параметры страницы  */
				SetPageHeight(page_height);
				if (paged)
                    CheckStrings(true);
                                        /*! перерисовать                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! обновить каретку              */
				PostMessage(hWnd,WM_USER,0,0);
			}
			break;
		case IDM_PAGED:
                                        /*! изменяем режим на противополож
										ный */
			paged = 1 - paged;
			HideCaret(hWnd);
			if (paged)
			{
                                        /*! обновляем вид меню            */
				CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),IDM_PAGED,MF_BYCOMMAND | MF_CHECKED);
				EnableMenuItem(GetSubMenu(GetMenu(hWnd),1),4,MF_BYPOSITION | MF_ENABLED);
                                        /*! устанавливаем новые параметры 
										страницы */
				SetPageWidth(PAGEWIDTH);
				SetPageHeight(PAGEHEIGHT);
                                        /*! переносим строки              */
				CheckStrings(true);
                                        /*! перерисовать                  */
				InvalidateRect(hWnd,NULL,true);
			}
			else
			{
                                        /*! обновляем меню                */
				CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),IDM_PAGED,MF_BYCOMMAND | MF_UNCHECKED);
				EnableMenuItem(GetSubMenu(GetMenu(hWnd),1),4,MF_BYPOSITION | MF_GRAYED);
				HDC hdc = GetDC(hWnd);
				RECT rc;
                                        /*! устанавливаем параметры страни
										цы для обычного режима */
				GetClientRect(hWnd,&rc);
				SetPageWidth(GetPageWidth());
				SetPageHeight(rc.bottom - rc.top);
				ReleaseDC(hWnd,hdc);
				CheckStrings(false);
                                        /*! перерисовать                  */
				InvalidateRect(hWnd,NULL,true);
			}
			carchar = carline = 0;
			ShowCaret(hWnd);
                                        /*! обновить каретку              */
			PostMessage(hWnd,WM_USER,0,0);
			DoAfterCheckBound(hWnd);
			break;
		case IDM_VALIGNTOP:
                                        /*! обновляем меню                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNTOP,MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNBOTTOM,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNCENTER,MF_BYCOMMAND | MF_UNCHECKED);
                                        /*! устанавливаем выравнивание    */
			valign = wID;
                                        /*! перерисовать                  */
			InvalidateRect(hWnd,NULL,true);
                                        /*! обновить каретку              */
			PostMessage(hWnd,WM_USER,0,0);
            break;
		case IDM_VALIGNBOTTOM:
			                            /*! обновляем меню                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNTOP,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNBOTTOM,MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNCENTER,MF_BYCOMMAND | MF_UNCHECKED);
			                            /*! устанавливаем выравнивание    */
			valign = wID;
                                        /*! перерисовать                  */
			InvalidateRect(hWnd,NULL,true);
                                        /*! обновить каретку              */
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_VALIGNCENTER:
			                            /*! обновляем меню                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNTOP,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNBOTTOM,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),4),IDM_VALIGNCENTER,MF_BYCOMMAND | MF_CHECKED);
			                            /*! устанавливаем выравнивание    */
			valign = wID;
                                        /*! перерисовать                  */
			InvalidateRect(hWnd,NULL,true);
                                        /*! обновить каретку              */
			PostMessage(hWnd,WM_USER,0,0);
			break;
		case IDM_HALIGNLEFT:
			                            /*! обновляем меню                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNLEFT,MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNCENTER,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNRIGHT,MF_BYCOMMAND | MF_UNCHECKED);
			                            /*! устанавливаем выравнивание    */
			halign = wID;
			{
				HideCaret(hWnd);
				HDC hdc = GetDC(hWnd);
                                        /*! обновляем выравнивание для кон
										текста устройства */
				SetTextAlign(hdc,TA_LEFT | TA_TOP | TA_NOUPDATECP);
				ReleaseDC(hWnd,hdc);
                ShowCaret(hWnd);
				                        /*! перерисовать                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! обновить каретку              */
				PostMessage(hWnd,WM_USER,0,0);
			}
			break;
		case IDM_HALIGNRIGHT:
			                            /*! обновляем меню                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNLEFT,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNCENTER,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNRIGHT,MF_BYCOMMAND | MF_CHECKED);
			                            /*! устанавливаем выравнивание    */
			halign = wID;
			{
				HideCaret(hWnd);
				HDC hdc = GetDC(hWnd);
                                        /*! обновляем выравнивание для кон
										текста устройства */
				SetTextAlign(hdc,TA_RIGHT | TA_TOP | TA_NOUPDATECP);
				ReleaseDC(hWnd,hdc);
				ShowCaret(hWnd);
				                        /*! перерисовать                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! обновить каретку              */
				PostMessage(hWnd,WM_USER,0,0);
			}
			break;
		case IDM_HALIGNCENTER:
			                            /*! обновляем меню                */
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNLEFT,MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNCENTER,MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetSubMenu(GetMenu(hWnd),1),3),IDM_HALIGNRIGHT,MF_BYCOMMAND | MF_UNCHECKED);
			                            /*! устанавливаем выравнивание    */
			halign = wID;
			{
				HideCaret(hWnd);
				HDC hdc = GetDC(hWnd);
                                        /*! обновляем выравнивание для кон
										текста устройства */
				SetTextAlign(hdc,TA_CENTER | TA_TOP | TA_NOUPDATECP);
				ReleaseDC(hWnd,hdc);
				ShowCaret(hWnd);
				                        /*! перерисовать                  */
				InvalidateRect(hWnd,NULL,true);
                                        /*! обновить каретку              */
				PostMessage(hWnd,WM_USER,0,0);				
			}
			break;
		case IDM_HELP:
                                        /*! выводим справку :)            */
			MessageBox(hWnd,"Help yourself :)","Info",MB_OK | MB_ICONINFORMATION);
			break;
		case IDM_ABOUT:
                                        /*! выводим диалог "О программе"  */
			DialogBoxParam(hInst,"aboutdialog",hWnd,DlgProc,(long)hWnd);
			break;
		}
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
                                        /*! обработка нажатий Home,C+Home */
		case VK_HOME:
                                        /*! если строк нет, выйти         */
			if (strings.empty())
				break;
                                        /*! если нажали Ctrl + Home       */
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
                                        /*! переход в начало документа    */
				page = 0; page_line = 0;
				carline = 0;
				carchar = 0;
			}
			else
			{
                                        /*! иначе в начало строки         */
				carchar = 0;
			}
                                        /*! перерисовать                  */
			InvalidateRect(hWnd,NULL,true);
			break;
		case VK_END:
                                        /*! если строк нет, выйти         */
			if (strings.empty())
				break;
                                        /*! если нажали Ctrl + End        */
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
                                        /*! переход в конец документа     */
				carline = strings.size() - 1;
				carchar = strings[carline].length();
				page = pages - 1;
				page_line = strings.size() - scr_lines - 1;
			}
			else
			{
                                        /*! иначе в конец строки          */
				carchar = strings[carline].length();
			}
                                        /*! перерисовать                  */
			InvalidateRect(hWnd,NULL,true);
			break;
		case VK_ESCAPE:
                                        /*! если нажали Esc, послать WM_CL
										OSE */
			SendMessage(hWnd,WM_CLOSE,0,0);
			break;
		case VK_DOWN:
                                        /*! сдвинуть точку ввода на строку
										вниз */
			carline++;
			break;
		case VK_UP:
                                        /*! сдвинуть точку ввода на строку
										вверх */
			carline--;
			break;		
		case VK_NEXT:
                                        /*! если нажали PgDown, сдвинуть н
										а один экран вниз*/
			carline += scr_lines;
                                        /*! проверить выход за границы    */
			if (carline >= (int)strings.size() - scr_lines)
				carline = (int)strings.size() - 1;
                                        /*! сдвинуть также строку вывода  */
			page_line += scr_lines;
                                        /*! проверить выход за границы    */
			if (page_line >= (int)strings.size() - scr_lines)
				page_line = (int)strings.size() - scr_lines;
                                        /*! установить строку вывода      */
			SetLine(page_line);
                                        /*! перерисовать                  */
			InvalidateRect(hWnd,NULL,true);
		    break;
		case VK_PRIOR:
                                        /*! если нажали PgUp, сдвинуть на 
										один экран вверх */
			carline -= scr_lines;
                                        /*! проверить выход за границы    */
			if (carline < 0)
				carline = 0;
		    break;
		case VK_RIGHT:
                                        /*! если строк нет, выйти         */
			if (strings.empty())
				break;
                                        /*! приращение координаты символа 
										ввода */
			carchar++;
                                        /*! если дошли до конца строки    */
			if (carchar > (int)strings[carline].length())
			{
				if (carline < (int)strings.size() - 1)
				{
                                        /*! если не последняя строка, прир
										астить координаты строки ввода */
					carchar = 0;
					carline++;
				}
				else
                                        /*! иначе оставить на месте       */
					carchar = strings[carline].length();
			}
			break;
		case VK_LEFT:
                                        /*! если строк нет, выйти         */
			if (strings.empty())
				break;
                                        /*! уменьшить координату сивола вв
										ода*/
			carchar--;
                                        /*! если дошли до начала строки   */
			if (carchar < 0)
			{
				if (carline > 0)
				{
                                        /*! если не первая строка, перейти
										на строку вверх */
					carline--;
                                        /*! координату сивола ввода в коне
										ц строки */
					carchar = strings[carline].length();
				}
				else
                                        /*! иначе оставить на месте       */
					carchar = 0;
			}
		    break;
		case VK_TAB:
			{
                                        /*! если нажали TAB, заменить его 
										TAB_SIZE пробелами */
				for (int i = 0; i < TAB_SIZE; i++)
                                        /*! послать один пробел           */
					SendMessage(hWnd,WM_CHAR,32,0);
			}
			break;
                                        /*! обработка Enter               */
		case VK_RETURN:
			{
                string app;

                                        /*! если строк нет                */
				if (strings.empty())
                                        /*! создать одну пустую           */
					strings.push_back("");
                                        /*! если координата символа ввода 
										в конце строки */
				if (carchar == strings[carline].length())
                                        /*! ничего не переносить          */
					app = "";
				else
				{
                                        /*! иначе перенести все от координ
										аты сивола ввода до конца строки */
					app.append(strings[carline],carchar,strings[carline].length() - carchar);
                                        /*! удалить перенесенное из строки*/
					strings[carline].erase(carchar,strings[carline].length() - carchar);
				}
                                        /*! перейти на новую строку       */
				carline++;
                                        /*! если не последняя             */
				if (strings.size() != carline)
				{
                                        /*! вставить то, что хотим перенес
										ти */
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
                                        /*! иначе добавить в конец        */
					strings.push_back(app);
                                        /*! координату ввода символа в нач
										ало новой строки*/
				carchar = 0;
                                        /*! установить флаг модификации   */
				modified = 1;
				InvalidateRect(hWnd,NULL,true);
			}
			break;
                                        /*! обработка Backspace           */
		case VK_BACK:
                                        /*! если строки есть              */
			if (!strings.empty())
			{
                                        /*! если в начале строки          */
				if (carchar == 0)
				{
                                        /*! если не первая строка         */
					if (carline != 0)
					{
                                        /*! если не страничный режим      */
						if (!paged)
						{
                                        /*! координата символа ввода - дли
										на строки над текущей */
							carchar = strings[carline - 1].length();
                                        /*! если длина текущей строки не 0*/
							if (strings[carline].length() != 0)
								        /*! просто добавить текущую строку
										к строке над текущей*/
								strings[carline - 1].append(strings[carline],0,strings[carline].length());
                                        /*! удалить текущую строку        */
							strings.erase(strings.begin() + carline,strings.begin() + carline + 1);
                                        /*! перейти на строку, к которой д
										обавляли*/
							carline--;
						}
						else
						{
                                        /*! если при добавлении строка вых
										одит за границы страницы */
						/*	if ((CharLength(carline - 1,strings[carline - 1].length()) + 
                                CharLength(carline,strings[carline].length())) > 
								page_width)
							{
                                        /*! удалить с конца плохой строки 
										один символ */
						/*		strings[carline - 1].erase(strings[carline - 1].length() - 1,1);
							} */
                                        /*! иначе то же самое, что и в обы
										чном режиме */
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

                                        /*! установить флаг модификации   */
						modified = 1;
					} // if (carline != 0)
				}
				else
				{
                                        /*! если в середине строки, то уда
										лить один символ перед координатой
										ввода */
					carchar--;
					strings[carline].erase(carchar,1);
					if (paged)
						if (StringWrapped(carline))
//							if (CharLength(carline,strings[carline].length()) + CharLength(carline + 1,
//								strings[carline + 1].length()) <= page_width)
                                    CheckStrings(true);
                                        /*! усановить флаг модификации    */
					modified = 1;
				} // if (carchar == 0)
                                        /*! перерисовать                  */
				InvalidateRect(hWnd,NULL,true);
			} // if (!strings.empty())
			break;
                                        /*! обработка нажатия Delete      */
		case VK_DELETE:
                                        /*! если строк нет, выйти         */
			if (strings.empty())
				break;
                                        /*! если текущая строка не пуста  */
			if (!strings[carline].empty())
			{
                                        /*! если позиция ввода в конце стр
										оки */
				if (carchar == strings[carline].length())
				{
                                        /*! если не последняя строка      */
					if (carline != strings.size() - 1)
					{
                                        /*! если не страничный режим      */
						if (!paged)
						{
                                        /*! добавить в конец текущей строк
										и следующую за ней строку */
							strings[carline] += strings[carline + 1];
                                        /*! удалить добавляемую строку с е
										е места */
							strings.erase(strings.begin() + carline + 1,strings.begin() + carline + 2);
						}
						else
						{
                                        /*! если нельзя добавить без выход
										а за границы страницы */
							if ((CharLength(carline + 1,strings[carline + 1].length()) + 
                                CharLength(carline,strings[carline].length())) > 
								page_width)
							{
                                        /*! удалить с начала следующей стр
										оки один символ*/
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
                                        /*! установить флаг модификации   */
						modified = 1;
					} // if (carline != strings.size() - 1)
				} // if (carchar == strings[carline].length())
				else
				{
                                        /*! если в середине строки, удалит
										ь символ за текущей позицией ввода*/
					strings[carline].erase(carchar,1);
					if (paged)
						if (StringWrapped(carline))
//							if (CharLength(carline,strings[carline].length()) + CharLength(carline + 1,
//								strings[carline + 1].length()) <= page_width)
                                    CheckStrings(true);
                                        /*! установить флаг модификации   */
					modified = 1;
				}
			} // if (!strings[carline].empty())
			else
                                        /*! если текущая строка пуста     */
				if (carline != strings.size() - 1)
				{
                                        /*! удалить ее                    */
					strings[carline] += strings[carline + 1];
                    strings.erase(strings.begin() + carline + 1);
					UnCheckWrap(carline + 1);
					ShiftWrap(carline + 1,-1);
                                        /*! установить флаг модификации   */
					modified = 1;
				}
                                        /*! перерисовать                  */
			InvalidateRect(hWnd,NULL,true);
        	break;
		}
                                        /*! проверка границ текущей позици
										и ввода*/
		DoBeforeCheckBound(hWnd);
                                        /*! обновление каретки            */
		PostMessage(hWnd,WM_USER,0,0);
                                        /*! проверка границ координаты кар
										етки */
		DoAfterCheckBound(hWnd);
		return 0;
                                        /*! обработка алфавитно-цифровых с
										имволов */
	case WM_CHAR:
                                        /*! если символ выводится         */
		if ((wParam >= 32) && (wParam <= 255))
		{
                                        /*! флаг "позиция ввода в конце стр
										оки" */
			int car_in_end = false;
                                        /*! если строк нет                */
			if (strings.empty())
                                        /*! создать одну пустую           */
				strings.push_back("");
                                        /*! если позиция ввода в конце стр
										оки */
			if (strings[carline].length() != carchar)
                                        /*! вставить символ               */
				strings[carline].insert(strings[carline].begin() + carchar,(char)wParam);
			else
			{
                                        /*! установить соотв. флаг        */
				car_in_end = true;
                                        /*! добавить символ в конец строки*/
				strings[carline] += (char)wParam;
			}
                                        /*! установить флаг модификации   */
			modified = 1;
                                        /*! перейти на следующую позицию  */
			carchar++;
                                        /*! если страничный режим         */
			if (paged)
                                        /*! если строка вышла за границы с
										траницы */
				if (CharLength(carline,strings[carline].length()) > page_width)
				{
                                        /*! перенести строки              */
					CheckStrings(true);
                                        /*! если каретка была в конце     */
					if (car_in_end)
					{
                                        /*! перейти на новую строку       */
						carline++;
                                        /*! установить ввод на первый симв
										ол */
						carchar = 1;
					}
				}
                                        /*! скорректировать позицию ввода */
			DoBeforeCheckBound(hWnd);
                                        /*! обновить каретку              */
			PostMessage(hWnd,WM_USER,0,0);
                                        /*! скорректировать каретку       */
			DoAfterCheckBound(hWnd);
                                        /*! перерисовать                  */
			InvalidateRect(hWnd,NULL,true);
		}
		return 0;
                                        /*! обработка вертикального скрола*/
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEDOWN:
                                        /*! прирастить номер начальной стр
										оки вывода */
			SetLine(++page_line);
			break;
		case SB_LINEUP:
                                        /*! уменьшить номер начальной стро
										ки вывода */
			SetLine(--page_line);
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
                                        /*! установить номер начальной стр
										оки вывода в соответсвии с позицие
										й скроллбара */
			page_line = HIWORD(wParam);
			SetLine(page_line);
			break;
		case SB_PAGEUP:
                                        /*! уменьшить номер начальной стро
										ки вывода на кол-во строк в окне в
										ывода */
			page_line -= scr_lines;
			if (page_line < 0)
				page_line = 0;
			SetLine(page_line);
			break;
		case SB_PAGEDOWN:
                                        /*! увеличить номер начальной стро
										ки вывода на кол-во строк в окне в
										ывода */
			page_line += scr_lines;
			if (page_line > (int)strings.size() - scr_lines)
				page_line = strings.size() - scr_lines;
			SetLine(page_line);
			break;
		}
                                        /*! перерисовать                  */
		InvalidateRect(hWnd,NULL,TRUE);
                                        /*! обновить вид скроллбара       */
		UpdateVScrollInfo(hWnd);
                                        /*! обновить каретку              */
		PostMessage(hWnd,WM_USER,0,0);
		return 0;
                                        /*! обработка колесика мыши       */
	case WM_MOUSEWHEEL:
                                        /*! сдвиг вниз                    */
		if ((short)HIWORD(wParam) < 0)
		{
			if ((int)strings.size() > scr_lines)
				SetLine(++page_line);
			else
				SetLine(strings.size() - scr_lines);
		}
		else
                                        /*! сдвиг вверх                   */
			SetLine(--page_line);
                                        /*! перерисовать                  */
		InvalidateRect(hWnd,NULL,true);
                                        /*! обновить вид скроллбара       */
		UpdateVScrollInfo(hWnd);
                                        /*! обновить каретку              */
		PostMessage(hWnd,WM_USER,0,0);
		return 0;
                                        /*! обработка горизонтального скро
										ллбара */
	case WM_HSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
                                        /*! если ширина страницы меньше ши
										рины области вывода, не использова
										ть скроллирование */
			if (page_width < wwidth)
				break;
                                        /*! сдвинуть координату горизонтал
										ьного вывода влево на ср. ширину с
										ивола текущего шрифта */
			win_x -= tm.tmAveCharWidth;
			SetWinX(win_x);
			break;
		case SB_LINERIGHT:
			                            /*! если ширина страницы меньше ши
										рины области вывода, не использова
										ть скроллирование */
			if (page_width < wwidth)
				break;
			                            /*! сдвинуть координату горизонтал
										ьного вывода вправо на ср. ширину 
										символа текущего шрифта */
			win_x += tm.tmAveCharWidth;
			SetWinX(win_x);
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
                                        /*! установить координату горизон
										тального вывода в соответствии с 
										позицией скроллбара */
			win_x = HIWORD(wParam);
			SetWinX(win_x);
			break;
		case SB_PAGELEFT:
                                        /*! сдвинуть координату горизонтал
										ного вывода на ширину области выво
										да влево */
			win_x -= wwidth;
			if (win_x < 0)
				win_x = 0;
			SetWinX(win_x);
			break;
		case SB_PAGERIGHT:
			                            /*! сдвинуть координату горизонтал
										ного вывода на ширину области выво
										да вправо */
			win_x += wwidth;
			SetWinX(win_x);
			break;
		}
                                        /*! перерисовать                  */
		InvalidateRect(hWnd,NULL,TRUE);
                                        /*! обновить вид гор. скроллбара  */
		UpdateHScrollInfo(hWnd);
                                        /*! обновить каретку              */
		PostMessage(hWnd,WM_USER,0,0);
		return 0;
	case WM_SETFOCUS:
                                        /*! создание и отображение каретки*/
		CreateCaret(hWnd,(HBITMAP)NULL,CARETWIDTH,tm.tmHeight);
		ShowCaret(hWnd);
                                        /*! установить каретку            */
		PostMessage(hWnd,WM_USER,0,0);
		return 0;
	case WM_KILLFOCUS:
                                        /*! скрыть и уничтожить каретку   */
		HideCaret(hWnd);
		DestroyCaret();
		return 0;
                                        /*! для закраски фона             */
	case WM_ERASEBKGND:
		{
			RECT rc;
			HPEN hPen, hPenOld;
			HBRUSH hBrush, hBrushOld;

			HideCaret(hWnd);
			GetClientRect(hWnd,&rc);
                                        /*! создать кисть фонового цвета  */
			hBrush = CreateSolidBrush(background_color);
                                        /*! выбрать новую кисть, сохранив 
										старую */
			hBrushOld = (HBRUSH)SelectObject((HDC)wParam,hBrush);
                                        /*! создать перо фонового цвета   */
			hPen = CreatePen(PS_SOLID,0,background_color);
                                        /*! выбрать новое перо, сохранив с
										тарое */
			hPenOld = (HPEN)SelectObject((HDC)wParam,hPen);
                                        /*! нарисовать заполненный прямоуг
										ольник на всю область вывода */
			Rectangle((HDC)wParam,0,0,rc.right - rc.left,rc.bottom - rc.top);
                                        /*! восстановить кисть и перо по у
										молчанию, удалить созданные */
			SelectObject((HDC)wParam,hPenOld);
			DeleteObject(hPen);
			SelectObject((HDC)wParam,hBrushOld);
			DeleteObject(hBrush);
			ShowCaret(hWnd);
		}
		return 1;
	case WM_USER:
		{
                                        /*! пересчитать координаты каретки*/
			carx = CalcCarX(carline,carchar);
			cary = CalcCarY(carline,carchar);
                                        /*! установить ее позицию         */
			SetCaretPos(carx,cary);
		}
		return 0;
	default:
                                        /*! обработка сообщений по умолчан
										ию */
		return DefWindowProc(hWnd,messg,wParam,lParam);
	}
}


/*!
 * @function : WinMain
 * @brief    : точка входа в программу
 *
 * @param HINSTANCE hInstance     : хэндл загрузочного образа из ЕХЕ файла
 * @param HINSTANCE hPrevInstance : для совместимости с Win3.1 
 * @param LPSTR lpCmdLine         : строка запуска
 * @param int nShowCmd            : команда запуска
 *
 * @return int WINAPI : код выхода
 */

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
                              /*! структура сообщения                     */
	MSG msg;
                              /*! структура класса окна                   */
	WNDCLASSEX wc;

                                        /*! получить полный путь к ЕХЕ    */
	GetModuleFileName(NULL,path,MAX_STR);
                                        /*! сохранить хэндл загр. модуля  */
	hInst = hInstance;
                                        /*! установить функцию обработки о
										шибок */
	error = errore;
                                        /*! подготовка структуры класса ок
										на */
	wc.cbSize = 48;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.hbrBackground = NULL;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
                                        /*! загрузить иконку из ресурсов  */
	wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MYICON));
	wc.hIconSm = wc.hIcon;
	wc.hInstance = hInstance;
                                        /*! меню из ресурсов              */
	wc.lpszMenuName = "picomenu";
	wc.lpszClassName = CLASSNAME;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;

                                        /*! регистрируем класс окна       */
	if (!RegisterClassEx(&wc))
		error("Invalid class");

                                        /*! подготавливаем строку с заголо
										вком окна из имени программы и име
										ни файла по умолчанию */
	sprintf(windowname,"%s - %s",PROGRAMNAME,DEFFILENAME);

                                        /*! создаем окно                  */
	hWnd = CreateWindowEx(WINDOWEXSTYLE,CLASSNAME,windowname,
		WINDOWSTYLE,WINDOWPOSX,WINDOWPOSY,WINDOWWIDTH,WINDOWHEIGHT,
		NULL,NULL,hInstance,NULL);

                                        /*! проверяем на ошибку           */
	if (!hWnd)
		error("Invalid window");

                                        /*! установить функйию обработки о
										шибок с учетом созданного окна */
	error = errorwe;

                                        /*! очистить вектор строк         */
	strings.clear();
                                        /*! загрузить "горячие" клавиши   */
	hAccTable = LoadAccelerators(hInst,"picoaccels");
                                        /*! разобрать командную строку    */
	if (ProcessCmdLine(lpCmdLine,filename))
	{
		char fmesg[MAX_STR];

                                        /*! попробовать загрузить файл    */
		if (!LoadFile(filename))
		{
                                        /*! в случае ошибки - выводим ее  */
			sprintf(fmesg,"File %s not found!",filename);
			MessageBox(hWnd,fmesg,"Error",MB_OK | MB_ICONSTOP);
			sprintf(filename,"UNNAMED.txt");
		}
	}
	else
		sprintf(filename,"UNNAMED.txt");

                                        /*! цикл обработки сообщений      */
	while (GetMessage(&msg,NULL,0,0))
	{
		if (!TranslateAccelerator(hWnd,hAccTable,&msg))
			TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	hWnd = NULL;

                                        /*! освободить занятые ресурсы    */
	cleanup();

                                        /*! стандартный возврат           */
	return msg.wParam;
}


/*!
 * @function : DlgProc
 * @brief    : функция обработки сообщений диалога
 *
 * @param HWND hwndDlg  : хэндл окна диалога
 * @param UINT message  : сообщение
 * @param WPARAM wParam : первый параметр сообщения
 * @param LPARAM lParam : второй параметр сообщения
 *
 * @return BOOL APIENTRY : обработали?
 */

BOOL APIENTRY DlgProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
                                        /*! обработка нажатия на единствен
										ную кнопку */
		case IDB_OK:
			if (HIWORD(wParam) == BN_CLICKED)
			{
                                        /*! завершить диалог              */
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
 * @brief    : обработка командной строки
 *
 * @param LPSTR line     : командная строка 
 * @param char *filename : возвращает имя первого файла
 *
 * @return int : 1 - filename содержит имя переданного через ком. строку файла, 0 - файлов 
 *				через ком. строку передано не было
 */

int ProcessCmdLine(LPSTR line,char *filename)
{
                              /*! позиция в ком. строке начала текущего ар
							  гумента */
	int start = 0;
                              /*! кол-во аргументов                       */
	int count = 0;
                              /*! буфер для накопления символов текущего а
							  ргумента */
	char buff[MAX_STR];

                                        /*! очистить строки               */
	memset(buff,0,MAX_STR);
	memset(filename,0,MAX_STR);
                                        /*! цикл по сиволам ком. строки   */
	for (UINT i = 0; i < strlen(line) + 1; i++)
	{
                                        /*! если это разделитель, либо кон
										ец строки */
		if ((line[i] == ' ') || (line[i] == '\0'))
		{
                                        /*! если буфер пуст               */
			if (!buff[0])
			{
                                        /*! переместить указатель на начал
										о аргумента на 1 вправо */
				start++;
                                        /*! ищем дальше                   */
				continue;
			}
                                        /*! буфер не пуст                 */
                                        /*! если найден n >= 2 аргумент, з
										запустить его в новом экземпляре п
										риложения через командную строку */
			if (count)
			{
				char cline[MAX_STR];

				sprintf(cline,"%s %s",path,buff);
				WinExec(cline,SW_SHOW);
			}
			else
                                        /*! иначе записать в строку с имен
										первого файла */
				strcpy(filename,buff);
                                        /*! увеличить кол-во найденных арг
										ументов */
			count++;
                                        /*! очистить буфер                */
			memset(buff,0,MAX_STR);
                                        /*! сместить указтель на начало ар
										гумента на 1 влево */
			start = i + 1;
		}
		else
		{
                                        /*! добавить символ в буфер       */
			buff[i - start] = line[i];
			buff[i - start + 1] = '\0';
		}
	}
                                        /*! если что-то найдено           */
	if (count)
                                        /*! возвратить код успешного завер
										шения */
		return 1;
	else
                                        /*! иначе - неуспешного           */
		return 0;
}


/*!
 * @function : LoadFile
 * @brief    : загружает текст из файла в вектор строк
 *
 * @param char *filename : имя файла
 *
 * @return int : успешно (1) / неуспешно (0)
 */

int LoadFile(char *filename)
{
                              /*! хэндл файла, его инициализация          */
	HANDLE f = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,
			OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
                              /*! число прочитанных байт                  */
	ULONG read = 0;
                              /*! позиция в буфере начала текущей строки  */
	UINT pos = 0, 
                              /*! размер файла                            */
		 filesize = 0;
                              /*! буфер для хранения текста из файла      */
	char *ptext = NULL;

                                        /*! если файл не открылся         */
	if ((!f) || (f == INVALID_HANDLE_VALUE))
                                        /*! возврат ошибки                */
		return 0;
	else
	{
                                        /*! определяем размер файла       */
		filesize = GetFileSize(f,NULL);
                                        /*! очищаем вектор строк          */
		strings.clear();
		wrapped.clear();
                                        /*! выделить память под чтение из 
										файла информации */
		ptext = new char [filesize + 1];
                                        /*! очистить выделенное           */
		memset(ptext,0,filesize + 1);
                                        /*! считать в буфер файл          */
		ReadFile(f,ptext,filesize,&read,NULL);
                                        /*! цикл по буферу                */
		for (UINT i = 0; i <= filesize; i++)
		{
                                        /*! если перенос строки, или конец*/
			if ((ptext[i] == '\r') || (i == filesize))
			{
				UINT j;
				int ind = 0;
                              /*! для хранения кол-во символов табуляции в
							  текущей строке */
				int tabscount = 0;

                                        /*! подсчитать сиволы табуляции   */
				for (j = 0; j < i - pos; j++)
					if (ptext[j + pos] == '\t')
                        tabscount++;

                                        /*! выделить память под строку с у
										четом замены сиволов табуляции про
										белами */
				char *temp = new char [i - pos + 1 - tabscount + TAB_SIZE * tabscount];
                                        /*! очистить выделенное           */
				memset(temp,0,i - pos + 1 - tabscount + TAB_SIZE * tabscount);
                                        /*! цикд по строке                */
				for (j = 0; j < i - pos; j++)
				{
                                        /*! если встретили символ '\t'    */
					if (ptext[j + pos] == '\t')
					{
						                /*! заменить символ табуляции TAB_
										SIZE пробелами */
                        for (int k = 0; k < TAB_SIZE; k++)
						{
							temp[ind++] = ' ';
						}
					}
					else
					{
                                        /*! иначе просто скопировать из бу
										фера символ в строку */
						temp[ind++] = ptext[j + pos];
					}
				}

                                        /*! занесение подготовленной строк
										и в вектор строк */
				strings.push_back(string(temp));
                                        /*! переместить указатель начала с
										роки в буфере на новую позицию */
				pos = i + 1;
                                        /*! освободить память под строку  */
				delete [] temp;
			}
                                        /*! если для обозначения новой стр
										оки используется пара "\r\n", что 
										является стандартом для windows те
										кстовых файлов, то скоректировать 
										указатель на следуюшую строку на 1*/
			if (ptext[i] == '\n')
				pos++;
		}

                                        /*! если установлен страничный реж
										им, сразу перенести строки */
		if (paged)
            CheckStrings(true);

                                        /*! сброс флага модификации       */
		modified = 0;
                                        /*! сброс счетчика сохранений     */
		count_saves = 0;

                                        /*! обновление заголовка окна     */
		sprintf(windowname,"%s - %s",PROGRAMNAME,filename);
		SetWindowText(hWnd,windowname);

                                        /*! обновление вида скроллбаров   */
		UpdateHScrollInfo(hWnd);
		UpdateVScrollInfo(hWnd);
				
                                        /*! закрыть файл                  */
		CloseHandle(f); 
		f = NULL;
                                        /*! освободить память, занятую под
										буфер */
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
 * @brief    : получить ширину в пикселях первых num_chars символов строки
 *
 * @param const char *line  : строка
 * @param int num_chars     : кол-во символов
 *
 * @return int : ширина в пикселях
 */

int GetLineCharsLength(const char *line,int num_chars)
{
	char *temp = NULL;
	HideCaret(hWnd);
	HDC hdc = GetDC(hWnd);
	SIZE sz;
	
                                        /*! если кол-во символов > 0      */
	if (num_chars)
	{
                                        /*! если кол-во символов > длины п
										ереданной строки, скорректировать */
		if (num_chars > (int)strlen(line))
			num_chars = strlen(line);
                                        /*! создать строку с первыми num_c
										hars сиволами */
		temp = new char [num_chars + 1];
		memset(temp,0,num_chars + 1);
		strncpy(temp,line,num_chars);
                                        /*! посчитать ее ширину, используя
										параметры текущего шрифта */
		GetTextExtentPoint32(hdc,temp,strlen(temp),&sz);
		delete [] temp;
	}
	else
		sz.cx = 0;
	ReleaseDC(hWnd,hdc);
	ShowCaret(hWnd);
                                        /*! возвратить ширину             */
	return sz.cx;
}


/*!
 * @function : FindLargestString
 * @brief    : находит строку с максимальной длиной
 *
 * @param vector<string> &s : где искать
 *
 * @return int : индекс найденной строки в случае успеха, иначе -1.
 */

int FindLargestString(vector<string> &s)
{
                                        /*! ничего не делать, если строк н
										ет */
	if (s.empty())
		return -1;
	int len = s[0].length(), ind = 0, i;

                                        /*! поиск по строкам вектора макси
										мальной длины строки */
	for (i = 1; i < (int)s.size(); i++)
	{
		if ((int)s[i].length() > len)
		{
			len = s[i].length();
                                        /*! сохранить индекс              */
			ind = i;
		}
	}
                                        /*! возвратить индекс             */
	return ind;
}


/*!
 * @function : GetPageWidth
 * @brief    : возвращает ширину страницы, основываясь на текущем режиме
 *
 * @param none
 *
 * @return int : вычисленная ширина страницы 
 */

int GetPageWidth()
{
                                        /*! в страничном режиме просто воз
										вращаем константное значение */
	if (paged)
		return PAGEWIDTH;
                                        /*! получить индекс строки максима
										льной длины */
                                        /*! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
										так как шрифт не всегда моноширинн
										ый, данный способ может давать оши
										бку при определении ширины страниц
										ы, так как на самом деле необходим
										о искать строку не с максимальной 
										длиной в сиволах, а максимальной ш
										ириной в пикселях !!!!!!!!!!!!!!! */
	int imaxlength = FindLargestString(strings);
                                        /*! если найдено                  */
	if (imaxlength >= 0)
	{
                                        /*! вычислить ее ширину           */
		int L = GetLineCharsLength(strings[imaxlength].c_str(),
					strings[imaxlength].length());
                                        /*! в случае, если ширина строки б
										ольше ширины окна вывода */
		if (L > wwidth)
                                        /*! возвратить ширину строки      */
			return L;
		else
                                        /*! иначе - окна вывода           */
			return wwidth;
	}
	else
                                        /*! если строк нет, возвратить шир
										ину окна вывода */
		return wwidth;
}



/*!
 * @function : DoBeforeCheckBound
 * @brief    : делает проверки на выход за допустимые пределы координаты позиции ввода
 *
 * @param HWND hWnd : для какого окна делать проверку
 *
 * @return void : 
 */

void DoBeforeCheckBound(HWND hWnd)
{
                                        /*! если строк нет                */
	if (strings.empty())
	{
                                        /*! установить значения по умолч. */
		carline = 0;
		carchar = 0;
	}
	else
	{
                                        /*! проверки строки ввода         */
		if (carline < 0)
			carline = 0;
		if (carline > (int)strings.size() - 1)
			carline = strings.size() - 1;
                                        /*! проверка номера символа ввода */
		if (carchar > (int)strings[carline].length())
			carchar = strings[carline].length();
	}
}


/*!
 * @function : DoAfterCheckBound
 * @brief    : проверка допустимости вычисленных значений координат каретки
 *
 * @param HWND hWnd : для какого окна проверять
 *
 * @return void : 
 */

void DoAfterCheckBound(HWND hWnd)
{
                                        /*! проверка по вертикали         */
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
	                                        /*! проверка по горизотали        */
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
                                        /*! пересчитать параметры страницы*/
	SetPageWidth(GetPageWidth());
                                        /*! обновить каретку              */
	PostMessage(hWnd,WM_USER,0,0);
                                        /*! обновить скроллбары           */
	UpdateVScrollInfo(hWnd);
	UpdateHScrollInfo(hWnd);
}


/*!
 * @function : SaveFile         
 * @brief    : сохраняет текст в файл с заданным имененем                  
 *
 * @param *filename : имя файла для сохранения
 *
 * @return int  : 1 - успешно, 0 - ошибка
 */

int SaveFile(char *filename)
{
                              /*! хэндл файла, его инициализация          */
	HANDLE f = CreateFile(filename,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,
		NULL);
                              /*! символы новой строки                    */
	char crlf[] = "\r\n";
                              /*! кол-во записанных байт в ходе операции з
							  аписи */
	DWORD written;


                                        /*! если не открылся, выход       */
	if ((f == INVALID_HANDLE_VALUE) || (!f))
		return 0;

	if (paged)
        CheckStrings(false);
                                        /*! цикл по строкам               */
	for (UINT i = 0; i < strings.size(); i++)
	{
                                        /*! запись текущей строки         */
		WriteFile(f,strings[i].c_str(),strings[i].length(),&written,NULL);
                                        /*! если ошибка, выход            */
		if (written != strings[i].length())
		{
			CloseHandle(f);
			DeleteFile(filename);
			return 0;
		}
                                        /*! записать перенос строки       */
        WriteFile(f,crlf,2,&written,NULL);
                                        /*! если ошибка, выход            */
		if (written != 2)
		{
			CloseHandle(f);
			DeleteFile(filename);
			return 0;
		}
	}
                                        /*! закрыть файл                  */
	CloseHandle(f);
	if (paged)
        CheckStrings(true);
                                        /*! увеличить число сохранений    */
	count_saves++;
                                        /*! сбросить флаг модификации     */
	modified = 0;
                                        /*! обновить заголовок окна       */
	sprintf(windowname,"%s - %s",PROGRAMNAME,filename);
	SetWindowText(hWnd,windowname);
	return 1;
}

/*!
 * @function : UpdateVScrollInfo                 
 * @brief    : обновление вида вертикального скроллбара                     
 *
 * @param HWND hWnd : для какого окна обновлять
 *
 * @return void : 
 */

void UpdateVScrollInfo(HWND hWnd)
{
	SCROLLINFO si;

                                        /*! подготовка структуры скролла  */
    si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
                                        /*! единица скролла - одна строка */
	si.nMax = pages * page_lines - 1;
	si.nMin = 0;
	si.nPage = scr_lines;
	si.nPos = page_line;
                                        /*! установить вид скролла        */
	SetScrollInfo(hWnd,SB_VERT,&si,TRUE);
}

/*!
 * @function : UpdateHScrollInfo                 
 * @brief    : обновление вида горизонтального скроллбара
 *
 * @param HWND hWnd : для какого окна обновлять
 *
 * @return void : 
 */

void UpdateHScrollInfo(HWND hWnd)
{
	SCROLLINFO si;
                                        /*! если есть строки              */
	if (!strings.empty())
	{
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
                                        /*! единица скролла - пиксел      */
		si.nMax = page_width + 1;// + 1;
		si.nMin = 0;
		si.nPage = wwidth;// + 1;
		si.nPos = win_x;
                                        /*! устанавляиваем вид скролла    */
		SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
	}
	else
	{
                                        /*! обнуляем структуру скролла    */
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
		si.nMax = 0;
		si.nMin = 0;
		si.nPage = 0;
		si.nPos = 0;
                                        /*! устанавляиваем вид скролла    */
		SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
	}

}

/*!
 * @function : DrawPages
 * @brief    : производит рисование контуров страниц     
 *
 * @param hWnd : на каком окне рисовать
 * @param hdc : на каком контексте устройства
 * @param s_page : с какой страницы
 * @param l_page : по какую страницу
 * @param page_L : строк в странице
 *
 * @return void  : 
 */

void DrawPages(HWND hWnd,HDC hdc,int s_page,int l_page,int page_L)
{
	HideCaret(hWnd);

	RECT rc;
                              /*! хэндл кисти, инициализация фоновым цвето
							  м, установка для контекста утсройства */
	HBRUSH hBrush = CreateSolidBrush(background_color);
                              /*! хэндл старой кисти                      */
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc,hBrush);

                                        /*! получение размеров окна вывода*/
	GetClientRect(hWnd,&rc);

                                        /*! абсолютное смещение точки нача
										ла рисования по оси ординат */
	int y = - (page_L - s_page * page_lines)* tm.tmHeight;
                                        /*! рисуем первый контур          */
	Rectangle(hdc,-win_x,y,page_width - win_x,y + page_height);

                                        /*! цикл по страницам             */
	for (int page_i = s_page; page_i <= l_page + 1; page_i++)
	{
                                        /*! прирастить ординату           */
        y += page_height;        	
                                        /*! вывести следующий контур      */
		Rectangle(hdc,-win_x,y,page_width - win_x,y + page_height);
	}
	
                                        /*! восстановление старой кисти   */
	SelectObject(hdc,hBrushOld);
	DeleteObject(hBrush);
	
	ShowCaret(hWnd);
}

/*!
 * @function : SetPageWidth
 * @brief    : устанавливает ширину страницы
 *
 * @param w : новое значение ширины
 *
 * @return void  : 
 */

void SetPageWidth(int w)
{
                                        /*! установка ширины страницы     */
	page_width = w;
}

/*!
 * @function : SetPageHeight
 * @brief    : устанавливает высоту страницы
 *
 * @param h : новое значение высоты
 *
 * @return void  : 
 */

void SetPageHeight(int h)
{
                                        /*! обнуление параметров страницы */
	page_height = 0;
	pages = 0;
	page_lines = 0;
                                        /*! если передано ненулевое значен
										ие */
	if (h)
	{
                                        /*! пересчитать кол-во строк в стр
										анице */
		page_lines = h / tm.tmHeight;
                                        /*! установить высоту страницы    */
        page_height = h;
                                        /*! если не страничный режим      */
		if (!paged)
                                        /*! установить высоту, зависящую о
										т высоты шрифта */
			page_height = page_lines * tm.tmHeight;
                                        /*! рассчитать кол-во станиц      */
		if (page_lines)
            pages = strings.size() / page_lines + 1;
	}
}

/*!
 * @function : CharLength
 * @brief    : возвращает ширину в пикселях первых carc символов строки с индексом carl
 *
 * @param carl : индекс строки
 * @param carc : номер символа
 *
 * @return int  : ширина 
 */

int CharLength(int carl,int carc)
{
                                        /*! возвратить ширину первых саrс 
										символов строки по индексу */
	return GetLineCharsLength(strings[carl].c_str(),carc);
}

/*!
 * @function : SetWinX
 * @brief    : устанавливает смещение области вывода по горизонтали 
 *
 * @param winx : новое значение смещение
 *
 * @return void  : 
 */

void SetWinX(int winx)
{
                                        /*! установить смещение области вы
										вода по горизонтали */
	win_x = winx;
	if (win_x < 0)
		win_x = 0;
}

/*!
 * @function : SetLine
 * @brief    : устанавливает начальную строку вывода
 *
 * @param lin : новое значение строки 
 *
 * @return void  : 
 */

void SetLine(int lin)
{
                                        /*! установить начальную строку вы
										вода */
	page_line = lin;
	if (page_line < 0)
		page_line = 0;	
}

/*!
 * @function : OutLine
 * @brief    : выводит строку в заданных координатах
 *
 * @param hdc : контекст устройства вывода
 * @param line : индекс строки для вывода
 * @param x : абсцисса
 * @param y : ордината
 *
 * @return void  : 
 */

void OutLine(HDC hdc, int line,int x,int y)
{
                                        /*! проверка границ               */
	if ((line >= (int)strings.size()) || (line < 0))
	{
		error("Line invalid");
		return;
	}

                                        /*! вывести строку                */
	TextOut(hdc,x,y,strings[line].c_str(),strings[line].length());
}

/*!
 * @function : CalcCarX
 * @brief    : рассчитать абсциссу каретки
 *
 * @param carline : номер строки каретки
 * @param carchar : номер символа каретки
 *
 * @return int  : рассчитанное значение
 */

int CalcCarX(int carline, int carchar)
{
	int add = 0;
	int str_len = 0;
	int carx = 0;

                                        /*! если есть строки              */
	if (!strings.empty())
	{
                                        /*! получить ширину строки        */
        str_len = CharLength(carline,strings[carline].length());
    
                                        /*! в зависимости от выравнивания 
										вычислить коррекцию по горизонтали*/
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

                                        /*! рассчитать абсциссу каретки   */
		carx = add - win_x + CharLength(carline,carchar);
	}
	else
        carx = add - win_x;
	return carx;
}

/*!
 * @function : CalcCarY
 * @brief    : рассчитывает ординату каретки
 *
 * @param carline : номер строки каретки
 * @param carchar : номер символа каретки
 *
 * @return int  : рассчитанное значение
 */

int CalcCarY(int carline, int carchar)
{
	int cary;

                                        /*! пересчитать параметры страницы*/
	SetPageHeight(page_height);
                                        /*! получить номер текущей страниц
										ы */
	if (page_lines)
		page = GETPAGE(carline,page_lines);
	if (page_lines)
	{
		int pline = carline % page_lines;
		int s_page = GETPAGE(page_line,page_lines);
		int curr_lines = page_lines;

		if (curr_lines + page * page_lines >= (int)strings.size())
			curr_lines = strings.size() - page * page_lines;

                                        /*! если страничный режим         */
		if (paged)
                                        /*! использовать вертик. выравнив.*/
			switch (valign)  
			{ 
                                        /*! рассчет в зависимости от вырав
										нивание */
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
                                        /*! рассчитать ординату каретки не
										используя выравнивание */
			cary = page * page_height + (pline - page_line) * tm.tmHeight;

    }
	else
                                        /*! значение по умолчанию         */
		cary = page * page_height - page_line * tm.tmHeight;
	return cary;
}

/*!
 * @function : CheckStrings
 * @brief    : переносит строки
 *
 * @param wrap : выполнять реальный перенос, а не фиктивный
 *
 * @return void  : 
 */

void CheckStrings(bool wrap)
{
	string app;
	bool found = false;
	int i;

                                        /*! если нет строк, выйти         */
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
	                                    /*! цикл по строкам от from_line  */
	for (i = 0/*from_line*/; i < (int)strings.size(); i++)
	{
                                        /*! проверить выход за страницу   */
		if (CharLength(i,strings[i].length()) > page_width)
		{
			wrapped.push_back(i);
                                        /*! производим перенос строки     */
			found = true;
			app = "";
                                        /*! пока строка не влезла в страни
										цу, переносить по 1-му символу */
			while (CharLength(i,strings[i].length()) > page_width)
			{
				app.insert(0,strings[i],strings[i].length() - 1,1);
				strings[i].erase(strings[i].length() - 1,1);
			}
                                        /*! если не последняя строка      */
			if (strings.size() != i + 1)
                                        /*! вставить в середину           */
				strings.insert(strings.begin() + i + 1,app);
				//strings[i + 1].insert(0,app);
			else
                                        /*! иначе в конец                 */
				strings.push_back(app);
                                        /*! установить флаг модификации   */
		}
	}
}


/*!
 * @function : StringWrapped
 * @brief    : определяет, является ли строка перенесенной
 *
 * @param int carline : номер строки
 *
 * @return bool : да / нет
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
 * @brief    : удаляет метку "перенесена" со строки
 *
 * @param int carline : номер строки
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
 * @brief    : сдвигает метки строк на delta
 *
 * @param int from_i  : откуда начинать сдвиг
 * @param int delta   : на сколько сдвигать
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
 * @brief    : возвращает полную объединенную строку (неперенесенную)
 *
 * @param int carline : номер строки
 *
 * @return string : объединенная строка
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
 * @brief    : помечает строку как перенесенную
 *
 * @param int carline : номер строки
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
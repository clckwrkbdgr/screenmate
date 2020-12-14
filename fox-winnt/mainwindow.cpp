#include "mainwindow.h"
#pragma warning(push, 0)
#include <iostream>
#pragma warning(pop)

MainWindow * MainWindow::last_instance = nullptr;

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   MainWindow & wnd = *(MainWindow::instance());
   switch(msg)
   {
      case WM_CLOSE:
         DestroyWindow(hwnd);
         break;
      case WM_DESTROY:
         PostQuitMessage(0);
         break;
      case WM_PAINT:
         wnd.paint();
         break;
      case WM_KEYDOWN:
         if(wParam == VK_ESCAPE) {
            PostQuitMessage(0);
         }
         break;
      case WM_MOVING:
         if(wnd.character) {
            wnd.character->captured();
         }
         break;
      case WM_NCHITTEST: {
         LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
         if(hit == HTCLIENT) {
            hit = HTCAPTION;
         }
         return hit;
      }
      case WM_TIMER:
         if(!SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE))
         {
            throw WinAPIException("Failed to stay on top!");
         }
         if(wnd.character) {
            wnd.character->tick();
            if(wnd.window_movement.x || wnd.window_movement.y) {
               wnd.shift_window(wnd.window_movement);
            }
            InvalidateRect(hwnd, NULL, FALSE);
         }
         break;
      case WM_COPYDATA: {
         PCOPYDATASTRUCT copydata = (PCOPYDATASTRUCT)lParam;
         std::string message((char*)copydata->lpData, (char*)copydata->lpData + copydata->cbData);
         if(wnd.character) {
            wnd.character->send_command(message);
         }
         return TRUE;
      }
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return 0;
}

MainWindow::MainWindow(HINSTANCE instance, const Point & initial_pos, const Size & initial_size)
   : hInstance(instance), wc(), hwnd(nullptr), spritesheet(nullptr),
   window_pos(initial_pos), window_size(initial_size), tile_size(window_size),
   current_tile(0, 0), tile_mirrored(false), window_movement(0, 0),
   character(nullptr), transparent(RGB(0, 0, 0)), current_wide(false)
{
   MainWindow::last_instance = this;
}

MainWindow * MainWindow::instance()
{
   return last_instance;
}

Rect MainWindow::client_rect() const
{
   Size actual_size = window_size;
   if(current_wide) {
      actual_size.cx *= 2;
   }
   return Rect(Point(0, 0), actual_size);
}

Rect MainWindow::tile_rect(const Point & tile_pos) const
{
   Size actual_size = tile_size;
   if(current_wide)
   {
      actual_size.cx *= 2;
   }
   return Rect(
         Point(
            actual_size.width() * tile_pos.x,
            actual_size.height() * tile_pos.y
            ),
         actual_size
         );
}

void MainWindow::register_class(const std::string & window_class_name)
{
   window_class.assign(window_class_name.begin(), window_class_name.end());
   window_class.push_back('\0');
   wc.cbSize        = sizeof(WNDCLASSEX);
   wc.style         = WS_EX_TOPMOST;
   wc.lpfnWndProc   = MainWindow::WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = &(window_class.front());
   wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

   if(!RegisterClassEx(&wc)) {
      throw WinAPIException("Window Registration Failed!");
   }
}

void MainWindow::create(int nCmdShow, const std::string & caption, int frame_delay, const COLORREF & transparent_color)
{
   transparent = transparent_color;

   hwnd = CreateWindowEx(
         0,
         wc.lpszClassName,
         caption.c_str(),
         WS_OVERLAPPEDWINDOW,
         window_pos.x, window_pos.y, window_size.width(), window_size.height(),
         NULL, NULL, hInstance, NULL
         );

   if(!hwnd) {
      throw WinAPIException("Window Creation Failed!");
   }

   SetWindowLong(hwnd, GWL_STYLE, 0);
   // Set transparency by color.
   auto exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
   exstyle |= WS_EX_LAYERED; // Transparency.
   exstyle &=~ WS_EX_APPWINDOW; // No decorations.
   exstyle |= WS_EX_TOOLWINDOW; // No taskbar button.
   SetWindowLong(hwnd, GWL_EXSTYLE, exstyle);
   SetLayeredWindowAttributes(hwnd, transparent, 0, LWA_COLORKEY);

   bring_to_top(true);
   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   SetTimer(hwnd, /*id=*/1, frame_delay, nullptr);
}

void MainWindow::load_resources(const std::string & filename, const Size & single_tile_size)
{
   spritesheet = (HBITMAP)LoadImage(nullptr, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   if(!spritesheet) {
      throw WinAPIException("Failed to load " + filename);
   }
   tile_size = single_tile_size;
}

void MainWindow::load_resources(WORD resource_id, const Size & single_tile_size)
{
   spritesheet = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(resource_id));
   if(!spritesheet) {
      throw WinAPIException("Failed to load resource #" + std::to_string(resource_id));
   }
   tile_size = single_tile_size;
}

void MainWindow::paint()
{
   Painter painter(hwnd);
   Texture texture(painter.dc(), spritesheet);
   Rect actual_rect;
   GetClientRect(hwnd, &actual_rect);
   painter.fill(actual_rect, transparent);

   auto rect = tile_rect(current_tile);
   if(tile_mirrored) {
      rect.flip_hor();
      --rect.left;
      --rect.right;
   }
   painter.blt(texture, client_rect(), rect);
}

void MainWindow::shift_window(const Point & shift)
{
   Rect rect;
   GetWindowRect(hwnd, &rect);
   rect.left += shift.x;
   rect.right += shift.x;
   rect.bottom += shift.y;
   rect.top += shift.y;
   MoveWindow(hwnd, rect.x(), rect.y(), rect.width(), rect.height(), FALSE);

   Rect desktop;
   GetWindowRect(GetDesktopWindow(), &desktop);
   if(rect.right < desktop.left || desktop.right < rect.left) {
      character->out_of_screen();
   }
}

void MainWindow::set_current_tile(const Point & pos)
{
   current_tile = pos;
}

int MainWindow::run_event_loop()
{
   MSG Msg;
   while(GetMessage(&Msg, NULL, 0, 0) > 0)
   {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }
   return static_cast<int>(Msg.wParam);
}

void MainWindow::bring_to_top(bool activate)
{
   UINT flags = SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE;
   if(!activate)
   {
      flags |= SWP_NOACTIVATE;
   }
   SetActiveWindow(hwnd);
   SetForegroundWindow(hwnd);
   if(!SetWindowPos(hwnd, HWND_TOPMOST, window_pos.x, window_pos.y, window_size.width(), window_size.height(), flags))
   {
      throw WinAPIException("Failed to bring window to top.");
   }
   SetActiveWindow(hwnd);
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::set_character(Character * a_character)
{
   character = a_character;
   character->set_window(this);
}

void MainWindow::set_tile_mirrored(bool mirrored)
{
   tile_mirrored = mirrored;
}

void MainWindow::set_window_movement(const Point & advance)
{
   window_movement = advance;
}

void MainWindow::set_window_pos(const Point & pos)
{
   SetWindowPos(hwnd, 0, pos.x, pos.y, window_size.width() * 2, window_size.height(), SWP_NOACTIVATE);
}

void MainWindow::set_wide(bool wide)
{
   if(current_wide == wide) {
      return;
   }
   current_wide = wide;

   Rect window_rect;
   GetWindowRect(hwnd, &window_rect);
   Point pos(window_rect.x(), window_rect.y());
   if(wide) {
      SetWindowPos(hwnd, 0, pos.x - window_size.width() / 2, pos.y, window_size.width() * 2, window_size.height(), SWP_NOACTIVATE);
   } else {
      SetWindowPos(hwnd, 0, pos.x + window_size.width() / 2, pos.y, window_size.width() * 2, window_size.height(), SWP_NOACTIVATE);
   }
}

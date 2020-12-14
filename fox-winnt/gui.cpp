#include "gui.h"
#pragma warning(push, 0)
#include <sstream>
#include <iostream>
#pragma warning(pop)

// class Console

FILE *g_ic_file_cout_stream = nullptr;
FILE *g_ic_file_cerr_stream = nullptr;
FILE *g_ic_file_cin_stream = nullptr;

Console::Console()
{
    if(!AllocConsole())
    {
       throw WinAPIException("Failed to alloc console!");
    }
    if(freopen_s(&g_ic_file_cout_stream, "CONOUT$", "w", stdout) != 0)
    {
       throw WinAPIException("Failed to init stdout!");
    }
    if(freopen_s(&g_ic_file_cerr_stream, "CONOUT$", "w", stderr) != 0)
    {
       throw WinAPIException("Failed to init stderr!");
    }
    if(freopen_s(&g_ic_file_cin_stream, "CONIN$", "w+", stdin) != 0)
    {
       throw WinAPIException("Failed to init stdin!");
    }
}

// class WinAPIException : public std::exception

WinAPIException::WinAPIException(const std::string & message)
   : msg(message)
{
}

const char * WinAPIException::what() const
{
   return msg.c_str();
}

void WinAPIException::ShowDialog() const
{
   return ShowDialog(msg);
}

std::string WinAPIException::FormatErrorMessage(const std::string & error_message)
{
   std::ostringstream error;
   error << error_message << '\n';
   DWORD err_code = GetLastError();
   error << "Error: " << err_code << '\n';
   LPSTR error_explanation = nullptr;
   size_t size = FormatMessageA(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, err_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&error_explanation, 0, nullptr
         );
   error << std::string(error_explanation, size);
   return error.str();
}

std::string WinAPIException::FormatErrorMessage() const
{
   return FormatErrorMessage(msg);
}

void WinAPIException::ShowDialog(const std::string & error_message)
{
   std::string message = FormatErrorMessage(error_message);
   std::cerr << message << std::endl;
   MessageBox(NULL, message.c_str(), "Error!", MB_ICONEXCLAMATION | MB_OK);
}

////////////////////////////////////////////////////////////////////////////////

void ShowLastErrorDialog(const std::string & error_message)
{
   WinAPIException::ShowDialog(error_message);
}

// class Point:: : public POINT
Point::Point(LONG _x, LONG _y)
{
   x = _x;
   y = _y;
}

// class Size : public SIZE
Size::Size(LONG w, LONG h)
{
   cx = w;
   cy = h;
}

const LONG & Size::width() const
{
   return cx;
}

const LONG & Size::height() const
{
   return cy;
}

Size operator*(const Size & size, int factor)
{
   return Size(size.width() * factor, size.height() * factor);
}

Size operator*(int factor, const Size & size)
{
   return size * factor;
}

// class Rect : public RECT

Rect::Rect()
{
   memset(this, 0, sizeof(RECT));
}

Rect::Rect(RECT & other)
{
   memcpy(this, &other, sizeof(other));
}

Rect::Rect(const Point & topleft, const Size & size)
{
   left = topleft.x;
   top = topleft.y;
   right = topleft.x + size.width();
   bottom = topleft.y + size.height();
}

LONG Rect::x() const
{
   return left;
}

LONG Rect::y() const
{
   return top;
}

LONG Rect::width() const
{
   return right - left;
}

LONG Rect::height() const
{
   return bottom - top;
}

void Rect::flip_hor()
{
   std::swap(left, right);
}

// class Brush

Brush::Brush(const COLORREF & solid_color)
   : h(CreateSolidBrush(solid_color))
{
}

const HBRUSH & Brush::handle() const
{
   return h;
}

Brush::~Brush()
{
   DeleteObject(h);
}

// class Texture

Texture::Texture(HDC parent, HBITMAP image)
{
   hdc = CreateCompatibleDC(parent);
   old_bitmap = SelectObject(hdc, image);
   GetObject(image, sizeof(bitmap), &bitmap);
}

HDC Texture::dc() const
{
   return hdc;
}

Size Texture::size() const
{
   return Size(bitmap.bmWidth, bitmap.bmHeight);
}

Texture::~Texture()
{
   SelectObject(hdc, old_bitmap);
   DeleteDC(hdc);
}

// class Painter

Painter::Painter(HWND parent_hwnd)
   : hwnd(parent_hwnd)
{
   hdc = BeginPaint(hwnd, &ps);
}

const HDC & Painter::dc() const
{
   return hdc;
}

void Painter::fill(const Rect & rect, const Brush & brush)
{
   FillRect(hdc, &rect, brush.handle());
}

void Painter::blt(HDC texture, const Rect & dest, const Rect & src, const int alpha)
{
   if(alpha < 0) {
      StretchBlt(
            hdc, dest.x(), dest.y(), dest.width(), dest.height(),
            texture, src.x(), src.y(), src.width(), src.height(),
            SRCCOPY
            );
   } else {
      BYTE real_alpha = alpha > 255 ? 255 : (BYTE)alpha;
      BLENDFUNCTION blend = {
         /* BlendOps */   AC_SRC_OVER,
         /* BlendFlags */ 0,
         /* Alpha */  real_alpha,
         /* Format */ AC_SRC_ALPHA
      };
      if(!AlphaBlend(
            hdc, dest.x(), dest.y(), dest.width(), dest.height(),
            texture, src.x(), src.y(), src.width(), src.height(),
            blend
            ))
      {
         throw WinAPIException("AlphaBlend failed!");
      }
   }
}

void Painter::blt(const Texture & texture, const Rect & dest, const Rect & src, const int alpha)
{
   blt(texture.dc(), dest, src, alpha);
}

Painter::~Painter()
{
   EndPaint(hwnd, &ps);
}

// class Region

Region::Region(const Point & region_topleft, const Size & region_size)
   : rgn(nullptr)
{
   rgn = CreateRectRgn(region_topleft.x, region_topleft.y,
         region_topleft.x + region_size.width(),
         region_topleft.y + region_size.height()
         );
}

Region::~Region()
{
   DeleteObject(rgn);
}

Region & Region::operator+=(const Region & other)
{
   CombineRgn(rgn, rgn, other.rgn, RGN_OR);
   return *this;
}

const HRGN & Region::handle() const
{
   return rgn;
}

// struct LLKeyboardMessage

LLKeyboardMessage::LLKeyboardMessage(WPARAM wparam)
   : param(wparam)
{
}

std::string LLKeyboardMessage::str() const
{
   switch(param) {
      case WM_KEYDOWN: return "WM_KEYDOWN";
      case WM_SYSKEYDOWN: return "WM_SYSKEYDOWN";
      case WM_KEYUP: return "WM_KEYUP";
      case WM_SYSKEYUP: return "WM_SYSKEYUP";
   }
   return "<unknown>";
}

// struct VKCode

std::string scancode_to_str(UINT scancode)
{
   char buffer[1024] = {0};
   if(!GetKeyNameText(scancode << 16, buffer, sizeof(buffer) - 1)) {
      switch(scancode) {
         case VK_LWIN: return "Left Win";
         case VK_RWIN: return "Right Win";
         default: break;
      }
      throw WinAPIException("Failed to get name of the key");
   }
   return buffer;
}

VKCode::VKCode(LONG vk_code)
   : code(vk_code)
{
}

std::string VKCode::str() const
{
   UINT scancode = MapVirtualKey(code, MAPVK_VK_TO_VSC);
   return scancode_to_str(scancode);
}

// struct ScanCode

ScanCode::ScanCode(LONG scan_code)
   : code(scan_code)
{
}

std::string ScanCode::str() const
{
   return scancode_to_str(code);
}

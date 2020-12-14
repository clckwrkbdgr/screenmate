#pragma once
#pragma warning(push, 0)
#include <windows.h>
#include <exception>
#include <string>
#pragma warning(pop)

class Console
{
public:
   Console();
};

class WinAPIException : public std::exception
{
public:
   WinAPIException(const std::string & message);
   const char * what() const;
   std::string FormatErrorMessage() const;
   void ShowDialog() const;
   static std::string FormatErrorMessage(const std::string & error_message);
   static void ShowDialog(const std::string & error_message);
private:
   std::string msg;
};

/** deprecated: use WinAPIException::ShowDialog() instead. */
void ShowLastErrorDialog(const std::string & error_message);

class Point : public POINT
{
public:
   Point(LONG _x, LONG _y);
};

class Size : public SIZE
{
public:
   Size(LONG w, LONG h);
   const LONG & width() const;
   const LONG & height() const;
};
Size operator*(const Size & size, int factor);
Size operator*(int factor, const Size & size);

class Rect : public RECT
{
public:
   Rect();
   Rect(RECT&);
   Rect(const Point & topleft, const Size & size);
   LONG x() const;
   LONG y() const;
   LONG width() const;
   LONG height() const;
   void flip_hor();
};

class Brush
{
public:
   Brush(const COLORREF & solid_color);
   const HBRUSH & handle() const;
   ~Brush();
private:
   HBRUSH h;
};

class Texture
{
public:
   Texture(HDC parent, HBITMAP image);
   HDC dc() const;
   Size size() const;
   ~Texture();
private:
   HDC hdc;
   HGDIOBJ old_bitmap;
   BITMAP bitmap;
};

class Painter
{
public:
   Painter(HWND parent_hwnd);
   const HDC & dc() const;
   void fill(const Rect & rect, const Brush & brush);
   void blt(HDC texture, const Rect & dest, const Rect & src, const int alpha = -1);
   void blt(const Texture & texture, const Rect & dest, const Rect & src, const int alpha = -1);
   ~Painter();
private:
   HWND hwnd;
   HDC hdc;
   PAINTSTRUCT ps;
};

class Region
{
public:
   Region(const Point & region_topleft, const Size & region_size);
   ~Region();
   Region & operator+=(const Region & other);
   const HRGN & handle() const;
private:
   HRGN rgn;
};

struct LLKeyboardMessage
{
   LLKeyboardMessage(WPARAM wparam);
   std::string str() const;
private:
   WPARAM param;
};

struct VKCode
{
   VKCode(LONG vk_code);
   std::string str() const;
private:
   LONG code;
};

struct ScanCode
{
   ScanCode(LONG scan_code);
   std::string str() const;
private:
   LONG code;
};

template<typename T>
struct has_str_method
{
private:
	template<class C> static std::true_type  test(decltype(&C::str));
	template<class C> static std::false_type test(...);
public:
	static const bool value = std::is_same<std::true_type, decltype(test<T>(nullptr))>::value;
};

namespace std {
   template<class T>
   typename std::enable_if<has_str_method<T>::value, std::string>::type to_string(const T & value)
   {
      return value.str();
   }
}

template<class T>
typename std::enable_if<has_str_method<T>::value, std::ostream>::type & operator<<(std::ostream & out, const T & value)
{
   return out << value.str();
}

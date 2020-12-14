#pragma once
#include "character.h"
#include "gui.h"
#pragma warning(push, 0)
#pragma warning(pop)
#include <vector>
#include <string>

class MainWindow : public Performer
{
public:
   MainWindow(HINSTANCE hInstance, const Point & initial_pos, const Size & initial_size);
   void register_class(const std::string & window_class_name);
   void create(int nCmdShow, const std::string & caption, int frame_delay_ms, const COLORREF & transparent_color);
   void load_resources(const std::string & filename, const Size & single_tile_size);
   void load_resources(WORD resource_id, const Size & single_tile_size);
   void paint();
   void shift_window(const Point & shift);
   int run_event_loop();
   void bring_to_top(bool activate = false);

   Rect client_rect() const;
   Rect tile_rect(const Point & tile_pos) const;

   static MainWindow * instance();

   // Performer API.
   void set_character(Character * character);

   virtual void set_current_tile(const Point & pos);
   virtual void set_tile_mirrored(bool mirrored);
   virtual void set_window_movement(const Point & advance);
   virtual void set_window_pos(const Point & pos);
   virtual void set_wide(bool);
private:
   HINSTANCE hInstance;
   std::vector<char> window_class;
   WNDCLASSEX wc;
   HWND hwnd;
   HBITMAP spritesheet;
   Point window_pos;
   Size window_size;
   Size tile_size;
   Point current_tile;
   bool tile_mirrored;
   Point window_movement;
   Character * character;
   COLORREF transparent;
   bool current_wide;

   static MainWindow * last_instance;
   static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

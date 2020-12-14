#pragma once
#include "gui.h"
#include <queue>

class Performer
{
public:
   virtual ~Performer();

   virtual void set_current_tile(const Point & pos) = 0;
   virtual void set_tile_mirrored(bool mirrored) = 0;
   virtual void set_window_movement(const Point & advance) = 0;
   virtual void set_window_pos(const Point & pos) = 0;
   virtual void set_wide(bool) = 0;

   void set_current_tile(int x, int y);
   void set_window_movement(int shift_x, int shift_y);
   void set_window_pos(int x, int y);
};

struct Action {
   Point sprite;
   Point movement;
   bool mirrored;
   template<class Row, class Column>
   Action(Row sprite_row, Column sprite_column, const Point & action_movement = Point(0, 0), bool sprite_mirrored = false)
      : sprite(static_cast<int>(sprite_column), static_cast<int>(sprite_row)), movement(action_movement), mirrored(sprite_mirrored)
   {
   }
   Action(int sprite_row, int sprite_column, const Point & action_movement = Point(0, 0), bool sprite_mirrored = false);
};

class Actions
{
public:
   bool done() const;
   Actions & push(const Action & action);
   Actions & again(size_t times = 1);
   Actions & another_column(int new_column);
   Action pop();
   Actions & reset();
private:
   std::queue<Action> actions;
};

class Character
{
public:
   virtual ~Character();
   void set_window(Performer * main_window);
   Performer * performer();

   virtual void tick() = 0;
   virtual void out_of_screen() = 0;
   virtual void captured() = 0;
   virtual void send_command(const std::string & command) = 0;
private:
   Performer * window;
};

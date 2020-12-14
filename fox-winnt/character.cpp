#include "character.h"

Performer::~Performer()
{
}

void Performer::set_current_tile(int x, int y)
{
   set_current_tile(Point(x, y));
}

void Performer::set_window_movement(int shift_x, int shift_y)
{
   set_window_movement(Point(shift_x, shift_y));
}

void Performer::set_window_pos(int x, int y)
{
   set_window_pos(Point(x, y));
}

////////////////////////////////////////////////////////////////////////////////

Action::Action(int sprite_row, int sprite_column, const Point & action_movement, bool sprite_mirrored)
   : sprite(sprite_column, sprite_row), movement(action_movement), mirrored(sprite_mirrored)
{
}

bool Actions::done() const
{
   return actions.empty();
}

Actions & Actions::push(const Action & action)
{
   actions.push(action);
   return *this;
}

Actions & Actions::again(size_t times)
{
   if(times == 0)
   {
      times = 1;
   }
   while(times --> 0) {
      actions.push(actions.back());
   }
   return *this;
}

Actions & Actions::another_column(int new_column)
{
   actions.push(actions.back());
   actions.back().sprite.x = new_column;
   return *this;
}

Action Actions::pop()
{
   Action action = actions.front();
   actions.pop();
   return action;
}

Actions & Actions::reset()
{
   std::queue<::Action>().swap(actions);
   return *this;
}

////////////////////////////////////////////////////////////////////////////////

Character::~Character()
{
}

Performer * Character::performer()
{
   return window;
}

void Character::set_window(Performer * main_window)
{
   window = main_window;
}

#include "fox_resource.h"
#include "mainwindow.h"
#pragma warning(push, 0)
#include <iostream>
#include <map>
#include <list>
#include <ctime>
#pragma warning(pop)

typedef std::map<int, int> IntervalMap;

int interval_index(const IntervalMap & intervals, int value)
{
   for(auto interval : intervals)
   {
      auto index = interval.first;
      auto length = interval.second;
      if(value < length)
      {
         return index;
      }
      value -= length;
   }
   return -1;
}

class Fox : public Character
{
public:
   enum class State {
      SITTING,
      MOVING_LEFT,
      MOVING_RIGHT,
      RUNNING_LEFT,
      RUNNING_RIGHT,
      COUNT
   };
   static const IntervalMap & state_probs()
   {
      static IntervalMap map;
      if(map.empty()) {
         map[int(State::SITTING)] = 30;
         map[int(State::MOVING_RIGHT)] = 25;
         map[int(State::MOVING_LEFT)] = 25;
         map[int(State::RUNNING_RIGHT)] = 10;
         map[int(State::RUNNING_LEFT)] = 10;
      }
      return map;
   }

   enum class SpriteRow {
      SITTING = 0,
      MOVING = 1,
      STANDING = 2,
      RUNNING_READY = 3,
      RUNNING_UP = 4,
      RUNNING_DOWN = 5,
   };

   enum {
      MOVEMENT_SPEED = 4,
      RUNNING_SPEED = 12,
      WINDOW_WIDTH = 64,
      WINDOW_HEIGHT = 64,
   };

   Fox()
      : immediate_burst(false)
   {
      just_sit(true, 0);
   }
   virtual ~Fox() {}
   virtual void tick()
   {
      Action action = actions.pop();
      performer()->set_tile_mirrored(action.mirrored);
      performer()->set_window_movement(action.movement);
      performer()->set_current_tile(action.sprite);
      performer()->set_wide(action.sprite.y >= static_cast<int>(SpriteRow::RUNNING_READY));
      if(immediate_burst) {
         immediate_burst = false;
         actions.reset();
         if(action.movement.x < 0) {
            start_running(30, action, false);
         } else if(action.movement.x > 0) {
            start_running(30, action, true);
         } else {
            start_running(30, action, (rand() % 2) > 0);
         }
         return;
      }
      if(actions.done()) {
         int prob = rand() % 100;
         State new_state = State(interval_index(state_probs(), prob));
         switch(new_state) {
            case State::MOVING_RIGHT:
               start_moving(4, action, true);
               break;
            case State::MOVING_LEFT:
               start_moving(4, action, false);
               break;
            case State::RUNNING_RIGHT:
               start_running(8, action, true);
               break;
            case State::RUNNING_LEFT:
               start_running(8, action, false);
               break;
            default:
            case State::SITTING: {
               start_sitting(4, action);
               break;
            }
         }
      }
   }
   virtual void out_of_screen()
   {
      Rect desktop;
      GetWindowRect(GetDesktopWindow(), &desktop);

      if(rand() % 2) {
         performer()->set_window_pos(-WINDOW_WIDTH + 2, rand() % (desktop.height() - WINDOW_HEIGHT));
         actions.reset();
         run(true);
         run(true);
         run(true);
         run(true);
      } else {
         performer()->set_window_pos(desktop.width() - 2, rand() % (desktop.height() - WINDOW_HEIGHT));
         actions.reset();
         run(false);
         run(false);
         run(false);
         run(false);
      }
   }
   virtual void captured()
   {
      actions.reset();
      just_sit(true, 0);
   }
   virtual void send_command(const std::string & command)
   {
      if(command == "burst") {
         immediate_burst = true;
      } else {
         std::string error = "Unknown command: <" + command + ">";
         MessageBox(NULL, error.c_str(), "Unknown command", MB_ICONEXCLAMATION | MB_OK);
      }
   }
private:
   Actions actions;
   bool immediate_burst;

   void sit_down(bool facing_left)
   {
      actions.push(Action(SpriteRow::STANDING, 3, Point(0, 0), !facing_left));
      actions.push(Action(SpriteRow::STANDING, 2, Point(0, 0), !facing_left));
      actions.push(Action(SpriteRow::STANDING, 1, Point(0, 0), !facing_left));
      actions.push(Action(SpriteRow::STANDING, 0, Point(0, 0), !facing_left));
   }

   void calm_down(bool facing_left)
   {
      actions.push(Action(SpriteRow::RUNNING_READY, 1, Point(0, 0), !facing_left));
      actions.push(Action(SpriteRow::RUNNING_READY, 0, Point(0, 0), !facing_left));
   }

   void burst(bool facing_left)
   {
      actions.push(Action(SpriteRow::RUNNING_READY, 0, Point(0, 0), !facing_left));
      actions.push(Action(SpriteRow::RUNNING_READY, 1, Point(0, 0), !facing_left));
   }

   void get_up(bool facing_left)
   {
      actions.push(Action(SpriteRow::STANDING, 0, Point(0, 0), !facing_left));
      actions.push(Action(SpriteRow::STANDING, 1, Point(0, 0), !facing_left));
      actions.push(Action(SpriteRow::STANDING, 2, Point(0, 0), !facing_left));
      actions.push(Action(SpriteRow::STANDING, 3, Point(0, 0), !facing_left));
   }

   void move(bool moving_left)
   {
      Point movement = Point(moving_left ? MOVEMENT_SPEED : -MOVEMENT_SPEED, 0);
      actions.push(Action(SpriteRow::MOVING, 0, movement, !moving_left));
      actions.another_column(1).another_column(2).another_column(3);
   }

   void run(bool moving_left)
   {
      Point movement = Point(moving_left ? RUNNING_SPEED : -RUNNING_SPEED, 0);
      actions.push(Action(SpriteRow::RUNNING_UP, 0, movement, !moving_left));
      actions.another_column(1);
      actions.push(Action(SpriteRow::RUNNING_DOWN, 0, movement, !moving_left));
      actions.another_column(1);
   }

   void just_sit(bool facing_left, int sitting_action)
   {
      actions.push(Action(SpriteRow::SITTING, 0, Point(0, 0), !facing_left));
      actions.again();
      actions.another_column(sitting_action);
      actions.push(Action(SpriteRow::SITTING, 0, Point(0, 0), !facing_left));
   }

   void turn(bool moving_left)
   {
      actions.push(Action(SpriteRow::STANDING, 3, Point(0, 0), moving_left));
      actions.push(Action(SpriteRow::STANDING, 2, Point(0, 0), moving_left));
      actions.push(Action(SpriteRow::STANDING, 1, Point(0, 0), true));
      actions.push(Action(SpriteRow::STANDING, 2, Point(0, 0), !moving_left));
      actions.push(Action(SpriteRow::STANDING, 3, Point(0, 0), !moving_left));
   }

   void start_sitting(size_t times, const Action & prev)
   {
      if(prev.sprite.y == static_cast<int>(SpriteRow::MOVING)) {
         sit_down(!prev.mirrored);
      }
      if(prev.sprite.y == static_cast<int>(SpriteRow::RUNNING_DOWN)) {
         calm_down(!prev.mirrored);
         sit_down(!prev.mirrored);
      }
      while(times --> 0) {
         int sitting_action = rand() % 8;
         if(sitting_action >= 4) {
            sitting_action = 0;
         }
         just_sit(!prev.mirrored, sitting_action);
      }
   }
   void start_moving(size_t times, const Action & prev, bool moving_left)
   {
      if(prev.sprite.y == static_cast<int>(SpriteRow::SITTING)) {
         get_up(moving_left);
      } else if(prev.sprite.y == static_cast<int>(SpriteRow::RUNNING_DOWN)) {
         if(prev.mirrored == moving_left) {
            calm_down(!prev.mirrored);
            turn(moving_left);
         } else {
            calm_down(moving_left);
         }
      } else if(prev.sprite.y == static_cast<int>(SpriteRow::MOVING) && prev.mirrored == moving_left) {
         turn(moving_left);
      }
      while(times --> 0) {
         move(moving_left);
      }
   }
   void start_running(size_t times, const Action & prev, bool moving_left)
   {
      if(prev.sprite.y == static_cast<int>(SpriteRow::SITTING)) {
         get_up(moving_left);
         burst(moving_left);
      } else if(prev.sprite.y == static_cast<int>(SpriteRow::MOVING)) {
         if(prev.mirrored == moving_left) {
            turn(moving_left);
         }
         burst(moving_left);
      } else if(prev.sprite.y == static_cast<int>(SpriteRow::RUNNING_DOWN) && prev.mirrored == moving_left) {
         calm_down(!prev.mirrored);
         turn(moving_left);
         burst(moving_left);
      }
      while(times --> 0) {
         run(moving_left);
      }
   }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
   srand(static_cast<unsigned>(time(nullptr)) + GetCurrentProcessId());
   static const std::string screen_mate_window("ScreenMateWindow");
   try {
#ifdef DEBUG
      Console console;
#endif
      LPCSTR cmd = GetCommandLine();
      std::cout << cmd << std::endl;
      if(strstr(cmd, "-burst")) {
         std::string message = "burst";
         COPYDATASTRUCT copydata;
         copydata.dwData = 1; // An ID. Whatever, there would be only one function: to send a text message.
         copydata.cbData = message.size();
         copydata.lpData = (PVOID)message.c_str();
         HWND hwDispatch = FindWindow(screen_mate_window.c_str(), NULL);
         if(!hwDispatch) {
            return 1;
         }
         LRESULT result = SendMessage(hwDispatch,
               WM_COPYDATA,
               (WPARAM)(HWND) /*hWnd*/ NULL,
               (LPARAM) (LPVOID) &copydata
               );
         return result ? EXIT_SUCCESS : EXIT_FAILURE;
      }

      Rect desktop;
      GetWindowRect(GetDesktopWindow(), &desktop);
      Point start(0, 0);
      start.x = rand() % (desktop.width() - Fox::WINDOW_WIDTH);
      start.y = rand() % (desktop.height() - Fox::WINDOW_HEIGHT);

      MainWindow window(hInstance, start, Size(Fox::WINDOW_WIDTH, Fox::WINDOW_HEIGHT));
      window.register_class(screen_mate_window);
      window.load_resources(IDR_ARCTIC_FOX, Size(16, 16));
      window.create(nCmdShow, "Fox", /*frame_delay*/100/*ms*/, /*transparent*/RGB(255, 0, 255));
      Fox fox;
      window.set_character(&fox);
      return window.run_event_loop();
   } catch(const WinAPIException & e) {
      ShowLastErrorDialog(e.what());
   }
   return 1;
}

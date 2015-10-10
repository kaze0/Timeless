#ifndef TIMER_WINDOW_H_   /* Include guard */
#define TIMER_WINDOW_H_
#include <pebble.h>
	
void start_count(int ms);

void timer_back_click_handler(ClickRecognizerRef recognizer, void *context);

void timer_click_config_provider(void *context);
void timer_window_init();
void timer_window_deinit();

void stop_timer();

void timer_handle_second_tick(struct tm *tick_time, TimeUnits units_changed);

#endif
#include <timerWindow.h>
#include<pebble.h>
	
int counter = 0;
int target_counter;
int quadrant = 0;
Window *timer_window;
TextLayer *timer_text_layer;
Layer *quadrant_layer;
ActionBarLayer *timer_action_bar;
GBitmap *action_configure;
GBitmap *action_reset;

static const VibePattern timer_custom_pattern = {
  .durations = (uint32_t []) {100, 300, 100},
  .num_segments = 3
};


//void timer_back_click_handler(ClickRecognizerRef recognizer, void *context) {
//	stop_timer();
	//FIXME: EXIT
//}

void moveTime() {
	char *time_left= "000";
	snprintf(time_left, 4, "%d", target_counter - counter);
	text_layer_set_text(timer_text_layer, time_left);
	
	//Calculate where to move the timer
	Layer *window_layer = window_get_root_layer(timer_window);
	GRect bounds = layer_get_frame(window_layer);
	const int16_t half_h = (bounds.size.h) / 2;
	const int16_t half_w = (bounds.size.w - ACTION_BAR_WIDTH)/ 2;
	int x = 0;
	int y = 0;
	
	if (quadrant == 0) {
		x = 0;
		y = 0;
	}
	
	if (quadrant == 1) {
		x = half_w;
		y = 0;    
	}
	
	if (quadrant == 2) {
		x = 0;
		y = half_h;
	}
	
	if (quadrant == 3) {
		x = half_w;
		y = half_h;
	}
	
	layer_set_frame(text_layer_get_layer(timer_text_layer), GRect(x, y, half_w, half_h));
	layer_mark_dirty(text_layer_get_layer(timer_text_layer));
}

void timer_long_select_click_handler(ClickRecognizerRef recognizer, void *context) {
	counter = 0;
	quadrant = 0;
	moveTime();
}

void timer_long_select_confirm_handler(ClickRecognizerRef recognizer, void *context) {
	//Just vibe to confirm press
	vibes_short_pulse();
}

void timer_select_click_handler(ClickRecognizerRef recognizer, void *context) {
	counter = 0;
	moveTime();
}

void timer_up_click_handler(ClickRecognizerRef recognizer, void *context) {
	counter = counter - 30;
	moveTime();
}

void timer_down_click_handler(ClickRecognizerRef recognizer, void *context) {
	//FIXME: show config
}

void timer_click_config_provider(void *context) {
    //window_set_click_context(BUTTON_ID_UP, context); //Think it's a pointer to data
    //window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, up_click_handler); //Holding down, think scrolling

   // window_single_click_subscribe(BUTTON_ID_BACK, timer_back_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) timer_up_click_handler);

	window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) timer_down_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) timer_select_click_handler);
    window_long_click_subscribe(BUTTON_ID_SELECT, 250, (ClickHandler) timer_long_select_confirm_handler, (ClickHandler) timer_long_select_click_handler);

}

void quadrant_layer_draw(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);

	//Draw behind the action bar so we don't have white peaking out
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(bounds.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, bounds.size.h), 0, GCornerNone);

	
    // Draw a black filled rectangle with sharp corners
    //graphics_context_set_fill_color(ctx, GColorBlack);
    //graphics_fill_rect(ctx, bounds, 0, GCornerNone);

    // Draw a white filled circle a radius of half the layer height
    //graphics_context_set_fill_color(ctx, GColorWhite);
    const int16_t half_h = (bounds.size.h) / 2;
	const int16_t half_w = (bounds.size.w - ACTION_BAR_WIDTH) / 2;

    //graphics_fill_circle(ctx, GPoint(half_h, half_h), half_h);
	
	//Fill in the quadrant as need be
	//if (quadrant != 0) {
		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_rect(ctx, GRect(0,0, half_w, half_h), 0, GCornerNone);
	//}
	
	//if (quadrant != 1) {
		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_rect(ctx, GRect(half_w, 0, half_w, half_h), 0, GCornerNone);
    //}
	
	//if (quadrant != 2) {
		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_rect(ctx, GRect(0, half_h, half_w, half_h), 0, GCornerNone);
    //}
	
    //if (quadrant != 3) {
		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_rect(ctx, GRect(half_w, half_h, half_w, half_h), 0, GCornerNone);
    //}
}

void setup_action_layer() {
  // Initialize the action bar:
  timer_action_bar = action_bar_layer_create();
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(timer_action_bar, timer_window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(timer_action_bar,
                                             timer_click_config_provider);
  // Set the icons:
  // The loading the icons is omitted for brevity... See HeapBitmap.
  action_bar_layer_set_icon(timer_action_bar, BUTTON_ID_SELECT, action_reset);
  action_bar_layer_set_icon(timer_action_bar, BUTTON_ID_DOWN, action_configure);
}

//FIXME: lot's of drawing changed here, so it's quite sloppy now
void timer_window_init() {
	timer_window = window_create();
 	window_stack_push(timer_window, true /* Animated */);
	//window_set_click_config_provider(timer_window, timer_click_config_provider);
	
	Layer *window_layer = window_get_root_layer(timer_window);
	GRect bounds = layer_get_frame(window_layer);

	quadrant_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_set_update_proc(quadrant_layer, quadrant_layer_draw);
	layer_add_child(window_layer, quadrant_layer);
	
	const int16_t half_h = (bounds.size.h) / 2;
	const int16_t half_w = (bounds.size.w - ACTION_BAR_WIDTH) / 2;
	timer_text_layer = text_layer_create((GRect){ .origin = { 0, 0 }, .size = { half_w, half_h} });
	text_layer_set_font(timer_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	text_layer_set_text_color(timer_text_layer, GColorWhite);
	text_layer_set_text_alignment(timer_text_layer, GTextAlignmentCenter);
	text_layer_set_background_color(timer_text_layer, GColorBlack);
	layer_add_child(window_layer, text_layer_get_layer(timer_text_layer));
	
	action_configure = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_BAR_CONFIG);
	action_reset =  gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_BAR_RESET);
								  
	setup_action_layer();
}

void timer_window_deinit() {
	gbitmap_destroy(action_reset);
	gbitmap_destroy(action_configure);
	
	tick_timer_service_unsubscribe();
	text_layer_destroy(timer_text_layer);
	layer_destroy(quadrant_layer);
	
	window_stack_pop(true);
	window_destroy(timer_window);
}

void stop_timer() {
	timer_window_deinit();
}

void timer_handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	counter++;
	//FIXME: update screen
   	char *time_left= "000";
    snprintf(time_left, 4, "%d", target_counter - counter);
	
	text_layer_set_text(timer_text_layer, time_left);

	if (counter >= target_counter) {
		//Reset timer count for when we move it
		
		quadrant++;
		moveTime();
		
		counter = 0;

		if (quadrant >= 4) {			
			//For this app, let's reset it
			quadrant = 0;
		} else {
			//Not done so let's say to move on
		}
		vibes_enqueue_custom_pattern(timer_custom_pattern);
	}
}


void start_count(int seconds) {
	counter = 0;
	quadrant = 0;
	target_counter = seconds;
	
    tick_timer_service_subscribe(SECOND_UNIT, timer_handle_second_tick);
	timer_window_init();
	
	char *time_left= "000";
    snprintf(time_left, 4, "%d", target_counter);
	
	text_layer_set_text(timer_text_layer, time_left);
}


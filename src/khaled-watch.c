#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_quote_layer;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;

static void update_time() {

	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

	text_layer_set_text(s_time_layer, s_buffer);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

	update_time();
	
}

static void main_window_load(Window *window) {

	// SCREEN BOUNDS + BG COLOR //

	window_set_background_color(window, GColorBlack);

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	// BACKGROUND IMAGE //

	s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_KHALED);
	s_bitmap_layer = bitmap_layer_create(bounds);

	bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
	bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);

	layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));

	// THE TIME //

	s_time_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(60, 50), PBL_IF_ROUND_ELSE(38, 32), bounds.size.w, 50));

	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorBlack);
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));

	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

	// QUOTE //

	s_quote_layer = text_layer_create(GRect(0, 10, bounds.size.w, 50));

	text_layer_set_background_color(s_quote_layer, GColorClear);
	text_layer_set_text_color(s_quote_layer, GColorWhite);
	text_layer_set_text(s_quote_layer, "\"You smart.\"");
	text_layer_set_font(s_quote_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text_alignment(s_quote_layer, GTextAlignmentCenter);

	layer_add_child(window_layer, text_layer_get_layer(s_quote_layer));

}

static void main_window_unload(Window *window) {

	// Garbage Collect

	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_quote_layer);
	bitmap_layer_destroy(s_bitmap_layer);
	gbitmap_destroy(s_bitmap);

}

static void init() {

	// Create main Window and return a pointer.
	s_main_window = window_create();

	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	// Register with the TickTimer Service
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	// Display the Window.
	window_stack_push(s_main_window, true);

	update_time();

}

static void deinit() {

	// Destroy the Window.
	window_destroy(s_main_window);

}

int main(void) {

	init();
	app_event_loop();
	deinit();

}

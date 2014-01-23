#include <pebble.h>

static Window *window;
static TextLayer *time_layer;

static AppTimer* timer;
static double min = 0;

enum {
  KEY_UP = 0x0,
  KEY_SELECT = 0x1,
  KEY_DOWN = 0x2
};

void send(uint8_t k) {
  DictionaryIterator *it;
  app_message_outbox_begin(&it);
  Tuplet tuplet = TupletInteger(k, 0);
  dict_write_tuplet(it, &tuplet);
  dict_write_end(it);
  app_message_outbox_send();
}

void update_stopwatch() {
  static char big_time[] = "00:00";

  int hours = (int)min / 60;
  int minutes = (int)min % 60;
      
  snprintf(big_time, 6, "%02d:%02d", hours, minutes);

  // Now draw the strings.
  text_layer_set_text(time_layer, big_time);
}

void handle_timer(void* data) {
  min += 1;
  timer = app_timer_register(60000, handle_timer, NULL);
  update_stopwatch();
}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  send(KEY_SELECT);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  send(KEY_UP);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  send(KEY_DOWN);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  time_layer = text_layer_create((GRect) { .origin = { 0, 45 }, .size = { bounds.size.w, 50 } });
  text_layer_set_text(time_layer, "00:00");
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(time_layer);
}

static void init(void) {
  app_message_open(1, 20);

  timer = app_timer_register(60000, handle_timer, NULL);

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

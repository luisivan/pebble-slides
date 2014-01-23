#include <pebble.h>

static Window *window;
static TextLayer *time_layer;
static ScrollLayer *scroll_layer;
static TextLayer *text_layer;

static AppTimer* timer;
static double min = 0;

enum {
  KEY_UP = 0x0,
  KEY_SELECT = 0x1,
  KEY_DOWN = 0x2,
  KEY_START = 0x3,
  KEY_END = 0x4,
  KEY_NOTE = 0x5
};

// Send keys
void send(uint8_t k) {
  DictionaryIterator *it;
  app_message_outbox_begin(&it);
  Tuplet tuplet = TupletInteger(k, 0);
  dict_write_tuplet(it, &tuplet);
  dict_write_end(it);
  app_message_outbox_send();
}


// Text

char note[1024];
#define spacing 10

static void view_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect max_text_bounds = GRect(0, 0, bounds.size.w, 2000);

  scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_click_config_onto_window(scroll_layer, window);

  text_layer = text_layer_create(GRect(0, 0, max_text_bounds.size.w, max_text_bounds.size.h));
  text_layer_set_text(text_layer, note);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  GSize max_size = text_layer_get_content_size(text_layer);
  text_layer_set_size(text_layer, GSize(max_size.w, max_size.h+spacing));

  scroll_layer_set_content_size(scroll_layer, GSize(bounds.size.w, max_size.h));
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(text_layer));

  scroll_layer_set_click_config_onto_window(scroll_layer, window);

  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));
}

static void view_unload(Window *window) {
  text_layer_destroy(text_layer);
  scroll_layer_destroy(scroll_layer);
  window_destroy(window);
}

void view() {
  Window *view = window_create();
  window_set_window_handlers(view, (WindowHandlers) {
      .load = view_load,
      .unload = view_unload,
  });
  window_stack_push(view, true);
}

// Events
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  view();
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

// Stopwatch
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

// Notes

static void msg_received(DictionaryIterator *iter, void *context) {
  if (dict_find(iter, KEY_START)) strcpy(note, "");

  strcat(note, dict_find(iter, KEY_NOTE)->value->cstring);

  if (dict_find(iter, KEY_END)) view();
}

// Main window
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
  app_message_register_inbox_received(msg_received);
  app_message_open(app_message_inbox_size_maximum(), 20);

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

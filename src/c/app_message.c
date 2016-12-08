#include <pebble.h>

static Window *s_window;	
static TextLayer *s_txt_layer;
char str[50];
char str2[50];

enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1
};

static void prv_exit_application(void *data) {
  // App can exit to return directly to their default watchface
  exit_reason_set(APP_EXIT_NOT_SPECIFIED);

  // Exit the application by unloading the only window
  window_stack_remove(s_window, false);
}

// Create the AppGlance displayed in the system launcher
static void prv_update_app_glance(AppGlanceReloadSession *session, size_t limit, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "prv_update_app_glance %s",str2);
  // Check we haven't exceeded system limit of AppGlance's
  if (limit < 1) return;

  // Generate a friendly message for the current Lockitron state
  APP_LOG(APP_LOG_LEVEL_INFO, "STATE: %s", str2);

  // Create the AppGlanceSlice (no icon, no expiry)
  AppGlanceSlice entry = (AppGlanceSlice) {
    .layout = {
      .subtitle_template_string = str2
    },
    .expiration_time = APP_GLANCE_SLICE_NO_EXPIRATION
  };

  // Add the slice, and check the result
  const AppGlanceResult result = app_glance_add_slice(session, entry);
  if (result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppGlance Error: %d", result);
  }
}

static void prv_exit_delay() {
  // Get the system timeout duration
  int timeout = preferred_result_display_duration();

  // After the timeout, exit the application
  app_timer_register(timeout, prv_exit_application, NULL);
  app_glance_reload(prv_update_app_glance, NULL);
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32);
    text_layer_set_font(s_txt_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (tuple->value->uint32==1) {
      char *time_format="ON (%R)";
      strftime(str2, sizeof(str2), time_format, t);
      snprintf(str, sizeof(str), "%s", "ON");
      text_layer_set_text_color(s_txt_layer, GColorGreen);
      text_layer_set_text(s_txt_layer, str);
    }
    else {
      char *time_format="OFF (%R)";
      strftime(str2, sizeof(str2), time_format, t);
      snprintf(str, sizeof(str), "%s", "OFF");
      text_layer_set_text_color(s_txt_layer, GColorRed);
      text_layer_set_text(s_txt_layer, str);
    }
    prv_exit_delay();
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
	}
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
  APP_LOG(APP_LOG_LEVEL_ERROR, "in_dropped_handler");
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "out_failed_handler");

}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //Create a TextLayer to show the result
  s_txt_layer = text_layer_create(GRect(0, (bounds.size.h/2)-30, bounds.size.w, 60));
  text_layer_set_background_color(s_txt_layer, GColorClear);
  text_layer_set_text_color(s_txt_layer, GColorBlack);
  text_layer_set_font(s_txt_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_txt_layer, GTextAlignmentCenter);
  text_layer_set_text(s_txt_layer, "Navidad");
  layer_add_child(window_layer, text_layer_get_layer(s_txt_layer));
}
static void prv_window_unload(Window *window) {
//  window_destroy(s_window);
}

static void init(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init");
	s_window = window_create();
	window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, false);
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);

  // Initialize AppMessage inbox and outbox buffers with a suitable size
  const int inbox_size = 128;
  const int outbox_size = 128;
	app_message_open(inbox_size, outbox_size);
}

static void deinit(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "DEINIT");
	app_message_deregister_callbacks();
	window_destroy(s_window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}
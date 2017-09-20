#include "app_message.h"

static Window *s_window;	
static TextLayer *s_txt_layer;
char str[50];
char str2[50];

enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1
};

// A struct for our specific settings (see main.h)
ClaySettings settings;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialize the default settings
static void prv_default_settings() {
  strncpy(settings.urlconsulta, "http://192.168.0.2:18080/domo/iPHC-status.php?q=a3&b=true", sizeof(settings.urlconsulta));
  strncpy(settings.urlnavon, "http://192.168.0.2:18080/domo/d.php?q=ssm+navON", sizeof(settings.urlnavon));
  strncpy(settings.urlnavoff, "http://192.168.0.2:18080/domo/d.php?q=ssm+navOFF", sizeof(settings.urlnavoff));
}

// Read settings from persistent storage
static void prv_load_settings() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "prv_load_settings");
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "prv_save_settings");
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, MESSAGE_KEY_status);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Received status: (0=off,1=on,2=error) [%d]", (int)tuple->value->uint32);
    text_layer_set_font(s_txt_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (tuple->value->uint32==0) {
      char *time_format="OFF (%R)";
      strftime(str2, sizeof(str2), time_format, t);
      snprintf(str, sizeof(str), "%s", "OFF");
      text_layer_set_text_color(s_txt_layer, GColorBlack);
      text_layer_set_text(s_txt_layer, str);
    }
    else if (tuple->value->uint32==1) {
      char *time_format="ON (%R)";
      strftime(str2, sizeof(str2), time_format, t);
      snprintf(str, sizeof(str), "%s", "ON");
      text_layer_set_text_color(s_txt_layer, GColorGreen);
      text_layer_set_text(s_txt_layer, str);
    }
    else if (tuple->value->uint32==2) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Received ERROR status: %d", (int)tuple->value->uint32);
      snprintf(str, sizeof(str), "%s", "ERROR");
      text_layer_set_text_color(s_txt_layer, GColorRed);
      text_layer_set_text(s_txt_layer, str);
    }
    else {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Received unknown status: %d", (int)tuple->value->uint32);
    }
    prv_exit_delay();
	}
  bool hayConfiguracion=false;
  ////////////////////////////////
  // valores de configuracion
  ////////////////////////////////
	tuple = dict_find(received, MESSAGE_KEY_urlconsulta);
	if(tuple) {
    snprintf(settings.urlconsulta, sizeof(settings.urlconsulta), "%s", tuple->value->cstring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "-Received URL Consulta from Phone: [%s] [%u] [%s]", tuple->value->cstring, sizeof(settings.urlconsulta), settings.urlconsulta);
    hayConfiguracion=true;
	}
	tuple = dict_find(received, MESSAGE_KEY_urlnavon);
	if(tuple) {
    snprintf(settings.urlnavon, sizeof(settings.urlnavon), "%s", tuple->value->cstring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "-Received URL Navidad ON from Phone: [%s] [%u] [%s]", tuple->value->cstring, sizeof(settings.urlnavon), settings.urlnavon);
    hayConfiguracion=true;
	}	
	tuple = dict_find(received, MESSAGE_KEY_urlnavoff);
	if(tuple) {
    snprintf(settings.urlnavoff, sizeof(settings.urlnavoff), "%s", tuple->value->cstring);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "-Received URL Navidad OFF from Phone: [%s] [%u] [%s]", tuple->value->cstring, sizeof(settings.urlnavoff), settings.urlnavoff);
    hayConfiguracion=true;
	}	
  if (hayConfiguracion) {
    prv_save_settings();
  }
  ////////////////////////////////
  ////////////////////////////////
  tuple = dict_find(received, MESSAGE_KEY_message);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
    if (startsWith("init=",tuple->value->cstring)) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Received from PHONE: JS Ready");
      send_configuration_message_to_phone();
    }
    else if (startsWith("settings=",tuple->value->cstring)) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Received from PHONE: CONFIGURING SETTINGS");
      snprintf(str, sizeof(str), "%s", "Conf...");
      text_layer_set_text_color(s_txt_layer, GColorBlue);
      text_layer_set_text(s_txt_layer, str);
    }
    else if (startsWith("configured=",tuple->value->cstring)) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Received from PHONE: JS Configured");
    }
    else if (startsWith("finish=",tuple->value->cstring)) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Received from PHONE: FINISH");
      snprintf(str, sizeof(str), "%s", "Fin conf.");
      text_layer_set_text_color(s_txt_layer, GColorWindsorTan);
      text_layer_set_text(s_txt_layer, str);
      prv_exit_delay();
    }
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

static void send_configuration_message_to_phone(){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "send_configuration_message_to_phone");
  DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
  dict_write_cstring(iter, MESSAGE_KEY_configurlconsulta, settings.urlconsulta);
  dict_write_cstring(iter, MESSAGE_KEY_configurlnavon, settings.urlnavon);
  dict_write_cstring(iter, MESSAGE_KEY_configurlnavoff, settings.urlnavoff);
  dict_write_uint8(iter, MESSAGE_KEY_configure, 1);
	dict_write_end(iter);
  app_message_outbox_send();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
  const int inbox_size = MAX_LENGTH*3+10;
  const int outbox_size = MAX_LENGTH*3+10;
	app_message_open(inbox_size, outbox_size);

  prv_load_settings();
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
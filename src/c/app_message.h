#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1
#define MAX_LENGTH 70

// A structure containing our settings
typedef struct ClaySettings {
  char urlconsulta[MAX_LENGTH];
  char urlnavon[MAX_LENGTH];
  char urlnavoff[MAX_LENGTH];
} __attribute__((__packed__)) ClaySettings;

static bool startsWith(const char *pre, const char *str);

static void prv_default_settings();
static void prv_load_settings();
static void prv_save_settings();

static void prv_exit_application(void *data);
static void prv_update_app_glance(AppGlanceReloadSession *session, size_t limit, void *context);
static void prv_exit_delay();

static void in_received_handler(DictionaryIterator *received, void *context);
static void in_dropped_handler(AppMessageResult reason, void *context);
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);

static void prv_window_load(Window *window);
static void prv_window_unload(Window *window);

static void send_configuration_message_to_phone();
static void send_start_message_to_phone();
#include "tic_internal.h"

int tic_string_yaml_write_handler(void * data, uint8_t * buffer, size_t size)
{
  tic_string * string = (tic_string *)data;

  // TODO
  fprintf(stderr, "hey write: %d \"%s\"\n", size, buffer);

  (void)string;
  (void)buffer;
  (void)size;

  return 1;
}

tic_error * tic_settings_to_string(const tic_settings * settings, char ** string)
{
  if (string == NULL)
  {
    return tic_error_create("String output pointer is NULL.");
  }

  *string = NULL;

  if (settings == NULL)
  {
    return tic_error_create("Settings pointer is NULL.");
  }

  tic_error * error = NULL;

  bool emitter_initialized = false;
  yaml_emitter_t emitter;
  if (error == NULL)
  {
    int success = yaml_emitter_initialize(&emitter);
    if (success)
    {
      emitter_initialized = true;
    }
    else
    {
      error = tic_error_create("Failed to initialize YAML emitter.");
    }
  }

  tic_string str;
  tic_string_setup(&str);

  yaml_emitter_set_output(&emitter, tic_string_yaml_write_handler, &str);

  yaml_event_t event;
  if (error == NULL)
  {
    yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);  // TODO: check error
    int success = yaml_emitter_emit(&emitter, &event);
    if (!success)
    {
      error = tic_error_create("Fail to start YAML stream.");
    }
  }

  if (error == NULL)
  {
    yaml_document_start_event_initialize(&event, NULL, NULL, NULL, 0);
    int success = yaml_emitter_emit(&emitter, &event);
    if (!success)
    {
      error = tic_error_create("Failed to start YAML document.");
    }
  }

  if (error == NULL)
  {
    yaml_mapping_start_event_initialize(&event, NULL, NULL, 0, 0);
    int success = yaml_emitter_emit(&emitter, &event);
    if (!success)
    {
      error = tic_error_create("Failed to start YAML mapping.");
    }
  }

  if (error == NULL)
  {
    // tmphax
    uint8_t x[2] = { 'f', 'h' };
    yaml_scalar_event_initialize(&event, NULL, NULL, x, 2, 0, 0, 0);
    int success = yaml_emitter_emit(&emitter, &event);
    if (!success)
    {
      error = tic_error_create("Failed to tmphax scalar.");
    }
  }

  if (error == NULL)
  {
    // tmphax
    uint8_t x[2] = { 'm', '2' };
    yaml_scalar_event_initialize(&event, NULL, NULL, x, 2, 0, 0, 0);
    int success = yaml_emitter_emit(&emitter, &event);
    if (!success)
    {
      error = tic_error_create("Failed to tmphax scalar 2.");
    }
  }

  if (error == NULL)
  {
    yaml_mapping_end_event_initialize(&event);
    int success = yaml_emitter_emit(&emitter, &event);
    if (!success)
    {
      error = tic_error_create("Failed to end YAML mapping.");
    }
  }

  if (error == NULL)
  {
    yaml_document_end_event_initialize(&event, 0);
    int success = yaml_emitter_emit(&emitter, &event);
    if (!success)
    {
      error = tic_error_create("Failed to end YAML document.");
    }
  }

  if (error == NULL)
  {
    yaml_stream_end_event_initialize(&event);  // TODO: check error
    int success = yaml_emitter_emit(&emitter, &event);
    if (!success)
    {
      error = tic_error_create("Failed to end YAML stream.");
    }
  }

  if (error == NULL && str.data == NULL)
  {
    error = &tic_error_no_memory;
  }

  if (error == NULL)
  {
    *string = str.data;
    str.data = NULL;
  }

  tic_string_free(str.data);

  if (emitter_initialized)
  {
    yaml_emitter_delete(&emitter);
  }

  return error;
}

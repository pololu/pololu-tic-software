#include "tic_internal.h"

tic_error * tic_settings_read_from_string(const char * string,
  tic_settings ** settings, char ** warnings)
{
  if (warnings != NULL)
  {
    *warnings = NULL;
  }

  if (string == NULL)
  {
    return tic_error_create("Settings input string is NULL.");
  }

  if (settings == NULL)
  {
    return tic_error_create("Settings output pointer is NULL.");
  }

  tic_error * error = NULL;

  tic_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = tic_settings_create(&new_settings);
  }

  tic_string wstr;
  if (warnings)
  {
    tic_string_setup(&wstr);
  }
  else
  {
    tic_string_setup_dummy(&wstr);
  }

  bool parser_initialized = false;
  yaml_parser_t parser;
  if (error == NULL)
  {
    int success = yaml_parser_initialize(&parser);
    if (success)
    {
      parser_initialized = true;
    }
    else
    {
      error = tic_error_create("Failed to initialize YAML parser.");
    }
  }

  if (error == NULL)
  {
    yaml_parser_set_input_string(&parser, (const uint8_t *)string, strlen(string));
  }

  bool document_initialized = false;
  yaml_document_t doc;

  if (error == NULL)
  {
    int success = yaml_parser_load(&parser, &doc);
    if (success)
    {
      document_initialized = true;
    }
    else
    {
      error = tic_error_create("Failed to load document.");
    }
  }

  // Detect memory allocation errors from the warnings string.
  if (error == NULL && warnings && wstr.data == NULL)
  {
    error = &tic_error_no_memory;
  }

  // Pass the settings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  // Pass the warnings string to the caller.
  if (error == NULL && warnings)
  {
    *warnings = wstr.data;
    wstr.data = NULL;
  }

  if (document_initialized)
  {
    yaml_document_delete(&doc);
  }

  if (parser_initialized)
  {
    yaml_parser_delete(&parser);
  }

  tic_string_free(wstr.data);

  tic_settings_free(new_settings);

  return error;
}

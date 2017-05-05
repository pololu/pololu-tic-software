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

  while (true)
  {
    yaml_event_t event;
    int success = yaml_parser_parse(&parser, &event);
    if (!success)
    {
      // TODO: better error handling, report line of error
      error = tic_error_create("Failed to parse YAML.");
      break;
    }

    if (event.type == YAML_STREAM_END_EVENT)
    {
      break;
    }

    if (event.type == YAML_SCALAR_EVENT)
    {
      printf("  scalar %s\n", event.data.scalar.value);
    }
    else
    {
      printf("hey %d\n", event.type);
    }

    yaml_event_delete(&event);
  }

#if 0
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

  if (error == NULL)
  {
    // tmphax
    printf("string@%p\n", string);
    printf("doc@%p\n", &doc);
    int i = 1;
    printf("  rootnode@%p\n", yaml_document_get_root_node(&doc));
    while (true)
    {
      yaml_node_t * node = yaml_document_get_node(&doc, i);
      if (node == NULL) { break; }
      printf("  node@%p type=%d tag=%s ", node, node->type, node->tag);
      printf("    %d:%d to %d:%d\n",
        node->start_mark.line, node->start_mark.line,
        node->end_mark.line, node->end_mark.line);
      if (node->type == YAML_SCALAR_NODE)
      {
        printf("    value=%p len=%d\n", node->data.scalar.value, node->data.scalar.length);
        printf("    ");
        for (uint32_t i = 0; i < node->data.scalar.length; i++)
        {
          putchar(node->data.scalar.value[i]);
        }
        printf("\n");
      }
      if (node->type == YAML_MAPPING_NODE)
      {
        for (yaml_node_pair_t * pair = node->data.mapping.pairs.start;
             pair < node->data.mapping.pairs.end; pair++)
        {
          printf("    pair@%p %d %d\n", pair, pair->key, pair->value);
        }
      }
      i++;
    }
  }
#endif

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

  if (error == NULL)
  {
    error = tic_error_create("not implemented yo");
  }

  return error;
}

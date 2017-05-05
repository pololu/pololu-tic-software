#include "tic_internal.h"

static tic_error * read_from_yaml_doc(yaml_document_t * doc, tic_settings * settings)
{
  yaml_node_t * root = yaml_document_get_root_node(doc);
  if (root->type != YAML_MAPPING_NODE)
  {
    return tic_error_create("YAML root node is not a mapping.");
  }

  for (yaml_node_pair_t * pair = root->data.mapping.pairs.start;
       pair < root->data.mapping.pairs.top; pair++)
  {
    yaml_node_t * key = yaml_document_get_node(doc, pair->key);
    yaml_node_t * value = yaml_document_get_node(doc, pair->value);
    printf("%s -> %s\n", key->data.scalar.value, value->data.scalar.value);
  }

  (void)settings; // tmphax

  return NULL;
}

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

  // Allocate a new settings object.
  tic_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = tic_settings_create(&new_settings);
  }

  // If needed, allocate a string for holding warnings.
  tic_string wstr;
  if (warnings)
  {
    tic_string_setup(&wstr);
  }
  else
  {
    tic_string_setup_dummy(&wstr);
  }

  // Make a YAML parser.
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

  // Construct a YAML document using the parser.
  bool document_initialized = false;
  yaml_document_t doc;
  if (error == NULL)
  {
    yaml_parser_set_input_string(&parser, (const uint8_t *)string, strlen(string));
    int success = yaml_parser_load(&parser, &doc);
    if (success)
    {
      document_initialized = true;
    }
    else
    {
      // TODO: better error message
      error = tic_error_create("Failed to load document.");
    }
  }

  // Proces the YAML document.
  if (error == NULL)
  {
    error = read_from_yaml_doc(&doc, new_settings);
  }

  // Detect memory allocation errors from the warnings string.
  if (error == NULL && warnings && wstr.data == NULL)
  {
    error = &tic_error_no_memory;
  }

  // Success!  Pass the settings and warnings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
    if (warnings)
    {
      *warnings = wstr.data;
      wstr.data = NULL;
    }
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

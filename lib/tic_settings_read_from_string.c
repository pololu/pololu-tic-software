#include "tic_internal.h"

// TODO: add RSpec examples for every error in this file

// We apply the product name from the settings file first, and use it to set the
// defaults.
static tic_error * apply_product_name(tic_settings * settings, const char * product_name)
{
  uint32_t product;
  if (!tic_name_to_code(tic_product_names, product_name, &product))
  {
    return tic_error_create("Unrecognized product name.");
  }
  tic_settings_product_set(settings, product);
  tic_settings_fill_with_defaults(settings);
  return NULL;
}

static tic_error * apply_string_pair(tic_settings * settings,
  const char * key, const char * value, uint32_t line)
{
  if (!strcmp(key, "product"))
  {
    // We already processed the product field separately.
  }
  else if (!strcmp(key, "control_mode"))
  {
    uint32_t control_mode;
    if (!tic_name_to_code(tic_control_mode_names, value, &control_mode))
    {
      return tic_error_create("Invalid control_mode.");
    }
    tic_settings_control_mode_set(settings, control_mode);
  }
  else
  {
    return tic_error_create("Unrecognized key at line %d.", line);
  }

  return NULL;
}

// Returns true if the node is a scalar and its value equals the given
// null-terminated string.
static bool scalar_eq(const yaml_node_t * node, const char * v)
{
  if (node == NULL) { return NULL; }
  if (node->type != YAML_SCALAR_NODE) { return false; }
  if (node->data.scalar.length != strlen(v)) { return false; }
  return !memcmp(v, node->data.scalar.value, node->data.scalar.length);
}

// Given a mapping and a key name (as a null-terminated string), gets the node
// corresponding to the value, or NULL if it could not be found.
static yaml_node_t * map_lookup(yaml_document_t * doc,
  yaml_node_t * map, const char * key_name)
{
  if (doc == NULL) { assert(0); return NULL; }
  if (map == NULL) { assert(0); return NULL; }
  if (map->type != YAML_MAPPING_NODE) { return NULL; }

  for (yaml_node_pair_t * pair = map->data.mapping.pairs.start;
       pair < map->data.mapping.pairs.top; pair++)
  {
    yaml_node_t * key = yaml_document_get_node(doc, pair->key);
    if (scalar_eq(key, key_name))
    {
      return yaml_document_get_node(doc, pair->value);
    }
  }
  return NULL;
}

#define MAX_SCALAR_LENGTH 255

// Takes a key-value pair from the YAML file, does some basic checks, creates
// proper null-terminated C strings, and then calls apply_string_pair to do the
// actual logic of parsing strins and applying the settings.
static tic_error * apply_yaml_pair(tic_settings * settings,
  const yaml_node_t * key, const yaml_node_t * value)
{
  if (key == NULL)
  {
    return tic_error_create("Internal YAML processing error: Invalid key index.");
  }
  if (value == NULL)
  {
    return tic_error_create("Internal YAML processing error: Invalid value index.");
  }

  uint32_t line = key->start_mark.line + 1;

  // Make sure the key is valid and convert it to a C string (we aren't sure
  // that libyaml always provides a null termination byte because scalars can
  // have have null bytes in them).
  if (key->type != YAML_SCALAR_NODE)
  {
    return tic_error_create(
      "YAML key is not a scalar on line %d.", line);
  }
  if (key->data.scalar.length > MAX_SCALAR_LENGTH)
  {
    return tic_error_create(
      "YAML key is too long on line %d.", line);
  }
  char key_str[MAX_SCALAR_LENGTH + 1];
  memcpy(key_str, key->data.scalar.value, key->data.scalar.length);
  key_str[key->data.scalar.length] = 0;

  // Make sure the value is valid and convert it to a C string.
  if (value->type != YAML_SCALAR_NODE)
  {
    return tic_error_create(
      "YAML value is not a scalar on line %d.", line);
  }
  if (value->data.scalar.length > MAX_SCALAR_LENGTH)
  {
    return tic_error_create(
      "YAML value is too long on line %d.", line);
  }
  char value_str[MAX_SCALAR_LENGTH + 1];
  memcpy(value_str, value->data.scalar.value, value->data.scalar.length);
  value_str[value->data.scalar.length] = 0;

  return apply_string_pair(settings, key_str, value_str, line);
}

// Validates the YAML doc and populates the settings object with the settings
// from the document.
//
// If there are any warnings they will be added to wstr.
static tic_error * read_from_yaml_doc(
  yaml_document_t * doc, tic_settings * settings)
{
  assert(doc != NULL);
  assert(settings != NULL);

  // Get the root node and make sure it is a mapping.
  yaml_node_t * root = yaml_document_get_root_node(doc);
  if (root->type != YAML_MAPPING_NODE)
  {
    return tic_error_create("YAML root node is not a mapping.");
  }

  // Process the "product" key/value pair first.
  yaml_node_t * product_value = map_lookup(doc, root, "product");
  if (product_value == NULL)
  {
    return tic_error_create("No product was specified in the settings file.");
  }
  uint32_t product_line = product_value->start_mark.line + 1;
  if (product_value->type != YAML_SCALAR_NODE)
  {
    return tic_error_create(
      "YAML product value is not a scalar on line %d.", product_line);
  }
  if (product_value->data.scalar.length > MAX_SCALAR_LENGTH)
  {
    return tic_error_create(
      "YAML product value is too long on line %d.", product_line);
  }
  char product_str[MAX_SCALAR_LENGTH + 1];
  memcpy(product_str, product_value->data.scalar.value,
    product_value->data.scalar.length);
  product_str[product_value->data.scalar.length] = 0;
  apply_product_name(settings, product_str);

  // Iterate over the pairs in the YAML mapping and process each one.
  for (yaml_node_pair_t * pair = root->data.mapping.pairs.start;
       pair < root->data.mapping.pairs.top; pair++)
  {
    yaml_node_t * key = yaml_document_get_node(doc, pair->key);
    yaml_node_t * value = yaml_document_get_node(doc, pair->value);
    tic_error * error = apply_yaml_pair(settings, key, value);
    if (error) { return error; }
  }

  return NULL;
}

// TODO: we don't actually have any warnings yet, right?  Maybe remove the param?
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

  if (error != NULL)
  {
    error = tic_error_add(error, "There was an error reading the settings file.");
  }

  return error;
}

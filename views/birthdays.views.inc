<?php

/**
 * @file
 * Declare Views integration for the Birthdays module.
 */

/**
 * Implements hook_field_views_data_alter().
 */
function birthdays_field_views_data_alter(&$data, &$field, &$module) {
  if ($field['type'] == 'birthdays_date') {
    dpm($data);
    $name = $field['field_name'];

    $data['field_data_' . $name][$name]['field']['handler'] = 'birthdays_field_views_handler_field';

    $data['field_data_' . $name][$name]['sort'] = array(
      'field' => $name,
      'table' => 'field_data_' . $name,
      'handler' => 'birthdays_field_views_handler_sort',
      'additional fields' => array(
        $name . '_day',
        $name . '_month',
      ),
    );
  }
}

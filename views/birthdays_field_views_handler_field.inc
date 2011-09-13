<?php

/**
 * @file
 * The birthdays_field_views_handler_field class.
 */

define('BIRTHDAYS_SORT_ALL', 0);
define('BIRTHDAYS_SORT_YEAR', 1);
define('BIRTHDAYS_SORT_NOW', 2);

/**
 * A Views field handler that provides click sorting of birthday fields.
 */
class birthdays_field_views_handler_field extends views_handler_field_field {

  /**
   * Overrides views_handler_field_field::option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['click_sort_mode'] = array(
      'default' => BIRTHDAYS_SORT_YEAR,
    );

    return $options;
  }

  /**
   * Overrides views_handler_field_field::options_form().
   */
  function options_form(&$form, &$form_state) {
    // Adjust the parent form.
    parent::options_form($form, $form_state);
    unset($form['click_sort_column']);

    // Add a sorting option.
    $form['click_sort_mode'] = array(
      '#type' => 'radios',
      '#title' => 'Click sort mode',
      '#options' => array(
        BIRTHDAYS_SORT_ALL => t('Sort by year, month and day.'),
        BIRTHDAYS_SORT_YEAR => t('Sort by month and day.'),
        BIRTHDAYS_SORT_NOW => t('Sort by time to next birthday.'),
      ),
      '#description' => t('The way this column should beclick sorted.'),
      '#fieldset' => 'more',
    );
  }

  /**
   * Overrides views_handler_field_field::click_sort().
   */
  function click_sort($order) {
    // Ensure the table exists.
    $this->ensure_my_table();

    // Look up the real column names.
    $columns = array('day' => NULL, 'month' => NULL, 'year' => NULL);
    foreach ($columns as $delta => $column) {
      $column = _field_sql_storage_columnname($this->definition['field_name'], $delta);
      if (!isset($this->aliases[$column])) {
        $this->aliases[$column] = $this->table_alias . '.' . $column;
      }
      $columns[$delta] = $this->aliases[$column];
    }

    // Add sorting to the query.
    $mode = $this->options['click_sort_mode'];
    if ($mode == BIRTHDAYS_SORT_ALL) {
      $this->query->add_orderby(NULL, NULL, $order, $columns['year']);
    }
    if ($mode == BIRTHDAYS_SORT_ALL || $mode == BIRTHDAYS_SORT_YEAR) {
      $this->query->add_orderby(NULL, NULL, $order, $columns['month']);
      $this->query->add_orderby(NULL, NULL, $order, $columns['day']);
    }
    else {
      $now = date('m') * 32 + date('d');
      switch (Database::getConnection()->databaseType()) {
        case 'mysql':
        case 'mysqli':
        case 'pgsql':
          $formula = "CASE WHEN {$columns['month']}*32 + {$columns['day']} >= $now THEN {$columns['month']}*32 + {$columns['day']} ELSE {$columns['month']}*32 + {$columns['day']} + 1000 END";
          break;
      }
      if (isset($formula)) {
        $this->query->add_orderby(NULL, $formula, $order, $this->definition['field_name'] . '_sort');
      }
    }
  }
}

<?php

/**
 * @file
 * The birthdays_field_views_handler_sort class.
 */

/**
 * A views handler for sorting by a birthday field.
 */
class birthdays_field_views_handler_sort extends views_handler_sort {

  /**
   * Overrides views_handler_sort::query().
   * Add an ORDER_BY to the query.
   */
  function query() {
    $this->ensure_my_table();

    $this->query->add_orderby($this->table_alias, $this->field . '_year', $this->options['order']);
    $this->query->add_orderby($this->table_alias, $this->field . '_month', $this->options['order']);
    $this->query->add_orderby($this->table_alias, $this->field . '_day', $this->options['order']);
  }

  /**
   * Add an "ORDER BY time to next birthday" to a query.
   *
   * @param $query
   *   The Views query.
   * @param $order
   *   DESC or ASC.
   * @param
   *   An array with the keys year, month and day where the values are the real
   *   column names in the query.
   * @param $fieldname
   *   The name for the sorting column.
   * @param $time
   *   The current time as a unix timestamp.
   */
  public static function addOrderBy($query, $order, $columns, $fieldname, $time) {
    // Use the formula to calculate a value for the current time.
    $now = date('m', $time) * 32 + date('d', $time);

    // Get the formula for the current database type.
    switch (Database::getConnection()->databaseType()) {
      case 'mysql':
      case 'mysqli':
      case 'pgsql':
        $formula = "CASE WHEN {$columns['month']}*32 + {$columns['day']} >= $now THEN {$columns['month']}*32 + {$columns['day']} ELSE {$columns['month']}*32 + {$columns['day']} + 1000 END";
        break;
    }

    // Add the ORDER BY.
    if (isset($formula)) {
      $query->add_orderby(NULL, $formula, $order, $fieldname);
    }
  }


}

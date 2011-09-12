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

}

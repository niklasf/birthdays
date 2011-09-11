<?php

/**
 * @file
 * Hooks provided by the Birthdays module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Executed on birthdays.
 *
 * @param $op
 *   "has_birthday" unless a module defines another operation.
 *
 * @param $entity
 *   The entity the birthday field is attached to. For example the user
 *   object.
 */
function hook_birthdays($op, $entity) {
  if (isset($entity->name)) {
    watchdog('birthdays', "It's %name's birthday.", array('%name' => $entity->name));
  }
}

/**
 * @} End of "addtogroup hooks".
 */

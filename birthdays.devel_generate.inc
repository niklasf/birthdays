<?php

/**
 * Implements hook_devel_generate().
 */
function birthdays_devel_generate($object, $field, $instance, $bundle) {
  while (TRUE) {
    try {
      if (rand(0, 1)) {
        $year = rand(date('Y') - 90, date('Y') - 1);
      }
      else {
        $year = 0;
      }
      return BirthdaysBirthday::fromDate($year, rand(1, 12), rand(1, 31))->toArray();
    }
    catch (InvalidArgumentException $e) { }
  }
}


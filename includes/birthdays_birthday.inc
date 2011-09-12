<?php

/**
 * @file
 * The BirthdaysBirthday class.
 */

/**
 * Convert between different data representations and do calculations on a
 * birthday.
 */
class BirthdaysBirthday {

  /**
   * The internal database value.
   */
  private $value;

  /**
   * Private constructur. Use on of the create methods instead.
   */
  private function __construct($value = array('year' => 0, 'month' => 0, 'day' => 0)) {
    $this->value = $value;
  }

  /**
   * Create an empty birthday value.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function fromEmpty() {
    return new BirthdaysBirthday();
  }

  /**
   * Create a birthday from text.
   *
   * @todo
   *   Currently this only parses german dd.mm(.yyyy) dates. Use Drupal's date
   *   system to do this in a generic way.
   *
   * @param $text
   *   The text representation of a birthday.
   *
   * @throws InvalidArgumentException when the given text is invalid.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function fromString($text) {
    if (trim($text) === '') {
      return self::fromEmpty();
    }

    if (preg_match('/^\s*(\d{1,2})\.(\d{1,2})\.?(\d{4}|\d{2})?\s*$/', $text, $matches)) {
      return self::fromDate(isset($matches[3]) ? $matches[3] : 0, $matches[2], $matches[1]);
    }

    throw new InvalidArgumentException(t("The given argument is not a valid birthday."));
  }

  /**
   * Create a birthday from values.
   *
   * @param $year
   *   The year or empty for no year.
   * @param $month
   *   The month (1 to 12).
   * @param $day
   *   The day (1 to 31).
   *
   * @throws InvalidArgumentException when one of the given values is invalid.
   */
  public static function fromDate($year, $month, $day) {
    // All parameters are integers.
    $year = intval($year);
    $month = intval($month);
    $day = intval($day);

    // Check for all empty.
    if (!$year && !$month && !$day) {
      return self::fromEmpty();
    }

    // Validate month.
    if ($month < 1 || $month > 12) {
      throw new InvalidArgumentException(t("%month is not a valid month.", array('%month' => $month)));
    }

    // The maximum days of a month, in a leap year. Indexed by month.
    $maxdays = array(1 => 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);

    if ($year) {
      // Negative years not allowed.
      if ($year < 0) {
        throw new InvalidArgumentException(t('Negative years (%year) are not allowed.', array('%year' => $year)));
      }

      // Adjust year if only 2 digits are given.
      if ($year <= 99) {
        if ($year >= date('y')) {
          $year += date('Y') - date('y') - 100;
        }
        else {
          $year += date('Y') - date('y');
        }
      }

      // Check if it's in the past more than the world record.
      if ($year < date('Y') - 115) {
        throw new InvalidArgumentException(t("%year is too far in the past.", array('%year' => $year)));
      }

      // Februrary has only 28 days if it's not a leap year.
      if (!($year % 400 == 0 || ($year % 4 == 0 && $year % 100 != 0))) {
        $maxdays[2] = 28;
      }
 
      // Check if it's in the future.
      $unixtime = mktime(1, 0, 0, $month, $day, $year);
      if ($unixtime >= time()) {
        throw new InvalidArgumentException(t("The given date is in the future."));
      }
   }

    // Validate day.
    if ($day < 1 || $day > $maxdays[$month]) {
      throw new InvalidArgumentException(t("%day is not a valid day of month %month", array('%day' => $day, '%month' => $month)));
    }

    return new BirthdaysBirthday(array('year' => $year, 'month' => $month, 'day' => $day));
  }

  /**
   * Create a birthday from a database value.
   *
   * @param $value
   *   An associative array with year, month and day set.
   *
   * @throws InvalidArgumentException when FALSE is given.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function fromArray($value) {
    if (!is_array($value)) {
      throw new InvalidArgumentException(t("Must be an array."));
    }

    if (!isset($value['year'])) {
      $value['year'] = 0;
    }

    if (!isset($value['day']) || !isset($value['month'])) {
      throw new InvalidArgumentException(t('Day and month must be set.'));
    }

    return self::fromDate($value['year'], $value['month'], $value['day']);
  }

  /**
   * Converts the value to a string.
   *
   * @todo
   *   Use proper Drupal date formatting instead of dd.mm(.YYYY).
   *
   * @returns A string representation of the value.
   */
  public function toString() {
    if ($this->isEmpty()) {
      return '';
    }

    if ($this->value['year']) {
      return date('d.m.Y', $this->toUnixtime());
    }
    else {
      return date('d.m.', $this->toUnixtime());
    }
  }

  /**
   * Converts the value to an associative array.
   *
   * @return
   *   An associative array with these indexes:
   *    - year (4 digits or 0)
   *    - month (1 to 12)
   *    - day (1 to 31)
   */
  public function toArray() {
    return $this->value;
  }

  /**
   * Coverts the value to a unix timestamp.
   *
   * @return
   *   A unix timestamp where the year 1800 means no year. Or NULL.
   */
  public function toUnixtime() {
    if ($this->isEmpty()) {
      return NULL;
    }

    return mktime(
      1, // hour
      0, // minute
      0, // secound
      $this->value['month'],
      $this->value['day'],
      $this->value['year'] ? $this->value['year'] : 1800
    );
  }

  /**
   * Whether the value is empty or not.
   *
   * @return
   *   TRUE if the value is empty.
   */
  public function isEmpty() {
    return !$this->value['year'] && !$this->value['month'] && !$this->value['day'];
  }

  /**
   * Calculate the age for a specific year.
   *
   * @param $year
   *   Optionally give a year other than the current.
   *
   * @return
   *   The age.
   */
  public function getAge($year = 0) {
    if ($this->value['year'] == 0) {
      return t('Age unknown');
    }
    else {
      if (!$year) {
        $year = date('Y');
      }
      return $year - $this->value['year'];
    }
  }
}

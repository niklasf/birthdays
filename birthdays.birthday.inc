<?php

/**
 * @file
 * The BirthdaysBirthday class.
 */

/**
 * Convert between different data representations and do calculations on a
 * birthday.
 */
final class BirthdaysBirthday {

  /**
   * The internal database value.
   */
  private $value = NULL;

  /**
   * Private constructur. Use on of the create methods instead.
   */
  private function __construct($value = NULL) {
    if ($value !== NULL) {
      $this->value = intval($value);
    }
  }

  /**
   * Create an empty birthday value.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function createEmpty() {
    return new BirthdaysBirthday();
  }

  /**
   * Create a birthday from text.
   *
   * @param $text
   *   The text representation of a birthday.
   *
   * @throws InvalidArgumentException when the given text is invalid.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function createFromText($text) {
    if (trim($text) === '') {
      return self::createEmpty();
    }

    if (preg_match('/^\s*(\d{1,2})\.(\d{1,2})\.?(\d{4})?\s*$/', $text, $matches)) {
      return self::createFromValues(isset($matches[3]) ? $matches[3] : 0, $matches[2], $matches[1]);
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
  public static function createFromValues($year, $month, $day) {
    // All parameters are integers.
    $year = intval($year);
    $month = intval($month);
    $day = intval($day);

    // Validate month.
    if ($month < 1 || $month > 12) {
      throw new InvalidArgumentException(t("%month is not a valid month.", array('%month' => $month)));
    }

    // The maximum days of a month, in a leap year. Indexed by month.
    $maxdays = array(1 => 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);

    if ($year) {
      // Adjust year if only 2 digits are given.
      if ($year > 70 && $year <= 99) {
        $year += 1900;
      }
      else if ($year > 0 && $year <= 70) {
        $year += 2000;
      }

      // Check if it's in the past more than the world record or in the future.
      if ($year < date('Y') - 115 || $year > date('Y')) {
        throw new InvalidArgumentException(t("%year is not a valid birthday year.", array('%year' => $year)));
      }

      // Februrary has only 28 days if it's not a leap year.
      if (!($year % 400 == 0 || ($year % 4 == 0 && $year % 100 != 0))) {
        $maxdays[2] = 28;
      }
    }
    else {
      // 1800 means "no year given" in the database.
      $year = 1800;
    }

    // Validate day.
    if ($day < 1 || $day > $maxdays[$month]) {
      throw new InvalidArgumentException(t("%day is not a valid day of month %month", array('%day' => $day, '%month' => $month)));
    }

    return self::createFromDatabaseValue(mktime(
      1, // hour
      0, // minute
      0, // secound,
      $month, // month,
      $day, // day
      $year // year
    ));
  }

  /**
   * Create a birthday from a database value.
   *
   * @param $value
   *   Database values are unix timestamps, where the year 1800 means no year.
   *
   * @throws InvalidArgumentException when FALSE is given.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function createFromDatabaseValue($value) {
    if ($value === FALSE) {
      throw new InvalidArgumentException(t("FALSE is not a valid database value."));
    }

    return new BirthdaysBirthday($value);
  }

  /**
   * Converts the value to a string.
   *
   * @returns A string representation of the value.
   */
  public function toString() {
    if ($this->value === NULL) {
      return '';
    }

    if (date('Y', $this->value) == 1800) {
      return date('d.m.', $this->value);
    }
    else {
      return date('d.m.Y', $this->value);
    }
  }

  /**
   * Converts the value to an associative array.
   *
   * @return
   *   An associative array with these indexes:
   *    - year (4 digits or FALSE)
   *    - month (1 to 12)
   *    - day (1 to 31)
   */
  public function toArray() {
    $result = array(
      'year' => intval(date('Y', $this->value)),
      'month' => intval(date('m', $this->value)),
      'day' => intval(date('d', $this->value)),
    );

    if ($result['year'] == 1800) {
      $result['year'] = FALSE;
    }
  }

  /**
   * Coverts the value to a database value.
   *
   * @return
   *   A unixtimestamp where the year 1800 means no year, as it is stored in
   *   the database. Or NULL.
   */
  public function toDatabaseValue() {
    return $this->value;
  }

  /**
   * Whether the value is empty or not.
   *
   * @return
   *   TRUE if the value is empty.
   */
  public function isEmpty() {
    return $this->value === NULL;
  }
}

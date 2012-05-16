<?php

/**
 * @file
 * The BirthdaysBirthday class.
 */

/**
 * Converts between different data representations and do calculations on a
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
   * Creates an empty birthday value.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function fromEmpty() {
    return new BirthdaysBirthday();
  }

  /**
   * Creates a birthday from values.
   *
   * @param $year
   *   The year or empty for no year.
   * @param $month
   *   The month (1 to 12).
   * @param $day
   *   The day (1 to 31).
   * @param $allow_future
   *   Allow values in the futzre. Default is FALSE.
   *
   * @throws InvalidArgumentException when one of the given values is invalid.
   */
  public static function fromDate($year, $month, $day, $allow_future = FALSE) {
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
      if (!$allow_future) {
        $unixtime = mktime(1, 0, 0, $month, $day, $year);
        if ($unixtime >= REQUEST_TIME) {
          throw new InvalidArgumentException(t("The given date is in the future."));
        }
      }
    }

    // Validate day.
    if ($day < 1 || $day > $maxdays[$month]) {
      throw new InvalidArgumentException(t("%day is not a valid day of month %month", array('%day' => $day, '%month' => $month)));
    }

    return new BirthdaysBirthday(array('year' => $year, 'month' => $month, 'day' => $day));
  }

  /**
   * Creates a birthday from a database value.
   *
   * @param $value
   *   An associative array with year, month and day set.
   * @param $allow_future
   *   Allow values in the future. Defaults to FALSE.
   *
   * @throws InvalidArgumentException when FALSE is given.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function fromArray($value, $allow_future = FALSE) {
    if (!is_array($value)) {
      throw new InvalidArgumentException(t("Must be an array."));
    }

    if (!isset($value['year'])) {
      $value['year'] = 0;
    }

    if (!isset($value['day']) || !isset($value['month'])) {
      throw new InvalidArgumentException(t('Day and month must be set.'));
    }

    return self::fromDate($value['year'], $value['month'], $value['day'], $allow_future);
  }

  /**
   * Creates a BirthdaysBirthday instance from an offset of dates from the
   * current time.
   * That does not include a year.
   *
   * @param $offset
   *   The date offset as a signed integer.
   *
   * @return
   *   A BirthdaysBirthday instance.
   */
  public static function fromOffset($offset) {
    $time = REQUEST_TIME + intval($offset) * 24 * 60 * 60;
    return self::fromDate(0, date('m', $time), date('d', $time));
  }

  /**
   * Converts the value to a string.
   *
   * @returns A string representation of the value.
   */
  public function toString($format = 'Y/m/d', $format_noyear = 'M d') {
    if ($this->isEmpty()) {
      return '';
    }

    // Remove the year from the format, if none is given.
    if (empty($this->value['year'])) {
      $format_noyear = str_replace('[starsign]', '[s\tarsig\n]', $format_noyear);
      $format_noyear = preg_replace('/([^djSFmMnt\\\])/', '\\\${1}', $format_noyear);
      $result = format_date($this->toUnixtime(), 'custom', $format_noyear);
    }
    else {
      $format = str_replace('[starsign]', '[s\tarsig\n]', $format);
      $format = str_replace('[age]', $this->getCurrentAge(), $format);
      $format = preg_replace('/([^dDjlNSwzWFmMntLoYy\\\])/', '\\\${1}', $format);
      $result = format_date($this->toUnixtime(), 'custom', $format);
    }

    return str_replace('[starsign]', t(ucfirst($this->getStarsign())), $result);
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
   * Coverts the value to a unix timestamp, 0:00:00 at the given date. Note
   * that this depends on the timezone.
   *
   * @return
   *   A unix timestamp where the year 2000 means no year, but could very
   *   well mean 2000 itself. So be careful or only use it to format dates
   *   with day and month. Or NULL.
   */
  public function toUnixtime() {
    if ($this->isEmpty()) {
      return NULL;
    }

    return mktime(
      0, // hour
      0, // minute
      0, // secound
      $this->value['month'],
      $this->value['day'],
      $this->value['year'] ? $this->value['year'] : 2000
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
   * Calculates the age for a specific year.
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

  /**
   * Calculates the current age.
   *
   * @return
   *   The current age.
   */
  public function getCurrentAge($now = REQUEST_TIME) {
    if ($this->value['year'] == 0) {
      return t('Age unkown');
    }

    $months_diff = date('m', $now) - $this->value['month'];

    if ($months_diff < 0) {
      return $this->getAge(date('Y', $now)) - 1;
    }
    elseif ($months_diff == 0) {
      if ($this->value['day'] > date('d', $now)) {
        return $this->getAge(date('Y', $now)) - 1;
      }
    }

    return $this->getAge(date('Y', $now));
  }

  /**
   * @return The year or 0.
   */
  function getYear() {
    return $this->value['year'];
  }

  /**
   * @retrun The month or 0.
   */
  function getMonth() {
    return $this->value['month'];
  }

  /**
   * @return The day or 0.
   */
  function getDay() {
    return $this->value['day'];
  }

  /**
   * Get the starsign.
   *
   * @return
   *   The latin name of the western starsign or NULL.
   */
  function getStarsign() {
    $day = $this->value['day'];

    // Find the matching starsign.
    switch ($this->value['month']) {
      case 1:
        return $day < 20 ? 'capricorn' : 'aquarius';
      case 2:
        return $day < 19 ? 'aquarius' : 'pisces';
      case 3:
        return $day < 21 ? 'pisces' : 'aries';
      case 4:
        return $day < 20 ? 'aries' : 'taurus';
      case 5:
        return $day < 21 ? 'taurus' : 'gemini';
      case 6:
        return $day < 22 ? 'gemini' : 'cancer';
      case 7:
        return $day < 23 ? 'cancer' : 'leo';
      case 8:
        return $day < 23 ? 'leo' : 'virgo';
      case 9:
        return $day < 23 ? 'virgo' : 'libra';
      case 10:
        return $day < 23 ? 'libra' : 'scorpio';
      case 11:
        return $day < 23 ? 'scorpio' : 'sagittarius';
      case 12:
        return $day < 22 ? 'sagittarius' : 'capricorn';
    }
  }

  /**
   * @return
   *   A list with starsign encapsulated in t(), so that they can be translated.
   */
  private function humanReadableStarsigns() {
    return array(t('Capricorn'), t('Aquarius'), t('Pisces'), t('Aries'), t('Taurus'), t('Gemini'), t('Cancer'), t('Leo'), t('Virgo'), t('Libra'), t('Scorpio'), t('Sagittarius'));
  }

  /**
   * Checks if a given year is a leap year.
   *
   * @param $year
   *   The year.
   *
   * @return
   *   TRUE if it is a leap year.
   */
  public function isLeapYear($year) {
    return (bool) date('L', strtotime($year . '-1-1'));
  }
}

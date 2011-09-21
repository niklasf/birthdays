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
   *   Parse dates with different formats in a generic way.
   *
   * @param $text
   *   The text representation of a birthday.
   * @param $allow_future
   *   Allow valeus in the future. Default is FALSE.
   *
   * @throws InvalidArgumentException when the given text is invalid.
   *
   * @return A BirthdaysBirthday instance.
   */
  public static function fromString($text, $allow_future = FALSE) {
    $text = trim($text);
    if ($text === '') {
      return self::fromEmpty();
    }

    // Match dd.mm(.yyyy).
    if (preg_match('/^(\d{1,2})\.(\d{1,2})\.?(\d{4}|\d{2})?$/', $text, $matches)) {
      return self::fromDate(isset($matches[3]) ? $matches[3] : 0, $matches[2], $matches[1], $allow_future);
    }

    // Match (yyyy-)mm-dd or (yyyy/)mm/dd.
    foreach(array('\/', '-') as $sep) {
      if (preg_match('/^((?P<year>\d{4}|\d{2})' . $sep . ')?(?P<month>\d{1,2})' . $sep . '(?P<day>\d{1,2})/', $text, $matches)) {
        return self::fromArray($matches, $allow_future);
      }
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
        if ($unixtime >= time()) {
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
   * Create a birthday from a database value.
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
   * Create a BirthdaysBirthday instance from an offset of dates from the
   * current time.
   * That does not include a year.
   *
   * @param $offset
   *   The date offset as a signed integer.
   *
   * @return
   *   A BirthdaysBirthday instance.
   */
  public function fromOffset($offset) {
    $time = time() + intval($offset) * 24 * 60 * 60;
    return self::fromDate(0, date('m', $time), date('d', $time));
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
   * Coverts the value to a unix timestamp, 0:00:00 at the given date. Note
   * that this depends on the timezone.
   *
   * @return
   *   A unix timestamp where the year 1800 means no year. Or NULL.
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

  /**
   * Calculate the current age.
   *
   * @return
   *   The current age.
   */
  public function getCurrentAge() {
    if ($this->value['year'] == 0) {
      return t('Age unkown');
    }

    $time = time();
    $months_diff = date('m', $time) - $this->value['month'];

    if ($months_diff < 0) {
      return $this->getAge(date('Y', $time)) - 1;
    }
    elseif ($months_diff == 0) {
      if ($this->value['day'] < date('d')) {
        return $this->getAge(date('Y', $time)) - 1;
      }
    }

    return $this->getAge(date('Y', $time));
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
   * Reduces a date format string i.e. "dd.mm.YYY H:i:s" to a date format
   * string that has no unavailable tokens. That is "dd.mm.YYY", because we are
   * dealing with pure date and have no hours, minutes and secounds.
   *
   * @param $year
   *   Defaults to TRUE. Whether tokens that need a year are aloud.
   *
   * @return
   *   The new date format string.
   */
  public static function reduceDateFormat($format, $allow_year = TRUE) {
    // An array of all the available tokens of the date() function. TRUE means
    // available, FALSE means not available.
    $tokens = array(
      'd' => TRUE,
      'D' => TRUE, // needs year
      'j' => TRUE,
      'l' => TRUE, // needs year
      'N' => TRUE, // needs year
      'S' => TRUE,
      'w' => TRUE, // needs year
      'z' => TRUE, // needs year
      'W' => TRUE, // needs year
      'F' => TRUE,
      'm' => TRUE,
      'M' => TRUE,
      'n' => TRUE,
      't' => TRUE, // needs year
      'L' => TRUE, // needs year
      'o' => TRUE, // needs year
      'Y' => TRUE, // needs year
      'y' => TRUE, // needs year
      'a' => FALSE,
      'A' => FALSE,
      'B' => FALSE,
      'g' => FALSE,
      'G' => FALSE,
      'h' => FALSE,
      'H' => FALSE,
      'i' => FALSE,
      's' => FALSE,
      'u' => FALSE,
      'e' => TRUE,
      'I' => TRUE,
      'O' => TRUE,
      'P' => TRUE,
      'T' => TRUE,
      'Z' => TRUE,
      'c' => TRUE, // needs year
      'r' => TRUE, // needs year
      'U' => TRUE, // needs year
    );

    // If no year is present, more tokens are not aloud.
    if (!$allow_year) {
      $tokens = array(
        'D' => FALSE,
        'l' => FALSE,
        'N' => FALSE,
        'w' => FALSE,
        'z' => FALSE,
        'W' => FALSE,
        't' => FALSE,
        'L' => FALSE,
        'o' => FALSE,
        'Y' => FALSE,
        'y' => FALSE,
        'c' => FALSE,
        'r' => FALSE,
        'U' => FALSE,
      ) + $tokens;
    }

    // Split the date format into parts.
    $parts = preg_split('/\s+/', $format);
    $new_parts = array();

    // One time pass parsing the parts.
    foreach ($parts as $part) {
      $escape = FALSE;
      $remove = FALSE;
      $new_part =  '';

      for ($i = 0; $i < strlen($part); $i++) {
        if ($escape) {
          if (!$remove) {
            $new_part .= $part[$i];
          }
        }
        else {
          if ($part[$i] == '\\') {
            $escape = TRUE;
            if (!$remove) {
              $new_part .= '\\';
            }
          }
          elseif (!isset($tokens[$part[$i]])) {
            if (!$remove) {
              $new_part .= $part[$i];
            }
          }
          elseif ($tokens[$part[$i]]) {
            $remove = FALSE;
            $new_part .= $part[$i];
          }
          else {
            $remove = TRUE;
          }
        }
      }

      if ($new_part !== '') {
        $new_parts[] = $new_part;
      }
    }

    // Implode the result.
    return implode(' ', $new_parts);
  }
}

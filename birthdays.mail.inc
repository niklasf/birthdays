<?php
// $Id$
/**
 * @file
 * This file contains the files necessary to e-mail the administrator his
 * periodical listings and/or the users on their birthday.
 */


/**
 * Implementation of hook_mail().
 */
function birthdays_mail($key, &$message, $params) {
  global $_birthdays_field;

  $language = $message['language'];

  switch ($key) {
    // Send message to administrator.
    case 'admin_message':
      $message['subject'] = t('Upcoming Birthdays');
      switch ($params['period']) {
        case BIRTHDAYS_ADMIN_MAIL_DAILY:
          $period = t('Today');
          break;
        case BIRTHDAYS_ADMIN_MAIL_WEEKLY:
          $period = t('This week');
          break;
        case BIRTHDAYS_ADMIN_MAIL_MONTHLY:
          $period = t('This month');
          break;
      }
      $message['body'][] = t('@period, the following users are having their birthdays:', array('@period' => $period), $language->language);

      // Build list of users
      foreach ($params['accounts'] as $uid) {
        $account = user_load(array('uid' => $uid));
        $account->{$_birthdays_field->name}['year'] = NULL; // Don't show the year
        // Calculate the age to be.
        $age = $account->age + !($account->{$_birthdays_field->name}['day'] == format_date(time(), 'custom', 'j') && $account->{$_birthdays_field->name}['month'] == format_date(time(), 'custom', 'n'));
        $message['body'][] = _birthdays_show_date($account->{$_birthdays_field->name}, $account) . ': ' . $account->name . ' (' . $age. ')';
      }
      break;
    // Send message to user.
    case 'user_message':
      $message['subject'] = variable_get('birthdays_send_user_subject', t('Happy Birthday!'));

      $message['body'][] = strtr(variable_get('birthdays_send_user_message', t("Hey @name,\nHappy birthday!\nHope you have a great day!"), array(), $language), array('@name' => check_plain($account->name)));
      break;
  }
}

/**
 * Sends e-mail to administrator once a day as reminder about the upcoming 7 days
 */
function _birthdays_send_admin_message($days = 1) {
  global $user;
  // Only proceed when admin messages are enabled
  if (variable_get('birthdays_remind', BIRTHDAYS_ADMIN_MAIL_DISABLED) != BIRTHDAYS_ADMIN_MAIL_DISABLED) {
    // Get birthdays
    $accounts = birthdays_get_birthdays_by_days($days);

    // If there were any users:
    if (count($accounts) > 0) {
      // Get site e-mail to send reminder to and from
      $to = variable_get('site_mail', ini_get('sendmail_from'));
      // Load the accounts into the parameters;
      $params['accounts'] = $accounts;
      $params['period'] = variable_get('birthdays_remind', BIRTHDAYS_ADMIN_MAIL_DISABLED);
      // Send the mail
      drupal_mail('birthdays', 'admin_message', $to, user_preferred_language($user), $params);
      // Log action
      watchdog('Birthdays', 'Sent birthday overview e-mail to admin', array(), WATCHDOG_NOTICE, '&#160;');
    }
  }
}


/**
 * Send all birthdays on this day a message
 *
 * @todo re-introduce the postcard module to make it fun
 */
function _birthdays_send_user_message() {
  // If messaging is enabled
  if (variable_get('birthdays_send_user', BIRTHDAYS_USER_MAIL_NO) != BIRTHDAYS_USER_MAIL_NO) {
    // Get all users having their birthday today
    $accounts = birthdays_get_todays_birthdays();

    foreach ($accounts as $uid) {
      // Load user
      $account = user_load(array('uid' => $uid));
      // If user and/or administrator allows sending messages
      if (variable_get('birthdays_send_user', BIRTHDAYS_USER_MAIL_NO) == BIRTHDAYS_USER_MAIL_YES || (variable_get('birthdays_send_user', BIRTHDAYS_USER_MAIL_NO) == BIRTHDAYS_USER_MAIL_USER && $account->birthdays_user_send_mail == BIRTHDAYS_USER_MAIL_USER_YES)) {
        $to = $account->name .'<'. $account->mail .'>';
        $params['account'] = $account;

        // Send mail
        drupal_mail('birthdays', 'user_message', $to, user_preferred_language($account), $params);
        // Log actions
        watchdog('Birthdays', 'Sent @name a birthday e-mail', array('@name' => $account->name), WATCHDOG_NOTICE, '&#160;');
      }
    }
  }
}

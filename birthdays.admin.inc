<?php
// $Id$
/**
 * @file
 * All functions related to the birthdays adminstration
 */

/**
 * Implementation of hook_form().
 */
function birthdays_admin_settings() {
  // Get fields from the profile.module of the type 'date'.
  $options = _birthdays_get_date_fields();

  // If there aren't any 'date' fields, throw an error and return an empty form
  if (empty($options)) {
    drupal_set_message(t('No profile fields of type \'date\' were found, please <a href="@profile-settings-page">create one here</a>.', array('@profile-settings-page' => url('admin/user/profile/add/date'))), 'error');
    return;
  }

  $options = array('') + $options;
  // Fieldset for general settings
  $form['birthdays_general_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['birthdays_general_settings']['birthdays_field_id'] = array(
    '#type' => 'select',
    '#title' => t('Profile field'),
    '#default_value' => variable_get('birthdays_field_id', NULL),
    '#description' => t("Select which profile field of type 'date' you want to use as date of birth."),
    '#options' => $options,
    '#required' => TRUE,
  );

  $form['birthdays_general_settings']['birthdays_show_starsign'] = array(
    '#type' => 'select',
    '#title' => t('Show star signs'),
    '#default_value' => variable_get('birthdays_show_starsign', BIRTHDAYS_STARSIGN_OFF),
    '#options' => array(
      BIRTHDAYS_STARSIGN_OFF => t('No'),
      BIRTHDAYS_STARSIGN_LINK => t('Yes, with link to Yahoo Astrology'),
      BIRTHDAYS_STARSIGN_NOLINK => t('Yes, without link to Yahoo Astrology')
    ),
    '#description' => t('Select whether the star signs should be enabled.'),
  );

  $form['birthdays_general_settings']['birthdays_hide_year'] = array(
    '#type' => 'select',
    '#title' => t('Hide year and age'),
    '#default_value' => variable_get('birthdays_hide_year', BIRTHDAYS_HIDE_YEAR_NO),
    '#description' => t('Select whether the birth year and age should be shown.'),
    '#options' => array(
      BIRTHDAYS_HIDE_YEAR_NO => t('No'),
      BIRTHDAYS_HIDE_YEAR_YES => t('Yes'),
      BIRTHDAYS_HIDE_YEAR_USER => t("User optional, 'No' by default"),
    ),
  );

  // Fieldset for the birthdays page.
  $form['birthdays_page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Birthdays page settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['birthdays_page']['birthdays_page_settings'] = array(
    '#type' => 'select',
    '#title' => t('Set birthdays page settings'),
    '#default_value' => variable_get('birthdays_page_settings', BIRTHDAYS_PAGE_FILTER_SORT_DATE),
    '#description' => t("Select whether users that haven't entered their date of birth should be shown on the birthdays page, and whether the list should be ordered by birthday or by username."),
    '#options' => array(
      BIRTHDAYS_PAGE_FILTER_SORT_DATE => t('Filter users, sort by date'),
      BIRTHDAYS_PAGE_FILTER_SORT_USER => t('Filter users, sort by username'),
      BIRTHDAYS_PAGE_NOFILTER_SORT_USER => t('Do not filter users, sort by username'),
    ),
  );

  $form['birthdays_page']['birthdays_page_list_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Birthdays page size'),
    '#default_value' => variable_get('birthdays_page_list_number', 25),
    '#description' => t('How many users should be listed on each page on the Birthdays listing?'),
    '#required' => TRUE,
    '#size' => 10,
  );

  $form['birthdays_page']['birthdays_page_show_filters'] = array(
    '#type' => 'select',
    '#title' => t('Show filter options'),
    '#default_value' => variable_get('birthdays_page_show_filters', 1),
    '#description' => t("Show the 'Filter by month and year' selections on the birthdays page. When 'Hide year' has been set to 'Yes', the year filter isn't shown."),
    '#options' => array(
      0 => t('No'),
      1 => t('Yes'),
    ),
  );

  // Fieldset for e-mails.
  $form['birthdays_email'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail settings'),
    '#description' => t('Adjust the e-mail settings for both e-mails to users and e-mails to the administrator. This requires cron jobs to be active. Please see the <a href="@cron_url">online handbook</a> for more information on how to accomplish that.', array('@cron_url' => 'http://drupal.org/cron')),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Fieldset for Admin E-mails
  $form['birthdays_email']['admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Admin'),
    '#collapsible' => TRUE,
    '#collapsed' => variable_get('birthdays_remind', BIRTHDAYS_ADMIN_MAIL_DISABLED) == BIRTHDAYS_ADMIN_MAIL_DISABLED,
  );

  $days = array(0 => t('sunday'), 1 => t('monday'), 2 => t('tuesday'), 3 => t('wednesday'), 4 => t('thursday'), 5 => t('friday'), 6 => t('saturday'));

  $form['birthdays_email']['admin']['birthdays_remind'] = array(
    '#type' => 'select',
    '#title' => t('Send upcoming birthdays to admin'),
    '#default_value' => variable_get('birthdays_remind', BIRTHDAYS_ADMIN_MAIL_DISABLED),
    '#options' => array(
      BIRTHDAYS_ADMIN_MAIL_DISABLED => t('Disabled'),
      BIRTHDAYS_MAIL_FREQUENCY_DAYLY => t('Dayly'),
      BIRTHDAYS_MAIL_FREQUENCY_WEEKLY => t('Weekly, on @first_weekday', array('@first_weekday' => $days[variable_get('date_first_day', 0)])),
      BIRTHDAYS_MAIL_FREQUENCY_MONTHLY => t('Monthly'),
    ),
    '#description' => t('Do you want to receive periodic e-mails containing all upcoming birthdays?')
  );

  // Fieldset for User E-mails
  $form['birthdays_email']['users'] = array(
    '#type' => 'fieldset',
    '#title' => t('User'),
    '#collapsible' => TRUE,
    '#collapsed' => variable_get('birthdays_send_user', BIRTHDAYS_USER_MAIL_NO) == BIRTHDAYS_USER_MAIL_NO,
  );

  $form['birthdays_email']['users']['birthdays_send_user'] = array(
    '#type' => 'select',
    '#title' => t('Send user e-mail on day of birth'),
    '#default_value' => variable_get('birthdays_send_user', BIRTHDAYS_USER_MAIL_NO),
    '#options' => array(
      BIRTHDAYS_USER_MAIL_NO => t('No'),
      BIRTHDAYS_USER_MAIL_YES => t('Yes'),
      BIRTHDAYS_USER_MAIL_USER => t("User optional, 'Yes' by default"),
    ),
    '#description' => t('Should users that have their birthday today receive an e-mail?'),
  );

  $form['birthdays_email']['users']['birthdays_send_user_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('birthdays_send_user_subject', t('Happy Birthday!')),
  );

  $form['birthdays_email']['users']['birthdays_send_user_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#default_value' => variable_get('birthdays_send_user_message', t("Hey @name,
Happy birthday!
Hope you have a great day!")
    ),
    '#description' => t('@name will be replaced by the username.'),
  );


  return system_settings_form($form);
}

/**
 * Implementation of hook_form().
 */
function birthdays_sync_form() {
  global $_birthdays_field;

  if (!isset($_birthdays_field)) {
    drupal_set_message(t('No date field has been selected as birthdays field. Please visit the <a href="@birthdays-settings">birthdays settings page</a>.', array('@birthdays-settings' => url('admin/settings/birthdays'))), 'warning');
    return array();
  }
  $form['profile_fieldset'] = array('#type' => 'fieldset');

  $form['profile_fieldset']['description'] = array(
    '#type' => 'item',
    '#title' => 'Profile to Birthdays',
    '#value' => t("Fill the Birthdays module's table with data from the Profile module's table. This is needed when you install the Birthdays module, but already collected birthdays with the Profile module (e.g. for age verification, to show in the profile or after uninstalling the Birthdays module)."),
  );

  $form['profile_fieldset']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Copy Profile data to Birthdays module'),
    '#submit' => array('birthdays_sync_profiles_to_birthdays'),
  );

  $form['birthdays_fieldset'] = array('#type' => 'fieldset');

  $form['birthdays_fieldset']['description'] = array(
    '#type' => 'item',
    '#title' => 'Birthdays to Profile',
    '#value' => t("Fill the Profile module's table with data from the Birthdays module's table. You can use this if you completely reinstalled the Profile module while leaving the Birthdays module alone, or to copy the old data collected by a version of the Birthdays module which didn't use the Profile module (pre-5.x)."),
  );

  $form['birthdays_fieldset']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Copy Birthdays data to Profile module'),
    '#submit' => array('birthdays_sync_birthdays_to_profiles'),
  );

  return $form;
}

function birthdays_sync_profiles_to_birthdays($form, &$form_state) {
  global $_birthdays_field;

  $result = db_query('SELECT uid FROM {users} WHERE 1');

  while ($uid = db_fetch_object($result)) {
    drupal_set_message($uid);
    $account = user_load(array('uid' => $uid->uid));

    if ($account->{$_birthdays_field->name}) {

      user_save($account, array($_birthdays_field->name => $account->{$_birthdays_field->name}));
    }
  }

  drupal_set_message(t('Birthdays have been synchronized.'));
}

function birthdays_sync_birthdays_to_profiles($form, &$form_state) {
  global $_birthdays_field;

  switch ($GLOBALS['db_type']) {
    case 'mysql':
    case 'mysqli':
      $result = db_query('SELECT {dob}.uid, MONTH({dob}.birthday) AS month, YEAR({dob}.birthday) AS year, DAYOFMONTH({dob}.birthday) AS day FROM {dob}');
      break;
    case 'pgsql':
      $result = db_query("SELECT uid, date_part('month',{dob}.birthday) AS month, date_part('year',{dob}.birthday) AS year, date_part('day',{dob}.birthday) AS day FROM {dob}");
      break;
  }

  while ($birthday = db_fetch_object($result)) {
    $account = user_load(array('uid' => $birthday->uid));
    $dob = array(
      'day' => $birthday->day,
      'month' => $birthday->month,
      'year' => $birthday->year
    );
    user_save($account, array($_birthdays_field->name => $dob));
  }

  drupal_set_message(t('Birthdays have been synchronized.'));
}

<?php
/**
 * @file
 * Because the Birthdays module uses 2 tables for the birthdays, they have
 * to be kept in sync with each other. Normally that will be no problem, but
 * in some cases there is the need to manually synchronize the two tables.
 * These cases are for example:
 * - The initial installation on a site which already used the Profile module
 *   to collect data.
 * - Temporarily disabling the Birthdays module while keeping the profile
 *   field active.
 * - Completely reinstalling the Profile module.
 * - Crashed or changed database tables.
 */

/**
 * Creates the synchronization form.
 */
function birthdays_sync_form() {
  global $_birthdays_field;

  // We can't synchronize if the field hasn't been set yet.
  if (!isset($_birthdays_field)) {
    drupal_set_message(t('No date field has been selected as birthdays field. Please visit the <a href="@birthdays-settings">birthdays settings page</a>.', array('@birthdays-settings' => url('admin/settings/birthdays'))), 'warning');
    return array();
  }
  $form['profile_fieldset'] = array('#type' => 'fieldset');

  $form['profile_fieldset']['description'] = array(
    '#type' => 'item',
    '#title' => t('Profile to Birthdays'),
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
    '#title' => t('Birthdays to Profile'),
    '#value' => t("Fill the Profile module's table with data from the Birthdays module's table. You can use this if you completely reinstalled the Profile module while leaving the Birthdays module alone, or to copy the old data collected by a version of the Birthdays module which didn't use the Profile module (pre-5.x)."),
  );

  $form['birthdays_fieldset']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Copy Birthdays data to Profile module'),
    '#submit' => array('birthdays_sync_birthdays_to_profiles'),
  );

  return $form;
}

/**
 * Profile to Birthdays submit handler.
 */
function birthdays_sync_profiles_to_birthdays($form, &$form_state) {
  birthdays_set_batch(t('Copy Profile data to Birthdays module'), 'batch_birthdays_sync_p2b');
}

/**
 * Birthdays to Profile submit handler.
 */
function birthdays_sync_birthdays_to_profiles($form, &$form_state) {
  birthdays_set_batch(t('Copy Birthdays data to Profile module.'), 'batch_birthdays_sync_b2p');
}

/**
 * Set the batch for either synchronisation processes.
 */
function birthdays_set_batch($title, $operation_callback) {
  global $_birthdays_field;

  $batch = array(
    'title' => $title,
    'operations' => array(
        array($operation_callback, array($_birthdays_field)),
    ),
    'finished' => 'batch_birthdays_sync_finished',
    'init_message' => t('Synchronization is starting.'),
    'progress_message' => t('Synchronizing'),
    'file' => drupal_get_path('module', 'birthdays'). '/birthdays.sync.inc',
  );

  // Set the batch. It will start immediately, because it is called from a
  // submit handler.
  batch_set($batch);
}

/**
 * Batch finished synchronizing. Display an apropriate message.
 */
function batch_birthdays_sync_finished($success, $results, $operations) {
  if($success) {
    drupal_set_message(t('Birthdays have been synchronized.'));
  }
  else {
    drupal_set_message(t('The birthdays have not been synchronized due to an error.'));
  }
}

/**
 * Profile to Birthdays batch copying process.
 */
function batch_birthdays_sync_p2b($field, &$context) {
  // First run, set up the sandbox.
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_user'] = 0;
    // Count how many users are going to be processed.
    $context['sandbox']['max'] = db_result(db_query('SELECT COUNT(DISTINCT uid) FROM {users} WHERE uid > 0'));
  }

  // I'm not sure how many a slow server can process within the batch execution
  // limit, but 25 seems not too much. Faster machines won't notice the limit
  // thanks to the Batch API.
  $limit = 25;

  // Get next batch of users.
  $result = db_query_range('SELECT uid FROM {users} WHERE uid > %d ORDER BY uid ASC', $context['sandbox']['current_user'], 0, $limit);

  // For each user id.
  while ($uid = db_fetch_object($result)) {
    // Get the user object.
    $account = user_load(array('uid' => $uid->uid));
    // If the date of birth was set
    if ($account->{$field->name}) {
      // Save it, so birthdays_user() will be called and all necessary actions
      // will be performed.
      user_save($account, array($field->name => $account->{$field->name}));
    }

    // Updating the sandbox.
    $context['sandbox']['progress']++;
    $context['sandbox']['current_user'] = $account->uid;
  }

  // Update the message.
  $context['message'] = t('Synchronized @current of @total users.', array('@current' => $context['sandbox']['progress'], '@total' => $context['sandbox']['max']));

  // If not yet finished, calculate the proportion of items that are.
  if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Birthdays to Profile batch copying process
 */
function batch_birthdays_sync_b2p($field, &$context) {
  // First run, set up the sandbox.
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_user'] = 0;
    $context['sandbox']['max'] = db_result(db_query('SELECT COUNT(uid) FROM {dob}'));
  }

  // I'm not sure how many a slow server can process within the batch execution
  // limit, but 25 seems not too much. Faster machines won't notice the limit
  // thanks to the Batch API.
  $limit = 25;

  // Different syntax for MySQL and Postgresql when getting  theday, month and
  // year parts of the date of birth. Only get the next batch of birthdays.
  switch ($GLOBALS['db_type']) {
    case 'mysql':
    case 'mysqli':
      $result = db_query_range("SELECT uid, MONTH(birthday) AS month, YEAR(birthday) AS year, DAYOFMONTH(birthday) AS day FROM {dob} WHERE uid > %d ORDER BY uid ASC", $context['sandbox']['current_user'], 0, $limit);
      break;
    case 'pgsql':
      $result = db_query_range("SELECT uid, date_part('month',birthday) AS month, date_part('year',birthday) AS year, date_part('day', birthday) AS day FROM {dob} WHERE uid > %d ORDER BY uid ASC", $context['sandbox']['current_user'], 0, $limit);
      break;
  }

  // For each date of birth.
  while ($birthday = db_fetch_object($result)) {
    // Get the user object.
    $account = user_load(array('uid' => $birthday->uid));
    // Make the date of birth array.
    $dob = array(
      'day' => $birthday->day,
      'month' => $birthday->month,
      'year' => $birthday->year
    );
    // And save the user.
    user_save($account, array($field->name => $dob), $field->category);

    // Updating the sandbox.
    $context['sandbox']['progress']++;
    $context['sandbox']['current_user'] = $account->uid;
  }

  // Update the message.
  $context['message'] = t('Synchronized @current of @total birthdays.', array('@current' => $context['sandbox']['progress'], '@total' => $context['sandbox']['max']));

  // If not yet finished, calculate the proportion of items that are.
  if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

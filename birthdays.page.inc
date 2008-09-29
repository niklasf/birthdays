<?php
// $Id$
/**
 * @file
 * All functions related to the birthdays listings page
 */

/**
 * Show birthdays page
 * @desc Callback for birthdays menu item to show a page which lists all users.
 * @return string
 *   Themed birthday listing of all users
 */
function birthdays_view_page() {
  global $_birthdays_field;
  // Nothing to see when the birthdays profile field hasn't been set yet.
  if (!isset($_birthdays_field)) {
    return NULL;
  }
  drupal_add_css(drupal_get_path('module', 'birthdays') .'/birthdays.css');
  if (isset($_REQUEST['birthdays_filter_month'])) {
    $filter_month = (int) $_REQUEST['birthdays_filter_month'];
  }
  if (isset($_REQUEST['birthdays_filter_year'])) {
    $filter_year = (int) $_REQUEST['birthdays_filter_year'];
  }
  $output = '';
  $filter = '';

  switch ($GLOBALS['db_type']) {
    case 'mysql':
    case 'mysqli':
      // Right join = filtering out
      $join = variable_get('birthdays_page_settings', BIRTHDAYS_PAGE_FILTER_SORT_DATE) != BIRTHDAYS_PAGE_NOFILTER_USER || !empty($filter_month) || !empty($filter_year) ? 'RIGHT JOIN' : 'LEFT JOIN';
      $sort = variable_get('birthdays_page_settings', BIRTHDAYS_PAGE_FILTER_SORT_DATE) == BIRTHDAYS_PAGE_FILTER_SORT_DATE || !empty($filter_month) || !empty($filter_year) ? 'MONTH({dob}.birthday), DAYOFMONTH({dob}.birthday), YEAR({dob}.birthday), ' : '';
      $filter .= !empty($filter_month) ? " AND MONTH({dob}.birthday) = ". $filter_month : '';
      $filter .= !empty($filter_year) ? " AND YEAR({dob}.birthday) = ". $filter_year : '';
      break;
    case 'pgsql':
      // Right join = filtering out
      $join = variable_get('birthdays_page_settings', BIRTHDAYS_PAGE_FILTER_SORT_DATE) != BIRTHDAYS_PAGE_NOFILTER_USER || !empty($filter_month) || !empty($filter_year) ? 'RIGHT JOIN' : 'LEFT JOIN';
      $sort = variable_get('birthdays_page_settings', BIRTHDAYS_PAGE_FILTER_SORT_DATE) == BIRTHDAYS_PAGE_FILTER_SORT_DATE || !empty($filter_month) || !empty($filter_year) ? "date_part('month', {dob}.birthday), date_part('day', {dob}.birthday), date_part('year', {dob}.birthday), " : '';
      $filter .= !empty($filter_month) ? " AND date_part('month', {dob}.birthday) = ". $filter_month : '';
      $filter .= !empty($filter_year) ? " AND date_part('year', {dob}.birthday) = ". $filter_year : '';
      break;
  }

  // Select all users (but ignore blocked and never logged in ones)
  $result = pager_query("SELECT {users}.uid FROM {users} ". $join ." {dob} ON {users}.uid = {dob}.uid  WHERE {users}.status <> 0 AND {users}.access <> 0". $filter ." ORDER BY ". $sort ."{users}.name", variable_get('birthdays_page_list_number', 25));

  while ($uid = db_fetch_object($result)) {
    // load the user objects
    $account = user_load(array('uid' => $uid->uid));
    if (empty($filter_year) || !$account->birthdays_user_hide_year) {
      $accounts[] = $account;
    }
  }

  // Call theme_birthdays_page
  $output .= theme('birthdays_page', $accounts, $filter_month, $filter_year);
  // Get the filter form

  return $output;
}


/**
 * Return a form containing a select box to filter users by month of birth
 *
 * @param $filter_month
 * @param int $filter_year
 * @return array of the form
 */
function birthdays_page_filter($filter_month = NULL, $filter_year = NULL) {
  $options_months[0] = '';
  for ($i = 1; $i <= 12; $i++) {
    $options_months[$i] = format_date(gmmktime(0, 0, 0, $i, 2, 1970), 'custom', 'F', 0);
  }

  $options_years[0] = '';
  $options_years = $options_years + drupal_map_assoc(range(1900, 2050));

  $form['#prefix'] = '<div class="container-inline birthdays-filter">';
  $form['#suffix'] = "</div>\n";

  $form['birthdays_filter_month'] = array(
    '#type' => 'select',
    '#options' => $options_months,
    '#default_value' => $filter_month,
    '#title' => t('Filter by month'),
    '#attributes' => array('onchange' => 'submit()'),
  );

  if (variable_get('birthdays_hide_year', BIRTHDAYS_HIDE_YEAR_OFF) != BIRTHDAYS_HIDE_YEAR_ON) {
    $form['birthdays_filter_year'] = array(
      '#type' => 'select',
      '#options' => $options_years,
      '#default_value' => $filter_year,
      '#title' => t('Year'),
      '#attributes' => array('onchange' => 'submit()'),
    );
  }

  $form['button'] = array(
    '#type'   => 'button',
    '#prefix' => '<noscript>',
    '#value'  => t('Filter'),
    '#suffix' => '</noscript>',
  );

  return $form;
}

/**
 * @desc Theme a birthdays page
 * @var $accounts array of loaded user objects
 */
function theme_birthdays_page($accounts = array(), $filter_month, $filter_year) {
  global $_birthdays_field;

  // $header contains the column headers
  $header = array(t('User'), t('Birthday'));

  if (variable_get('birthdays_hide_year', BIRTHDAYS_HIDE_YEAR_OFF) != BIRTHDAYS_HIDE_YEAR_ON) {
    $header[] = t('Age');
  }

  if (variable_get('birthdays_show_starsign', BIRTHDAYS_STARSIGN_OFF)) {
    $header[] = t('Starsign');
  }

  if (variable_get('user_pictures', FALSE)) {
    $header[] = '&nbsp;';
  }

  // $rows contains all cells
  $rows = array();
  $accounts = is_array($accounts) ? $accounts : array();
  $i = 0;
  // For each user
  foreach ($accounts as $account) {
    //Theme username (with link and such) and format date field.
    $rows[$i] = array(theme('username', $account), _birthdays_show_date($account->{$_birthdays_field->name}, $account));

    if (variable_get('birthdays_hide_year', BIRTHDAYS_HIDE_YEAR_OFF) != BIRTHDAYS_HIDE_YEAR_ON) {
      $age = _birthdays_show_age($account);
      $rows[$i][] = isset($age) ? $age : '';
    }

    if (variable_get('birthdays_show_starsign', BIRTHDAYS_STARSIGN_OFF)) {
      $rows[$i][] = birthdays_get_starsign_image($account->birthdays_starsign, variable_get('birthdays_show_starsign', BIRTHDAYS_STARSIGN_OFF));
    }

    if (variable_get('user_pictures', FALSE)) {
      $rows[$i][] = theme('user_picture', $account);
    }

    $i++;
  }

  $output = '';
  // Return a table. Everthing else (titles, and such) is handled by the page system.
  if (variable_get('birthdays_page_show_filters', 1)) {
    $output .= drupal_get_form('birthdays_page_filter', $filter_month, $filter_year);
  }
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, variable_get('birthdays_page_list_number', 25), 0);

  return $output;
}

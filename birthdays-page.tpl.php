<?php

/**
 * @file birthdays-page.tpl.php
 * Display a page listing users and their birthdays. Depending on the settings
 * it will be sorted by date of birth or by username. Users without an
 * birthday can be filtered from the listing.
 *
 * Available variables:
 * - $show_starsigns: Boolean indication whether to show the star sign.
 * - $show_year:      Global Boolean indicating whether year & age are enabled.
 * - $show_user_picture: Boolean indicating whether user pictures are enabled.
 * - $show_filter:    Boolean for toggling the page filter form.
 * - $filter_form:    Form to filter by year and month.
 * - $pager:          The pager to browser through the pages.
 * - $birthdays:      Associative array with the following data for each user:
 *   - 'username':      Themed username with link to account.
 *   - 'age':           Current age shown depending on the settings of the
 *                      module or the user's preference.
 *   - 'show_age':      Boolean indicating whether the age should be shown.
 *   - 'date':          Date of birth for this user.
 *   - 'account':       Entire account, for user pictures and other ways to
 *                      improve the birthdays block.
 *   - 'starsign':      Themed star sign.
 *   - 'user_picture':  Themed user picture.
 *   - 'zebra':         Odd or even, for alternating table rows.
 *   -
 */
?>
<?php if ($show_filter): ?>
  <div class="container-inline birthdays-filter"><?php print $filter_form ?></div>
<?php endif ?>
<table class="sticky-enabled">
  <thead><tr>
    <th><?php print t('User') ?></th>
    <th><?php print t('Birthday') ?></th>
    <?php if ($show_year): ?>
      <th><?php print t('Age') ?></th>
    <?php endif; ?>
    <?php if ($show_starsign): ?>
      <th><?php print t('Starsign'); ?></th>
    <?php endif; ?>
    <?php if ($show_user_picture): ?>
      <th>&nbsp;</th>
    <?php endif; ?>
  </tr></thead>
  <tbody>
  <?php foreach ($birthdays as $birthday): ?>
    <tr class="<?php print $birthday['zebra'] ?>">
      <td><?php print $birthday['username'] ?></td>
      <td><?php print $birthday['date'] ? $birthday['date'] : '&nbsp;' ?></td>
      <?php if ($show_year): ?>
        <td><?php print $birthday['show_age'] ? $birthday['age'] : '&nbsp;' ?></td>
      <?php endif; ?>
      <?php if ($show_starsign): ?>
        <td><?php print $birthday['starsign'] ? $birthday['starsign'] : '&nbsp;' ?></td>
      <?php endif; ?>
      <?php if ($show_user_picture): ?>
        <td><?php print $birthday['picture'] ?></td>
      <?php endif; ?>
    </tr>
  <?php endforeach; ?>
  </tbody>
</table>
<?php print $pager ?>

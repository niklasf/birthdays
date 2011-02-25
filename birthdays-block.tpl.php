<?php

/**
 * @file birthdays-block.tpl.php
 * Display the users that are having their birthday, as requested by one of the
 * blocks.
 *
 * Available variables:
 * - $no_birthdays:   Boolean indicating whether there are birthdays.
 * - $show_starsigns: Boolean indication whether to show the star sign.
 * - $amount:         Number of days or users to be shown in the block (depends
 *                    on the type of block).
 * - $block_type:     Which block is shown? Either 'by_days' or 'by_birthdays'.
 * - $more:           Themed link to the Birthdays page.
 * - $birthdays:      Associative array with the following data:
 *   - 'username':      Themed username with link to account.
 *   - 'age':           Age the user will be after his/her birthday, shown
 *                      depending on the settings of the module or the user's
 *                      preference.
 *   - 'date':          Month and day of birth of the user, formatted according
 *                      the system's "short date format".
 *   - 'account':       Entire account, for user pictures and other ways to
 *                      improve the birthdays block.
 *   - 'starsign':      Themed star sign.
 */
?>
<?php if ($no_birthdays): ?>
  <p><?php print t('There are no birthdays soon.'); ?></p>
<?php else: ?>
  <table>
    <?php foreach($birthdays as $birthday): ?>
    <tr>
      <td>
        <?php print $birthday['username'] ?>
        <?php if ($birthday['show_age']): ?>
          &nbsp;<small>(<?php print $birthday['age'] ?>)</small>
        <?php endif; ?>
      </td>
      <td><?php print $birthday['date'] ?></td>
    </tr>
    <?php endforeach; ?>
  </table>
<?php endif; ?>

<? print $more; ?>

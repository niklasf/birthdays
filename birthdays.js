/**
 * @file
 * Datepicker support for the birthdays module.
 */

(function ($) {

Drupal.behaviors.birthdays = {
  attach: function (context) {
    var settings = {
      dateFormat: Drupal.settings.birthdays.dateformat,
      autoSize: true,
      changeMonth: true,
      changeYear: true,
      maxDate: '+0',
      minDate: '-115y',
      yearRange: '-155:+0',
      constrainInput: false,
      firstDay: Drupal.settings.birthdays.firstDay
    };

    var noYearSettings = jQuery.extend({}, settings);
    noYearSettings.dateFormat = Drupal.settings.birthdays.dateformat_noyear;

    $('.birthdays-datepicker', context).datepicker(settings);
    $('.birthdays-datepicker-noyear', context).datepicker(noYearSettings);
  }
};

})(jQuery);

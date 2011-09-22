(function ($) {

Drupal.behaviors.birthdays = {
  attach: function (context) {
    var settings = {
      dateFormat: 'yy/mm/dd',
      autoSize: true,
      changeMonth: true,
      changeYear: true,
      maxDate: '+0',
      minDate: '-115y',
      yearRange: '-115:+0',
      constrainInput: false
    };

    var noYearSettings = jQuery.extend({}, settings);
    noYearSettings.dateFormat = 'mm/dd';

    $('.birthdays-datepicker', context).datepicker(settings);
    $('.birthdays-datepicker-noyear', context).datepicker(noYearSettings);
  }
};

})(jQuery);
